#include "roof.h"

Roof::Roof(double length, double width)
    : length_(length)
    , width_(width) {
}

std::pair<double, double> Roof::GetSizes() const {
    return {length_, width_};
}