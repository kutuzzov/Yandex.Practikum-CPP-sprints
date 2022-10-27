#include "carpenter.h"
#include "square_calculation.h"

// Определите методы класса здесь
int Carpenter::CalcShelves(Wall& wall) const {
    return CalcSquare(wall.GetHeight(), wall.GetWidth()) / 2;
}
