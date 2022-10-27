#include "painter.h"

int main() {
    Painter bill;
    Wall wall(3.5, 2.45);

    bill.Paint(wall, Wall::Color::BLUE);
}
