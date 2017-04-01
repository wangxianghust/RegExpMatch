/*构造解析树，假设字母表是Alphabet的26个字母
 */

#include <iostream>
#include <string>
#include <cctype>

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
        Node* Parse(string, int);
        void InOrderPrint(Node* root);
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

Node* ParseTree::Parse(string pattern, int index){
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
            Node* vr = Parse(pattern, index+1);
            v = CreateNode('|', v, vr);
            cout << now << endl;
        } else if('*' == now){
            v = CreateNode('*', v, NULL);
            ++index;
        } else if('(' == now){
            cout << now << endl;
            Node* vr = Parse(pattern, index+1);
            ++index;
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

int main(){
    ParseTree parser;
    string pattern("(AT|GA)((AG|AAA)*)");
    parser.Preprocess(pattern);
    cout << pattern << endl;
    Node* root = parser.Parse(pattern, 0);
    parser.InOrderPrint(root);
}
