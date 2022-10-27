#pragma once
#include "square_calculation.h"
#include "wall.h"

#include <utility>

using namespace std;

class Accountant {
public:
    template <class BuildingObject>
    double CalcPaintNeeded(const BuildingObject& building_object) const {
        pair<double, double> sizes = building_object.GetSizes();
        return CalcSquare(sizes.first, sizes.second) * 0.4;
    }

    template <class BuildingObject>
    double CalcBricksNeeded(const BuildingObject& building_object) const {
        pair<double, double> sizes = building_object.GetSizes();
        return CalcSquare(sizes.first, sizes.second) * 5;
    }
};