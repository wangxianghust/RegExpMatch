//This is a copy of another code

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <set>
#include <map>

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
