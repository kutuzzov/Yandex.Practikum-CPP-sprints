#include "trip_manager.h"

#include <cassert>
#include <iostream>
#include <stdexcept>

using namespace std;

// Эти определения статических переменных правильнее было бы поместить в соответствующий cpp-файл,
// но мы для простоты разместим их здесь

int FlightProvider::capacity = 0;
int FlightProvider::counter = 0;

int HotelProvider::capacity = 0;
int HotelProvider::counter = 0;

void TestNoOverbooking() {
    FlightProvider::capacity = 100;
    HotelProvider::capacity = 100;
    FlightProvider::counter = 0;
    HotelProvider::counter = 0;
    {
        TripManager tm;
        auto trip = tm.Book({});
    }
    assert(FlightProvider::counter == 0);
    assert(HotelProvider::counter == 0);
}

void TestFlightOverbooking() {
    FlightProvider::capacity = 1;
    HotelProvider::capacity = 100;
    FlightProvider::counter = 0;
    HotelProvider::counter = 0;
    try {
        TripManager tm;
        auto trip = tm.Book({});
    } catch (const runtime_error&) {
        assert(FlightProvider::counter == 0);
        assert(HotelProvider::counter == 0);
        return;
    }
    cout << "Flight overbooking was expected"s << endl;
}

void TestHotelOverbooking() {
    FlightProvider::capacity = 100;
    HotelProvider::capacity = 0;
    FlightProvider::counter = 0;
    HotelProvider::counter = 0;
    try {
        TripManager tm;
        auto trip = tm.Book({});
    } catch (const runtime_error& ex) {
        assert(FlightProvider::counter == 0);
        assert(HotelProvider::counter == 0);
        return;
    }
    cout << "Hotel overbooking was expected"s << endl;
}

int main() {
    TestNoOverbooking();
    TestFlightOverbooking();
    TestHotelOverbooking();
}
