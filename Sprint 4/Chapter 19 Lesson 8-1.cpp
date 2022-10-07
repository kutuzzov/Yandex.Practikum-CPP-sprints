#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>
#include <functional>

using namespace std;

// функция, записывающая элементы диапазона в строку
template <typename It>
string PrintRangeToString(It range_begin, It range_end) {
    // удобный тип ostringstream -> https://ru.cppreference.com/w/cpp/io/basic_ostringstream
    ostringstream out;
    for (auto it = range_begin; it != range_end; ++it) {
        out << *it << " "s;
    }
    out << endl;
    // получаем доступ к строке с помощью метода str для ostringstream
    return out.str();
}

template <typename It>
vector<string> GetPermutations(const It& range_begin, const It& range_end) {
    vector<string> result;
    sort(range_begin, range_end);
    reverse(range_begin, range_end);
    do {
        result.push_back(PrintRangeToString(range_begin, range_end));
    } while (prev_permutation(range_begin, range_end));

    return result;
}

int main() {
    vector<int> permutation(3);
    // iota             -> http://ru.cppreference.com/w/cpp/algorithm/iota
    // Заполняет диапазон последовательно возрастающими значениями
    iota(permutation.begin(), permutation.end(), 1);
    
    auto result = GetPermutations(permutation.begin(), permutation.end());
    for (const auto& s : result) {
        cout << s;
    }
    return 0;
}
