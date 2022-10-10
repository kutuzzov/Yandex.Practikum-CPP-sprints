#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

void PrintSpacesPositions(string& str) {
    for (auto pos = find(str.begin(), str.end(), ' '); pos != str.end(); pos = find(next(pos), str.end(), ' ')) {
        cout << distance(str.begin(), pos) << endl;
    }
}

int main() {
    string str = "He said: one and one and one is three"s;
    PrintSpacesPositions(str);
    return 0;
}
