#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

// функция выводит элементы контейнера через запятую
template <typename It>
void PrintRangeComma(It range_begin, It range_end) {
    if (range_begin == range_end) {
        return;
    }
    for (auto it = range_begin;;) {
        cout << *it;
        ++it;
        if (it == range_end) {
            break;
        }
        cout << ", "s;
    }
    cout << endl;
}

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
    // 1. Если диапазон содержит меньше 2 элементов, выходим из функции
    int range_length = static_cast<int>(range_end - range_begin);
    if (range_length < 2) {
        return;
    }

    // 2. Создаём вектор, содержащий все элементы текущего диапазона
    vector<typename RandomIt::value_type> elements(range_begin, range_end);

    // 3. Разбиваем вектор на две равные части
    auto mid = elements.begin() + range_length / 2;

    // 4. Вызываем функцию MergeSort от каждой половины вектора
    MergeSort(elements.begin(), mid);
    MergeSort(mid, elements.end());

    // 5. С помощью алгоритма merge сливаем отсортированные половины
    // в исходный диапазон
    // merge -> http://ru.cppreference.com/w/cpp/algorithm/merge
    merge(elements.begin(), mid, mid, elements.end(), range_begin);
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
    PrintRangeComma(test_vector.begin(), test_vector.end());

    // Сортируем вектор с помощью сортировки слиянием
    MergeSort(test_vector.begin(), test_vector.end());

    // Выводим результат
    PrintRangeComma(test_vector.begin(), test_vector.end());

    return 0;
}
