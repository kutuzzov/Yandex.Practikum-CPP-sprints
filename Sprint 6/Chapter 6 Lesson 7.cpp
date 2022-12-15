#include <cassert>
#include <vector>

template <typename T>
void ReverseArray(T* start, size_t size) {
    // Напишите тело функции самостоятельно
    if (size) {
        size_t reverse_count = size - 1, count = 0;

        while (reverse_count > count)
        {
            T tmp = *(start + count); // сохраняем начало массива во временную переменную
            *(start + count) = *(start + reverse_count); // присваиваем началу массива значение конца массива
            *(start + reverse_count) = tmp; // возвращаем концу массива сохраненное в tmp начало массива

            ++count; // увеличиваем счетчик с начала массива
            --reverse_count; // уменьшаем счетчик с конца массива
        }
    }
}

int main() {
    using namespace std;

    vector<int> v = { 1, 2 };
    ReverseArray(v.data(), v.size());
    assert(v == (vector<int>{2, 1}));
}
