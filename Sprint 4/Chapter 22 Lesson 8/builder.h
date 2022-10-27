#pragma once
#include "square_calculation.h"

class Builder {
public:
    void HoldDoor() const {
        // Просто держит дверь 100 условных секунд
        int i = 100;
        while (0 != i) {
            i--;
        }
    }
};