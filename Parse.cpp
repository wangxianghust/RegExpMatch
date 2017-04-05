/*构造解析树，假设字母表是Alphabet的26个字母
 */

#include <iostream>
#include <string>
#include <cctype>
#include <vector>

using namespace std;

struct Node{
    char data;
    Node* left; //if *, only have left child
    Node* right;
    Node(): data('\0'), left(NULL), right(NULL){}
    Node(char c): data(c), left(NULL), right(NULL) {}
    Node(char c, Node* left_, Node* right_): data(c), left(left_), right(right_){}
};

class ParseTree{
    public:
        Node* Parse(string, int &);
        void InOrderPrint(Node* root);
        void LevelPrint(Node* root);
        void Preprocess(string &s){
            s += '$';
        }
    private:
        Node* CreateNode(char data, Node* left, Node* right){
            Node* root = new Node(data);
            root->left = left;
            root->right = right;
            return root;
        }
        
};

Node* ParseTree::Parse(string pattern, int &index){  //NOTICE: use int & here, cause index is global var.
    Node* v = new Node();
    while(pattern[index] != '$'){
        char now = pattern[index];
        if(isalpha(now)){
            Node* vr = new Node(now);
            if(v->data != '\0'){
                v = CreateNode('.', v, vr);
            } else {
                v = vr;
            }
            //cout << now << endl;
            ++index;
        } else if('|' == now){
            Node* vr = Parse(pattern, ++index); //NOTICE: ++index NOT index+1
            v = CreateNode('|', v, vr);
        } else if('*' == now){
            v = CreateNode('*', v, NULL);
            ++index;
        } else if('(' == now){
            Node* vr = Parse(pattern, ++index);
            ++index;  //cause the Parse will return by ")", the index should go one more step to skip ")"
            if(v->data != '\0'){
                v = CreateNode('.', v, vr);
            } else {
                v = vr;
            }
        } else if(')' == now){
            return v;
        }
    }
    return v;
}

void ParseTree::InOrderPrint(Node* root){
    if(!root) return;
    InOrderPrint(root->left);
    cout << root->data << " ";
    InOrderPrint(root->right);
}

void ParseTree::LevelPrint(Node* root){
    if(! root) return;
    vector<Node*> vec;
    vec.push_back(root);
    int cur = 0;
    int last = 0;
    while(cur < vec.size()){ //BFS: iteration.
        last = vec.size();
        while(cur < last){  //The new layer
            Node* node = vec[cur];
            cout << node->data << "   ";
            if(node->left) vec.push_back(node->left);
            if(node->right) vec.push_back(node->right);
            ++cur;
        }
        cout << endl;
    }
}

int main(){
    ParseTree parser;
    string pattern("(AT|GA)((AG|AAA)*)");
    parser.Preprocess(pattern);
    cout << pattern << endl;
    int index = 0;
    Node* root = parser.Parse(pattern, index);
    parser.InOrderPrint(root);
    cout << endl;
    parser.LevelPrint(root);
}
