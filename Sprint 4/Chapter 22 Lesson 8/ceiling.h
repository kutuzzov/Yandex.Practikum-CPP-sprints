#pragma once
#include <utility>

class Ceiling {
public:
    Ceiling(double length, double width);
    std::pair<double, double> GetSizes() const;

private:
    double length_;
    double width_;
};