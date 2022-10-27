#include "carpenter.h"

#include "square_calculation.h"

int Carpenter::CalcShelves(const Wall& wall) const {
    double height = wall.GetHeight();
    double width = wall.GetWidth();
    return CalcSquare(width, height) / 2;
}

void Carpenter::InstallDoor(Wall& wall, const Builder& builder) const {
    // Hold the door, builder! Hold the door!
    builder.HoldDoor();
    wall.SetDoorInstalled();
}