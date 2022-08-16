#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

struct Animal {
    string name;
    int age;
    double weight;
};

template <typename Container, typename KeyMapper>
void SortBy(Container& container, KeyMapper key_mapper) {
    // теперь можно сортировать контейнер
    // с компаратором key_mapper(lhs) < key_mapper(rhs)
    sort(container.begin(), container.end(),
        [key_mapper](const auto& lhs, const auto& rhs) {
            return key_mapper(lhs) < key_mapper(rhs);
        });
}

void PrintNames(const vector<Animal>& animals) {
    for (const Animal& animal : animals) {
        cout << animal.name << ' ';
    }
    cout << endl;
}

int main() {
    vector<Animal> animals = {
        {"Мурка"s,   10, 5},
        {"Белка"s,   5,  1.5},
        {"Георгий"s, 2,  4.5},
        {"Рюрик"s,   12, 3.1},
    };

    PrintNames(animals);

    SortBy(animals, [](const Animal& animal) { return animal.name; });
    PrintNames(animals);

    SortBy(animals, [](const Animal& animal) { return -animal.weight; });
    PrintNames(animals);
}
