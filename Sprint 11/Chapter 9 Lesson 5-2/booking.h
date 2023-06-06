#pragma once

namespace raii {

template <typename Provider>
class Booking {
private:
    using BookingId = typename Provider::BookingId;

    Provider* provider_;
    int counter_;

public:
    Booking(Provider* p, int counter)
        : provider_(p), counter_(counter)
    {}

    Booking(const Booking&) = delete;
    Booking(Booking&& other) = default;
    Booking& operator=(const Booking&) = delete;
    Booking& operator=(Booking&& other) = default;
    ~Booking() {
        if (provider_ != nullptr) provider_->CancelOrComplete(*this);
    }

    // ��� ������� �� ��������� � ������, �� � �������� ��������� ��� ����� ���� �����
    BookingId GetId() const {
        return counter_;
    }
};

} // namespace raii