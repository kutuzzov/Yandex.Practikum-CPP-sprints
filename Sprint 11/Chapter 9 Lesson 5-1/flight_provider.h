#pragma once
#include <stdexcept>
#include <string>

class FlightProvider {
public:
    using BookingId = int;

    struct BookingData {
        std::string city_from;
        std::string city_to;
        std::string date;
    };

    BookingId Book(const BookingData&) {
        using namespace std;
        if (counter >= capacity) {
            throw runtime_error("Flight overbooking"s);
        }
        counter++;
        return counter;
    }
    void Cancel(const BookingId&) {
        using namespace std;
        counter--;
        if (counter < 0) {
            counter = 0;
        }
    }

public:
    static int capacity;
    static int counter;
};
