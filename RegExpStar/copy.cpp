//This is a copy of another code

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <set>
#include <map>
#include <queue>

using namespace std;

struct nst{
    vector<int> a[2], e; //a[2]的含义是：
    bool f=0;//标记是不是终止态
    //a[2]的含义是：characters只有a,b两个字符，a[0]
    //输入b后的转移表，e代表空转移。
};

vector<nst> nfa;

struct dst{
    int a[2] = {-1,-1};
    bool f=0;
};

vector<dst> dfa;

stack<int> st;

int nfa_size, dfa_size;

string dispregex;

struct nst init_nfa_state;
struct dst init_dfa_state;

void custom_clear() {
    for(int i=0; i<100; ++i) cout << endl;
}

/***************************** regex to nfa ****************************/

string insert_concat(string regexp){
    string ret="";
    char c,c2;
    for(unsigned int i=0; i<regexp.size(); i++){
        c=regexp[i];
        if(i+1<regexp.size()){
            c2=regexp[i+1];
            ret+=c;
            //此处+代表或的关系，不同于正则语义
            if(c!='('&&c2!=')'&&c!='+'&&c2!='+'&&c2!='*'){
                ret+='.';
            }
        }
    }
    ret+=regexp[regexp.size()-1];
    return ret;
}

//每添加一个char,添加两个状态
//就相当于向添加两个状态然后再进行状态修改
void character(int i){
    nfa.push_back(init_nfa_state);
    nfa.push_back(init_nfa_state);
    //a[i]表示a还是b以后的跳转
    nfa[nfa_size].a[i].push_back(nfa_size+1);
    st.push(nfa_size);
    ++nfa_size;
    st.push(nfa_size);
    ++nfa_size;
}
//如何理解union 和 concatenation 的方式，关键是re使用后缀树的形式保存的
//num1 num2 oprator   所以union 和 concatenation 就可以理解了。
void union_(){
    int end2 = st.top(); st.pop();
    int start2 = st.top(); st.pop();
    int end1 = st.top(); st.pop();
    int start1 = st.top(); st.pop();

    nfa.push_back(init_nfa_state);
    nfa.push_back(init_nfa_state);

    nfa[nfa_size].e.push_back(start1);
    nfa[nfa_size].e.push_back(start2);
    nfa[end1].e.push_back(nfa_size+1);
    nfa[end2].e.push_back(nfa_size+1);
    
    st.push(nfa_size);
    ++nfa_size;
    st.push(nfa_size);
    ++nfa_size;
}

void concatenation(){
    //connect 2 state is ok, then put the new start and end to states.
    int end2 = st.top(); st.pop();
    int start2 = st.top(); st.pop();
    int end1 = st.top(); st.pop();
    int start1 = st.top(); st.pop();

    nfa[end1].e.push_back(start2);
    st.push(start1);
    st.push(end2);
}

void kleene_star(){
    nfa.push_back(init_nfa_state);
    nfa.push_back(init_nfa_state);
    
    int end1 = st.top(); st.pop();
    int start1 = st.top(); st.pop();
    nfa[end1].e.push_back(start1);

    nfa[nfa_size].e.push_back(start1);
    nfa[nfa_size].e.push_back(nfa_size+1);
    nfa[end1].e.push_back(nfa_size+1);

    st.push(nfa_size);
    ++nfa_size;
    st.push(nfa_size);
    ++nfa_size;
}

void postfix_to_nfa(string postfix){
    for(int i=0; i<(int)postfix.size(); ++i){
        switch(postfix[i]){
            case 'a':
            case 'b':
                character(postfix[i]-'a'); break;
            case '*': kleene_star(); break;
            case '+': union_(); break;
            case '.': concatenation(); break;
        }
    }
}

