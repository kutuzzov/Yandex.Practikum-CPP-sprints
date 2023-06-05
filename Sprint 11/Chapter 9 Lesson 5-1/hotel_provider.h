#pragma once
#include <stdexcept>
#include <string>

class HotelProvider {
public:
    using BookingId = int;

    struct BookingData {
        std::string city;
        std::string date_from;
        std::string date_to;
    };

    BookingId Book(const BookingData&) {
        using namespace std;
        if (counter >= capacity) {
            throw runtime_error("Hotel overbooking"s);
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
