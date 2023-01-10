#include "simple_vector.h"

#include <cassert>
#include <iostream>

using namespace std;

void TestReserveConstructor() {
    cout << "TestReserveConstructor"s << endl;
    SimpleVector<int> v(Reserve(5));
    assert(v.GetCapacity() == 5);
    assert(v.IsEmpty());
    cout << "Done!"s << endl;
}

void TestReserveMethod() {
    cout << "TestReserveMethod"s << endl;
    SimpleVector<int> v;
    // зарезервируем 5 мест в векторе
    v.Reserve(5);
    assert(v.GetCapacity() == 5);
    assert(v.IsEmpty());

    // попытаемся уменьшить capacity до 1
    v.Reserve(1);
    // capacity должно остаться прежним
    assert(v.GetCapacity() == 5);
    // поместим 10 элементов в вектор
    for (int i = 0; i < 10; ++i) {
        v.PushBack(i);
    }
    assert(v.GetSize() == 10);
    // увеличим capacity до 100
    v.Reserve(100);
    // проверим, что размер не поменялся
    assert(v.GetSize() == 10);
    assert(v.GetCapacity() == 100);
    // проверим, что элементы на месте
    for (int i = 0; i < 10; ++i) {
        assert(v[i] == i);
    }
    cout << "Done!"s << endl;
}

int main() {
    TestReserveConstructor();
    TestReserveMethod();
}
