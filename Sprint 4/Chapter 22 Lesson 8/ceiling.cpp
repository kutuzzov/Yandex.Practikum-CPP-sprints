#include "ceiling.h"

Ceiling::Ceiling(double length, double width)
    : length_(length)
    , width_(width) {
}

std::pair<double, double> Ceiling::GetSizes() const {
    return {length_, width_};
}