#include <iostream>
#include <string_view>

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
    for (int i = 0; i < NUM_PLANETS; ++i) {
        if (PLANETS[i] == name) {
            return true;
        }
    }
    return false;
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
