#include <array>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <algorithm>

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

    VehiclePlate(char l0, char l1, int digits, char l2, int region)
        : letters_{ l0, l1, l2 }
        , digits_(digits)
        , region_(region) {
    }

    string ToString() const {
        ostringstream out;
        out << letters_[0] << letters_[1];

        // чтобы дополнить цифровую часть номера слева нулями
        // до трёх цифр, используем подобные манипуляторы:
        // setfill задаёт символ для заполнения,
        // right задаёт выравнивание по правому краю,
        // setw задаёт минимальное желаемое количество знаков
        out << setfill('0') << right << setw(3) << digits_;
        out << letters_[2] << setw(2) << region_;

        return out.str();
    }

    int Hash() const {
        return digits_;
    }

private:
    array<char, 3> letters_;
    int digits_;
    int region_;
};

ostream& operator<<(ostream& out, VehiclePlate plate) {
    out << plate.ToString();
    return out;
}

class VehiclePlateHasher {
public:
    size_t operator()(const VehiclePlate& plate) const {
        // измените эту функцию, чтобы она учитывала все данные номера
        // рекомендуется использовать метод ToString() и существующий 
        // класс hash<string>
        return static_cast<size_t>(hasher_(plate.ToString()));
    }

private:
    hash<string> hasher_;
};

class ParkingCounter {
public:
    // зарегистрировать парковку автомобиля
    void Park(VehiclePlate car) {
        // место для вашей реализации
        ++car_to_parks_[car];
    }

    // метод возвращает количество зарегистрированных 
    // парковок автомобиля
    int GetCount(const VehiclePlate& car) const {
        // место для вашей реализации
        auto search = car_to_parks_.find(car);
        if (search == car_to_parks_.end()) return {};
        return car_to_parks_.at(car);
    }

    auto& GetAllData() const {
        return car_to_parks_;
    }

private:
    // для хранения данных используйте контейнер unordered_map
    // назовите поле класса car_to_parks_
    unordered_map<VehiclePlate, int, VehiclePlateHasher> car_to_parks_;
};

int main() {
    ParkingCounter parking;

    parking.Park({ 'B', 'H', 840, 'E', 99 });
    parking.Park({ 'O', 'K', 942, 'K', 78 });
    parking.Park({ 'O', 'K', 942, 'K', 78 });
    parking.Park({ 'O', 'K', 942, 'K', 78 });
    parking.Park({ 'O', 'K', 942, 'K', 78 });
    parking.Park({ 'H', 'E', 968, 'C', 79 });
    parking.Park({ 'T', 'A', 326, 'X', 83 });
    parking.Park({ 'H', 'H', 831, 'P', 116 });
    parking.Park({ 'A', 'P', 831, 'Y', 99 });
    parking.Park({ 'P', 'M', 884, 'K', 23 });
    parking.Park({ 'O', 'C', 34, 'P', 24 });
    parking.Park({ 'M', 'Y', 831, 'M', 43 });
    parking.Park({ 'B', 'P', 831, 'M', 79 });
    parking.Park({ 'O', 'K', 942, 'K', 78 });
    parking.Park({ 'K', 'T', 478, 'P', 49 });
    parking.Park({ 'X', 'P', 850, 'A', 50 });

    assert(parking.GetCount({ 'O', 'K', 942, 'K', 78 }) == 5);
    assert(parking.GetCount({ 'A', 'B', 111, 'C', 99 }) == 0);

    for (const auto& [plate, count] : parking.GetAllData()) {
        cout << plate << " "s << count << endl;
    }
}
