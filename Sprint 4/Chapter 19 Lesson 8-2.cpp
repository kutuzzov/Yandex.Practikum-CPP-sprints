#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
    if ((range_end - range_begin) < 2) return; 
    
    vector<typename RandomIt::value_type> result(range_begin, range_end);
    auto middle = result.begin() + (range_end - range_begin) / 2;
    
    MergeSort(result.begin(), middle);
    MergeSort(middle, result.end());
    
    merge(result.begin(), middle, middle, result.end(), range_begin);
}

template <typename RandomIt>
void PrintRange(RandomIt range_begin, RandomIt range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

int main() {
    vector<int> test_vector(10);

    // iota             -> http://ru.cppreference.com/w/cpp/algorithm/iota
    // Заполняет диапазон последовательно возрастающими значениями
    iota(test_vector.begin(), test_vector.end(), 1);

    // random_shuffle   -> https://ru.cppreference.com/w/cpp/algorithm/random_shuffle
    // Перемешивает элементы в случайном порядке
    random_shuffle(test_vector.begin(), test_vector.end());

    // Выводим вектор до сортировки
    PrintRange(test_vector.begin(), test_vector.end());

    // Сортируем вектор с помощью сортировки слиянием
    MergeSort(test_vector.begin(), test_vector.end());

    // Выводим результат
    PrintRange(test_vector.begin(), test_vector.end());

    return 0;
}