void display_nfa()
{
    cout<<endl<<endl;
    cout<<"Phase 1: regex to nfa conversion using thompson's construction algorithm\n";
    cout<<"------------------------------------------------------------------------\n";
    cout<<"State\t|\ta\t|\tb\t|\teps\t|accepting state|"<<endl;
    cout<<"------------------------------------------------------------------------\n";
    for(unsigned int i=0; i<nfa.size(); i++)
    {
        cout<<i<<"\t|\t";
        for(unsigned int j=0; j<nfa[i].a[0].size(); j++)cout<<nfa[i].a[0][j]<<' ';
        cout<<"\t|\t";
        for(unsigned int j=0; j<nfa[i].a[1].size(); j++)cout<<nfa[i].a[1][j]<<' ';
        cout<<"\t|\t";
        for(unsigned int j=0; j<nfa[i].e.size(); j++)cout<<nfa[i].e[j]<<' ';
        cout<<"\t|\t";
        if(nfa[i].f)cout<<"Yes"; else cout<<"No";
        cout<<"\t|\n";
    }
    cout<<"------------------------------------------------------------------------\n";
}

int priority(char c){
    switch(c){
        case '*': return 3;
        case '.': return 2;
        case '+': return 1;
        default: return 0;
    }
}

//将正则表达式转化为后缀表达式，需要一个符号栈来处理不同符号的优先级
string regexp_to_postfix(string regexp){
    string postfix = "";
    stack<char> op;
    for(int i=0; i<(int)regexp.size(); ++i){
        switch(char now = regexp[i]){
            case 'a':
            case 'b':
                postfix += now; break;
            case '(':
                op.push(now); break;
            case ')':
                while(op.top() != '('){
                    postfix += op.top();
                    op.pop();
                }
                op.pop();
                break;
            default:
                while(!op.empty()){
                    char c = op.top();
                    if(priority(c) >= priority(now)){
                        postfix += op.top();
                        op.pop();
                    } else break;
                }
                op.push(now);
        }
    }
    while(!op.empty()){
        postfix += op.top();
        op.pop();
    }
    return postfix;    
}

//闭包运算
void epsilon_closure(int state, set<int> &si){
    for(int i=0; i<(int)nfa[state].e.size(); ++i){
        int next = nfa[state].e[i];
        if(si.find(next) == si.end()){
            si.insert(next);
            epsilon_closure(next, si);
        }        
    }
}

//下面的函数对于输入的字符，进行状态转换。
set<int> state_change(int c, set<int> &si){
    set<int> temp;
    int is_a = (c == 0) ? 0 : 1; //0 代表a, 1 代表b
    for(auto it=si.begin(); it != si.end(); ++it){
        for(int i=0; i<(int)nfa[*it].a[is_a].size(); ++i){
            temp.insert(nfa[*it].a[is_a][i]);
        }
    }
    return temp;
}

void nfa_to_dfa(set<int> &si, queue<set<int>> &que, int start_state){
    map<set<int>, int> mp;
    mp[si] = -1; //初始化，最后检测，如果发现，说明是不可达的节点。
    int ct = 0;
    si.clear();
    si.insert(ct);
    epsilon_closure(start_state, si);
    if(mp.find(si) == mp.end()){
        mp[si] = ct;
        que.push(si);
        ct++;
    }
    int p = 0;
    bool f1 = false; 
    while(! que.empty()){
        dfa.push_back(init_dfa_state);
        si = que.front();
        set<int> que_front = si; //save this state.
        f1 = false;
        //判断是否结束状态
        for(auto it=si.begin(); it != si.end(); ++it){
            if(nfa[*it].f == true) f1 = true;
        }
        dfa[p].f = f1;

        for(int i=0; i<2; ++i){
            //2表示a,b两个字符,eg 输入a
            si = state_change(i, que_front);//输入a后达到的状态
            for(auto it=si.begin(); it != si.end(); ++it){//闭包计算
                epsilon_closure(*it, si);
            }
            //handle this state to dfa.
            if(mp.find(si) == mp.end()){//不存在则添加
                mp[si] = ct;
                que.push(si);
                dfa[p].a[i] = ct;
                ct++;
            } else {
                dfa[p].a[i] = ( mp.find(si) )->second;
            }

        }
        que.pop();
        ++p;
        //cout << p << endl;
    }

    for(int i=0; i<p; ++i){
        for(int j=0; j<2; ++j){
            if(dfa[i].a[j] == -1) dfa[i].a[j] = p; //p是不可达状态
        }
    }
    
    dfa.push_back(init_dfa_state);
    for(int i=0; i<2; ++i){
        dfa[p].a[i] = p;
    }
}

