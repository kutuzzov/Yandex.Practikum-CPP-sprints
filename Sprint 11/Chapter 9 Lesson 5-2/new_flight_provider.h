#pragma once
// «десь будет подключЄн ваш файл с определением шаблонного класса Booking в пространстве имЄн RAII
#include "booking.h"
#include <iostream>


#include <stdexcept>
#include <string>

class FlightProvider {
public:
    using BookingId = int;
    using Booking = raii::Booking<FlightProvider>;
    friend Booking;  // явно разрешаем функци€м класса Booking вызывать private-функции нашего класса FlightProvider

    struct BookingData {
        std::string city_from;
        std::string city_to;
        std::string date;
    };

    Booking Book(const BookingData&) {
        using namespace std;
        if (counter >= capacity) {
            throw runtime_error("Flight overbooking"s);
        }
        counter++;
        return { this, counter };
    }

private:
    // —крываем эту функцию в private, чтобы еЄ мог позвать только соответствующий friend-класс Booking
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