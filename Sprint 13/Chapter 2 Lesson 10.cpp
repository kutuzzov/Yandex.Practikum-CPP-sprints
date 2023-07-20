#include <cassert>
#include <string>
#include <string_view>

using namespace std::literals;

template <typename T0, typename... Ts>
bool EqualsToOneOf(const T0& v0, const Ts&... vs) {
    return ((v0 == vs) || ...);
}

int main() {
    assert(EqualsToOneOf("hello"sv, "hi"s, "hello"s));
    assert(!EqualsToOneOf(1, 10, 2, 3, 6));
    assert(!EqualsToOneOf(8));
}
