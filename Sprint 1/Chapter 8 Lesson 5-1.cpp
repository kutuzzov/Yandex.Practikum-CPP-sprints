#include <iostream>
#include <string>
#include <vector>

using namespace std;

template <typename Type>
ostream& operator<<(ostream& out, const vector<Type>& container) {
    for (const Type& element : container) {
        out << element << " "s;
    }
    return out;
}

int main() {
    const vector<int> ages = { 10, 5, 2, 12 };
    cout << ages << endl;
}
