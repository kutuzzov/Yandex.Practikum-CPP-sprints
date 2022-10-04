#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

template<typename Iterator>
void PrintRange(Iterator container_begin, Iterator container_end) {
    for (auto it = container_begin; it != container_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

int main() {
    cout << "Test1"s << endl;
    set<int> test1 = { 1, 1, 1, 2, 3, 4, 5, 5 };
    PrintRange(test1.begin(), test1.end());
    cout << "Test2"s << endl;
    vector<int> test2 = {}; // пустой контейнер
    PrintRange(test2.begin(), test2.end());
    cout << "End of tests"s << endl;
}
