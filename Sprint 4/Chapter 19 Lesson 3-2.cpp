#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

template<typename Container, typename Element>
void FindAndPrint(Container container, Element element) {
    auto element_pos = find(container.begin(), container.end(), element);
    for (auto it = container.begin(); it != element_pos; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
    for (auto it = element_pos; it != container.end(); ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

int main() {
    set<int> test = { 1, 1, 1, 2, 3, 4, 5, 5 };
    cout << "Test1"s << endl;
    FindAndPrint(test, 3);
    cout << "Test2"s << endl;
    FindAndPrint(test, 0); // элемента 0 нет в контейнере
    cout << "End of tests"s << endl;
}
