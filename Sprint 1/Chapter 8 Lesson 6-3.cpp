#include <iostream>
#include <string>
#include <vector>
#include <set>

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

template <typename Type>
ostream& operator<<(ostream& out, const set<Type>& container) {
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
    const set<string> cats = { "Мурка"s, "Белка"s, "Георгий"s, "Рюрик"s };
    cout << cats << endl;
}
