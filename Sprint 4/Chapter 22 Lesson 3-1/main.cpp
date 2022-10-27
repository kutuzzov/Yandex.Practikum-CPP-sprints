#include "builder.h"
#include <iostream>

using namespace std;

int main() {
    Builder tom;
    // Попросите Тома посчитать кирпичи для стены 3.5 х 2.45
    Wall wall(3.5, 2.45);
    cout << tom.CalcBricksNeeded(wall) << endl;
}
