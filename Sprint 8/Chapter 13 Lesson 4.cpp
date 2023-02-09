#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>
#include <future>
#include <execution>

using namespace std;

constexpr int MAX_ASYNC_LAUNCHES = 2;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end, int launches = 0) {
    int range_length = range_end - range_begin;
    if (range_length < 2) {
        return;
    }

    vector elements(range_begin, range_end);
    auto mid = elements.begin() + range_length / 2;

    auto left = [&elements, &mid, &launches] { return MergeSort(elements.begin(), mid, launches + 1); };
    auto right = [&mid, &elements, &launches] { return MergeSort(mid, elements.end(), launches + 1); };

    if (launches <= MAX_ASYNC_LAUNCHES) {
        auto left_future = async(left);
        auto right_future = async(right);
        left_future.get();
        right_future.get();
    }
    else {
        left();
        right();
    }

    merge(execution::par, elements.begin(), mid, mid, elements.end(), range_begin);
}

int main() {
    mt19937 generator;

    vector<int> test_vector(10);

    iota(test_vector.begin(), test_vector.end(), 1);
    shuffle(test_vector.begin(), test_vector.end(), generator);
    PrintRange(test_vector.begin(), test_vector.end());
    MergeSort(test_vector.begin(), test_vector.end());
    PrintRange(test_vector.begin(), test_vector.end());
    MergeSort(test_vector.data(), test_vector.data() + test_vector.size());

    return 0;
}
