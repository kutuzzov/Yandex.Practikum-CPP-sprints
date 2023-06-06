#pragma once
#include "new_flight_provider.h"
#include "new_hotel_provider.h"

#include <vector>

class Trip {
public:
    std::vector<HotelProvider::Booking> hotels;
    std::vector<FlightProvider::Booking> flights;

    Trip() = default;
    Trip(const Trip&) = delete;
    Trip(Trip&&) = default;
    ~Trip() = default;

    Trip& operator=(const Trip&) = delete;
    Trip& operator=(Trip&&) = default;

    void Cancel() {
        hotels.clear();
        flights.clear();
    }
};

class TripManager {
public:
    struct BookingData {
        std::string city_from;
        std::string city_to;
        std::string date_from;
        std::string date_to;
    };

    Trip Book(const BookingData&) {
        Trip trip;
        {
            FlightProvider::BookingData flight_booking_data;
            trip.flights.push_back(flight_provider_.Book(flight_booking_data));
        }
        {
            HotelProvider::BookingData hotel_booking_data;
            trip.hotels.push_back(hotel_provider_.Book(hotel_booking_data));
        }
        {
            FlightProvider::BookingData flight_booking_data;
            trip.flights.push_back(flight_provider_.Book(flight_booking_data));
        }
        return trip;
    }

    void Cancel(Trip& trip) {
        trip.Cancel();
    }

private:
    HotelProvider hotel_provider_;
    FlightProvider flight_provider_;
};