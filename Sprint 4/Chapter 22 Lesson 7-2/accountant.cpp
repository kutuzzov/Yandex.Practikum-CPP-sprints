#include "accountant.h"
#include "square_calculation.h"

double Accountant::CalcPaintNeeded(const Wall& wall) const {
    double height = wall.GetHeight();
    double width = wall.GetWidth();
    return CalcSquare(width, height) * 0.4;
}

double Accountant::CalcBricksNeeded(const Wall& wall) const {
    double height = wall.GetHeight();
    double width = wall.GetWidth();
    return CalcSquare(width, height) * 5;
}