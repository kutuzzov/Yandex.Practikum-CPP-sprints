#include <iostream>
#include <string>

using namespace std;

template <typename F>
string BruteForce(F check) {
    // верните строку str, для которой check(str) будет true
    string str = "XXXXX"s;
    for (str[0] = 'A'; str[0] <= 'Z'; ++str[0]) {
        for (str[1] = 'A'; str[1] <= 'Z'; ++str[1]) {
            for (str[2] = 'A'; str[2] <= 'Z'; ++str[2]) {
                for (str[3] = 'A'; str[3] <= 'Z'; ++str[3]) {
                    for (str[4] = 'A'; str[4] <= 'Z'; ++str[4]) {
                        if (check(str)) return str;
                    }
                }
            }
        }
    }
    return {};
}

int main() {
    string pass = "ARTUR"s;
    auto check = [pass](const string& s) {
        return s == pass;
    };
    cout << BruteForce(check) << endl;
}
