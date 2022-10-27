#pragma once
#include "wall.h"

class Accountant {
public:
    double CalcPaintNeeded(const Wall& wall) const;
    double CalcBricksNeeded(const Wall& wall) const;
};
