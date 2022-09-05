#include <iostream>
#include <string>

using namespace std;

template <typename T>
void RunTestImpl(T func, const string& func_str) {
    /* Напишите недостающий код */
    func();
    cerr << func_str << " OK"s << endl;
}

#define RUN_TEST(func) RunTestImpl((func), #func) // напишите недостающий код

void Test1() {
}

int main() {
    RUN_TEST(Test1);
}
