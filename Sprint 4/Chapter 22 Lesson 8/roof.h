#pragma once
#include <utility>

class Roof {
public:
    Roof(double length, double width);
    std::pair<double, double> GetSizes() const;

private:
    double length_;
    double width_;
};