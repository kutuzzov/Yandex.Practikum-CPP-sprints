#include <iostream>
#include <string>
#include <vector>

using namespace std;

template <typename Type>
ostream& operator<<(ostream& out, const vector<Type>& container) {
    bool is_first = true;
    for (const Type& element : container) {
        if (is_first) {
            out << element;
            is_first = false;
        }
        else {
            out << ", "s << element;
        }
    }
    return out;
}

int main() {
    const vector<int> ages = { 10, 5, 2, 12 };
    cout << ages << endl;
}
