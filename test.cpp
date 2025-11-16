#include  <iostream>

using namespace  std;

int main() {
    string name;
    cout << "name: \n"; getline(cin, name);
    cout << name.empty() << endl;
}