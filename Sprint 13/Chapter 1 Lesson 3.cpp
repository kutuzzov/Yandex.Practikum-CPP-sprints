#include <cstdint>
#include <iostream>
#include <climits>
#include <optional>
#include <limits>

using namespace std;

template <typename T>
optional<T> SafeAdd(T a, T b) {
    if ((a > 0 && b > 0 && a > std::numeric_limits<int64_t>::max() - b) ||
        (a < 0 && b < 0 && (a < std::numeric_limits<int64_t>::min() - b)))
        return nullopt;
    return a + b;
}

int main() {
    int64_t a;
    int64_t b;
    cin >> a >> b;
    auto result = SafeAdd(a, b);
    if (result) cout << result.value() << endl;
    else cout << "Overflow!" << endl;
}
