#include "list.h"

#include <cassert>
#include <vector>

using namespace std;

// функция возвращает функциональный объект,
// выполняющий вставку в выбранный список
template <class Type>
auto MakeInsertingFunction(vector<SingleLinkedList<Type>>& lists, const int& x) { // исправляем на const int&
    return [&](const Type& value) {
        lists[x].PushFront(value);
    };
}

template <class T>
void InsertRange(int from, int to, T push_function) {
    for (int i = from; i < to; ++i) {
        push_function(i);
    }
}

int main() {
    // main тестирует вектор, в нём нет ошибок.
    // не меняйте код этой функции
    // помимо этих тестов, список должен проходить все обычные тесты списка.
    // Ищите ошибки в коде списка и
    // других функциях этого файла

    vector<SingleLinkedList<int>> lists_a(10);

    auto push_to_2a = MakeInsertingFunction(lists_a, 2);
    auto push_to_5a = MakeInsertingFunction(lists_a, 5);
    auto push_to_7a = MakeInsertingFunction(lists_a, 7);

    InsertRange(10, 12, push_to_2a);
    InsertRange(12, 14, push_to_5a);
    InsertRange(14, 16, push_to_7a);

    assert(lists_a[2] == SingleLinkedList<int>({ 11, 10 }));
    assert(lists_a[5] == SingleLinkedList<int>({ 13, 12 }));
    assert(lists_a[7] == SingleLinkedList<int>({ 15, 14 }));

    vector<SingleLinkedList<int>> lists_b = lists_a;

    auto push_to_2b = MakeInsertingFunction(lists_b, 2);
    auto push_to_5b = MakeInsertingFunction(lists_b, 5);
    auto push_to_7b = MakeInsertingFunction(lists_b, 7);

    InsertRange(20, 22, push_to_2b);
    InsertRange(22, 24, push_to_5b);
    InsertRange(24, 26, push_to_7b);

    assert(lists_b[2] == SingleLinkedList<int>({ 21, 20, 11, 10 }));
    assert(lists_b[5] == SingleLinkedList<int>({ 23, 22, 13, 12 }));
    assert(lists_b[7] == SingleLinkedList<int>({ 25, 24, 15, 14 }));

    lists_a[2].PopFront();
    lists_a[5].InsertAfter(lists_a[5].begin(), 100);
    lists_b[5].EraseAfter(next(lists_b[5].begin()));
    lists_b[7].Clear();

    assert(lists_a[2] == SingleLinkedList<int>({ 10 }));
    assert(lists_a[5] == SingleLinkedList<int>({ 13, 100, 12 }));
    assert(lists_b[5] == SingleLinkedList<int>({ 23, 22, 12 }));
    assert(lists_b[7] == SingleLinkedList<int>());
}
