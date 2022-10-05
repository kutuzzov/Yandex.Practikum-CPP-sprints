#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <set>

using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename Container>
void EraseAndPrint(Container& container, int pos_to_erase, int begin_to_erase, int end_to_erase) {
    auto it_to_erase = container.erase(container.begin() + pos_to_erase);
    PrintRange(container.begin(), container.end());
    it_to_erase = container.erase(container.begin() + begin_to_erase, container.begin() + end_to_erase);
    PrintRange(container.begin(), container.end());
}

int main() {
    vector<string> langs = { "Python"s, "Java"s, "C#"s, "Ruby"s, "C++"s };
    EraseAndPrint(langs, 0, 0, 2);
}
