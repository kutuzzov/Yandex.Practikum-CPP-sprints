#include <cassert>
#include <string>
#include <utility>

// Шаблонная функция Swap обменивает значения двух объектов одного и того же типа,
// указатели на которые она принимает в качестве аргументов
template <typename T>
void Swap(T* value1, T* value2) {
    // Реализуйте тело функции самостоятельно
    std::swap(*value1, *value2);
}

int main() {
    using namespace std;

    {
        int x = 1;
        int y = 5;

        Swap(&x, &y);
        assert(x == 5);
        assert(y == 1);
    }

    {
        string one = "one"s;
        string two = "two"s;
        Swap(&one, &two);
        assert(one == "two"s);
        assert(two == "one"s);
    }

    {
        // Видимость структуры Point ограничена текущим блоком
        struct Point {
            int x, y;
        };

        Point p1{ 1, 2 };
        Point p2{ 3, 4 };

        Swap(&p1, &p2);
        assert(p1.x == 3 && p1.y == 4);
        assert(p2.x == 1 && p2.y == 2);
    }
    return 0;
}
