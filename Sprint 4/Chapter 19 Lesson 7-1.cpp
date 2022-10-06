#include <iostream>

using namespace std;

int Fibonacci(int number) {
    if (number <= 2) return 1;
    else return Fibonacci(number - 1) + Fibonacci(number - 2);
}

int main() {
    cout << Fibonacci(6) << endl;
}