void print_dfa(){
    cout<<endl;
    cout<<"NFA TO DFA CONVERSION"<<endl;
    cout<<"---------------------------------------------------------"<<endl;
    cout<<"STATE\t|\t"<<"a"<<"\t|\t"<<"b"<<"\t|\t"<<"IS_FINAL"<<"\t|"<<endl;
    cout<<"---------------------------------------------------------"<<endl;
    for(int i=0;i<dfa.size();i++){
        cout<<i<<"\t|\t"<<dfa[i].a[0]<<"\t|\t"<<dfa[i].a[1]<<"\t|\t"<<dfa[i].f<<"\t|"<<endl;
    }
    cout<<"---------------------------------------------------------"<<endl;
}

void print_menu(){
    cout<<"\n---------------------------------------\n";
    cout<<"Input Regex: "<<dispregex<<endl<<endl;
    cout<<"1. NFA\n";
    cout<<"2. Intermediate DFA\n";
    cout<<"3. Minimized DFA\n";
    cout<<"4. Simulation\n";
    cout<<"Press any other key to exit...\n\n";
}

bool simulate(int start_state){
    cout << endl << "Enter string: " << endl;
    string input;
    cin.ignore();
    getline(cin, input);
    int cur_state, next_state;
    cur_state = start_state;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Input\t|\tCurrent\t|\tNext\t|"<<endl;
    cout<<"-----------------------------------------"<<endl;
    for(int i=0; i<input.size(); ++i){
        if(input[i] == 'a'){
            next_state = dfa[cur_state].a[0];
        } else {
            next_state = dfa[cur_state].a[1];
        }
        cout<<input[i]<<"\t|\t"<<cur_state<<"\t|\t"<<next_state<<"\t|\n";
        cur_state=next_state;
    }
    cout<<"-----------------------------------------"<<endl;
    cout<<endl<<"Verdict: ";
    if(cur_state>=0 && dfa[cur_state].f){
         cout << "Accepted" << endl; 
         return true;
    } else cout << "Rejected" << endl;
    return false;
}


int main(){
    string regexp,postfix;
        cout << "Enter Regular Expression" << endl;
        cin >> regexp;
        regexp = insert_concat(regexp);
        postfix = regexp_to_postfix(regexp);
        cout << "Postfix expression is " << endl << postfix << endl;
        postfix_to_nfa(postfix);
        int final_state = st.top(); st.pop();
        int start_state = st.top(); st.pop();
        nfa[final_state].f = 1;
        //display_nfa();
        
        set<int> si;
        queue<set<int>> que;
        nfa_to_dfa(si, que, start_state);
        cout << endl << endl;
        //print_dfa();
       // dfa.clear();
        //nfa.clear();
   getchar();
   print_menu();
   while(1){
       //print_menu();
       char choice;
       choice = getchar();
       //custom_clear();
       switch(choice){
           case '1':
               display_nfa();
               getchar();
               break;
            case '2':
               print_dfa();
               getchar();
               break;
            case '3':
               cout << "I have not realized this function" << endl;
               getchar();// eat the \enter
               break;
            case '4':
               //while(1) if(simulate(start_state) ) break; 
               simulate(start_state);
               getchar();
               break;
            default:
               exit(EXIT_SUCCESS);
       }
   } 
   
}
