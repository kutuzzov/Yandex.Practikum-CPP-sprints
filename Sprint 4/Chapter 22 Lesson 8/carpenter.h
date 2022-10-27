#pragma once
#include "builder.h"
#include "wall.h"

class Carpenter {
public:
    int CalcShelves(const Wall& wall) const;
    void InstallDoor(Wall& wall, const Builder& builder) const;
};