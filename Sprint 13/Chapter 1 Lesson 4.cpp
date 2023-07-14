#include <iostream>
#include <string_view>
#include <string>
#include <algorithm>

using namespace std;


// clang-format off

const int NUM_PLANETS = 8;
const string_view PLANETS[] = {
    "Mercury"sv, "Venus"sv, "Earth"sv,
    "Mars"sv, "Jupiter"sv, "Saturn"sv,
    "Uranus"sv, "Neptune"sv,
};

// clang-format on

bool IsPlanet(string_view name) {
    return count(begin(PLANETS), end(PLANETS), name);
}

void Test(string_view name) {
    cout << name << " is " << (IsPlanet(name) ? ""sv : "NOT "sv)
        << "a planet"sv << endl;
}

int main() {
    string name;
    getline(cin, name);
    Test(name);
}
