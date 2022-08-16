#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

using namespace std;

template <typename Type1, typename Type2>
ostream& operator<<(ostream& out, const pair<Type1, Type2>& container) {
    return out << "(" << container.first << ", " << container.second << ")";
}

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
    out << "["s;
    Print(out, container);
    out << "]"s;
    return out;
}

template <typename Type>
ostream& operator<<(ostream& out, const set<Type>& container) {
    out << "{"s;
    Print(out, container);
    out << "}"s;
    return out;
}

template <typename Type1, typename Type2>
ostream& operator<<(ostream& out, const map<Type1, Type2>& container) {
    out << "<"s;
    Print(out, container);
    out << ">"s;
    return out;
}

int main() {
    const set<string> cats = { "Мурка"s, "Белка"s, "Георгий"s, "Рюрик"s };
    cout << cats << endl;

    const vector<int> ages = { 10, 5, 2, 12 };
    cout << ages << endl;

    const map<string, int> cat_ages = {
        {"Мурка"s, 10},
        {"Белка"s, 5},
        {"Георгий"s, 2},
        {"Рюрик"s, 12}
    };
    cout << cat_ages << endl;
}
