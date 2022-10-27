#include "builder.h"
#include "painter.h"
#include "carpenter.h"
#include <iostream>

using namespace std;

int main() {
    Builder tom;
    Painter bill;
    Carpenter jack;
    Wall wall(3.5, 2.45);

    cout << tom.CalcBricksNeeded(wall) << endl;
    cout << bill.CalcPaintNeeded(wall) << endl;
    cout << jack.CalcShelves(wall) << endl;
}