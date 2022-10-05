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
void EraseAndPrint(Container& container, int pos) {
    auto it_to_erase = container.erase(container.begin()+pos);
    PrintRange(container.begin(), it_to_erase);
    PrintRange(it_to_erase, container.end());
}

int main() {
    vector<string> langs = { "Python"s, "Java"s, "C#"s, "Ruby"s, "C++"s };
    EraseAndPrint(langs, 2);
}
