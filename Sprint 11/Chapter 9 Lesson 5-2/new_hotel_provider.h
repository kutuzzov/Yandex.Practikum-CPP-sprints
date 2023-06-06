#pragma once
#include "booking.h"

#include <stdexcept>
#include <string>

class HotelProvider {
public:
    using BookingId = int;
    using Booking = raii::Booking<HotelProvider>;
    friend Booking;

    struct BookingData {
        std::string city;
        std::string date_from;
        std::string date_to;
    };

    Booking Book(const BookingData&) {
        using namespace std;
        if (counter >= capacity) {
            throw runtime_error("Hotel overbooking"s);
        }
        counter++;
        return { this, counter };
    }

private:
    void CancelOrComplete(const Booking&) {
        counter--;
        if (0 > counter) {
            counter = 0;
        }
    }

public:
    static int capacity;
    static int counter;
};