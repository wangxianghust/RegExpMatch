#include <iostream>

using namespace std;

int main(){
    int line[1024] = {1,3};
    std::cout << line << std::endl;
    for(auto i : line) cout << i << "--";
    cout << endl;
}
