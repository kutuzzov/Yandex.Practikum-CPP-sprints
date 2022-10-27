#include "accountant.h"
#include "ceiling.h"
#include "roof.h"

#include <iostream>

using namespace std;

int main() {
    Accountant ray;
    Wall wall(3.5, 2.45);
    Roof roof(5, 7);
    Ceiling ceiling(5, 7);

    cout << "Требуется кирпичей: "s
         << ray.CalcBricksNeeded<Wall>(wall) + ray.CalcBricksNeeded<Roof>(roof) + ray.CalcBricksNeeded<Ceiling>(ceiling)
         << endl;

    cout << "Требуется краски: "s
         << ray.CalcPaintNeeded<Wall>(wall) + ray.CalcPaintNeeded<Roof>(roof) + ray.CalcPaintNeeded<Ceiling>(ceiling)
         << endl;
}