#include <iostream>
#include <string>
#include <vector>
#include <set>

using namespace std;

template <typename Type>
void Print(ostream& out, const Type& container) {
    bool is_first = true;
    for (const auto& element : container) {
        if (is_first) {
            out << element;
            is_first = false;
        }
        else {
            out << ", "s << element;
        }
    }
}

template <typename Type>
ostream& operator<<(ostream& out, const vector<Type>& container) {
    Print(out, container);
    return out;
}

template <typename Type>
ostream& operator<<(ostream& out, const set<Type>& container) {
    Print(out, container);
    return out;
}

int main() {
    const set<string> cats = { "Мурка"s, "Белка"s, "Георгий"s, "Рюрик"s };
    cout << cats << endl;

    const vector<int> ages = { 10, 5, 2, 12 };
    cout << ages << endl;
}
