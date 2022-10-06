#include <iostream>

using namespace std;

bool IsPowOfTwo(int number) {
    if (number == 0) return false;
    if (number == 1) return true;
    if (number % 2 == 0) {
        IsPowOfTwo(number/2);
        return true;
    }
    return false;
}

int main() {
    int result = IsPowOfTwo(1024);
    cout << result << endl;
}
