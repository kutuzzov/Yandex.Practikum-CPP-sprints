#pragma once
#include "square_calculation.h"
#include "wall.h"

class Painter {
public:
    void Paint(Wall& wall, Wall::Color color) const {
        wall.SetColor(color);
    }
    double CalcPaintNeeded(const Wall& wall) const {
        double height = wall.GetHeight();
        double width = wall.GetWidth();
        return CalcSquare(width, height) * 0.4;
    }
};