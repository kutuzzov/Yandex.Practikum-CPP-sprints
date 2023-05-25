#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

using namespace std;

// реализуйте эту функцию:
size_t GetFileSize(string file) {
    fstream fin(file, ios::in);
    if (fin) fin.seekg(0, ios::end);
    else return string::npos;
    return fin.tellg();
}

int main() {
    ofstream("test.txt") << "123456789"sv;
    assert(GetFileSize("test.txt"s) == 9);

    ofstream test2("test2.txt"s);
    test2.seekp(1000);
    test2 << "abc"sv;
    test2.flush();

    assert(GetFileSize("test2.txt"s) == 1003);
    assert(GetFileSize("a file not exists"s) == string::npos);
}
