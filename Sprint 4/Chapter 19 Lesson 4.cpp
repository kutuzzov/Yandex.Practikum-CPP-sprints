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

template <typename It>
auto MakeVector(It range_begin, It range_end) {
    return vector(range_begin, range_end);
}

int main() {
    set<string> unique_langs = { "Python"s, "Java"s, "C#"s, "Ruby"s, "C++"s };
    vector<string> langs = MakeVector(unique_langs.begin(), unique_langs.end());
    PrintRange(langs.begin(), langs.end());
}
