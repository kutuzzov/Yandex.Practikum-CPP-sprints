#include <iostream>

using namespace std;

template <typename F>
int FindFloor(int n, F drop) {
    int a = 1, b = n;
    while (a != b) {
        int m = (a + b) / 2;
        if (drop(m)) b = m;
        else a = m + 1;
    }
    return a;
}

int main() {
    int n, t;
    cout << "Enter n and target floor number: "s << endl;
    cin >> n >> t;

    int count = 0;
    int found = FindFloor(n, [t, &count](int f) {
        ++count;
        return f >= t;
        });

    cout << "Found floor "s << found << " after "s << count << " drops"s;

    return 0;
}
