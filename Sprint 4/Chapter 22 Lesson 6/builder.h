#pragma once
#include "square_calculation.h"
#include "wall.h"

class Builder {
public:
    double  CalcBricksNeeded(const Wall& wall) const {
        double height = wall.GetHeight();
        double width = wall.GetWidth();
        return CalcSquare(width, height) * 5;
    }
};