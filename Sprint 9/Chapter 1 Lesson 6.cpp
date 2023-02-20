#include "log_duration.h"

#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <random>
#include <set>
#include <string>
#include <sstream>
#include <unordered_set>

using namespace std;

class VehiclePlate {
private:
    auto AsTuple() const {
        return tie(letters_, digits_, region_);
    }

public:
    bool operator==(const VehiclePlate& other) const {
        return AsTuple() == other.AsTuple();
    }

    bool operator<(const VehiclePlate& other) const {
        return AsTuple() < other.AsTuple();
    }

    VehiclePlate(char l0, char l1, int digits, char l2, int region)
        : letters_{l0, l1, l2}
        , digits_(digits)
        , region_(region) {
    }

    string ToString() const {
        ostringstream out;
        out << letters_[0] << letters_[1];
        out << setfill('0') << right << setw(3) << digits_;
        out << letters_[2] << setw(2) << region_;

        return out.str();
    }

    const array<char, 3>& GetLetters() const {
        return letters_;
    }

    int GetDigits() const {
        return digits_;
    }

    int GetRegion() const {
        return region_;
    }

private:
    array<char, 3> letters_;
    int digits_;
    int region_;
};

struct PlateHasherTrivial {
    size_t operator()(const VehiclePlate& plate) const {
        return static_cast<size_t>(plate.GetDigits());
    }
};

struct PlateHasherRegion {
    size_t operator()(const VehiclePlate& plate) const {
        return static_cast<size_t>(plate.GetDigits() + plate.GetRegion() * 1000);
    }
};

struct PlateHasherString {
    size_t operator()(const VehiclePlate& plate) const {
        return hasher(plate.ToString());
    }

    hash<string> hasher;
};

struct PlateHasherAll {
    int64_t operator()(const VehiclePlate& plate) const {
        auto letters = plate.GetLetters();
        return (plate.GetRegion() * 1000000 + ((letters[0] - 'A') + (letters[1] - 'A') + (letters[2] - 'A')) * 1000 + plate.GetDigits());
    }
};

ostream& operator<<(ostream& out, VehiclePlate plate) {
    out << plate.ToString();
    return out;
}

class PlateGenerator {
    char GenerateChar() {
        uniform_int_distribution<short> char_gen{0, static_cast<short>(possible_chars_.size() - 1)};
        return possible_chars_[char_gen(engine_)];
    }

    int GenerateNumber() {
        uniform_int_distribution<short> num_gen{0, 999};
        return num_gen(engine_);
    }

    int GenerateRegion() {
        uniform_int_distribution<short> region_gen{0, static_cast<short>(possible_regions_.size() - 1)};
        return possible_regions_[region_gen(engine_)];
    }

public:
    VehiclePlate Generate() {
        return VehiclePlate(GenerateChar(), GenerateChar(), GenerateNumber(), GenerateChar(), GenerateRegion());
    }

private:
    mt19937 engine_;

    // допустимые значения сохраним в static переменных
    // они объявлены inline, чтобы их определение не надо было выносить вне класса
    inline static const array possible_regions_
        = {1,  2,  102, 3,   4,   5,   6,   7,   8,  9,   10,  11,  12, 13,  113, 14,  15, 16,  116, 17, 18,
           19, 20, 21,  121, 22,  23,  93,  123, 24, 84,  88,  124, 25, 125, 26,  27,  28, 29,  30,  31, 32,
           33, 34, 35,  36,  136, 37,  38,  85,  39, 91,  40,  41,  82, 42,  142, 43,  44, 45,  46,  47, 48,
           49, 50, 90,  150, 190, 51,  52,  152, 53, 54,  154, 55,  56, 57,  58,  59,  81, 159, 60,  61, 161,
           62, 63, 163, 64,  164, 65,  66,  96,  67, 68,  69,  70,  71, 72,  73,  173, 74, 174, 75,  80, 76,
           77, 97, 99,  177, 199, 197, 777, 78,  98, 178, 79,  83,  86, 87,  89,  94,  95};

    // постфикс s у литерала тут недопустим, он приведёт к неопределённому поведению
    inline static const string_view possible_chars_ = "ABCEHKMNOPTXY"sv;
};

int main() {
    static const int N = 1'000'000;

    PlateGenerator generator;
    vector<VehiclePlate> fill_vector;
    vector<VehiclePlate> find_vector;

    generate_n(back_inserter(fill_vector), N, [&]() {
        return generator.Generate();
    });
    generate_n(back_inserter(find_vector), N, [&]() {
        return generator.Generate();
    });

    int found;
    {
        LOG_DURATION("unordered_set");
        unordered_set<VehiclePlate, PlateHasherAll> container;
        for (auto& p : fill_vector) {
            container.insert(p);
        }
        found = count_if(find_vector.begin(), find_vector.end(), [&](const VehiclePlate& plate) {
            return container.count(plate) > 0;
        });
    }
    cout << "Found matches (1): "s << found << endl;

    {
        LOG_DURATION("set");
        set<VehiclePlate> container;
        for (auto& p : fill_vector) {
            container.insert(p);
        }
        found = count_if(find_vector.begin(), find_vector.end(), [&](const VehiclePlate& plate) {
            return container.count(plate) > 0;
        });
    }
    cout << "Found matches (2): "s << found << endl;
}
