#include <cassert>
#include <memory>
#include <string>

// Шаблон ApplyToMany применяет функцию f (первый аргумент) последовательно к каждому из остальных своих аргументов

template <typename f, typename... Args>
void ApplyToMany(f& foo, Args&&... args) {
    (..., foo(std::forward<Args>(args)));
}

void TestSum() {
    int x;
    auto lambda = [&x](int y) {
        x += y;
    };

    x = 0;
    ApplyToMany(lambda, 1);
    assert(x == 1);

    x = 0;
    ApplyToMany(lambda, 1, 2, 3, 4, 5);
    assert(x == 15);
}

void TestConcatenate() {
    using namespace std::literals;
    std::string s;
    auto lambda = [&s](const auto& t) {
        if (!s.empty()) {
            s += " ";
        }
        s += t;
    };

    ApplyToMany(lambda, "cyan"s, "magenta"s, "yellow"s, "black"s);
    assert(s == "cyan magenta yellow black"s);
}

void TestIncrement() {
    auto increment = [](int& x) {
        ++x;
    };

    int a = 0;
    int b = 3;
    int c = 43;

    ApplyToMany(increment, a, b, c);
    assert(a == 1);
    assert(b == 4);
    assert(c == 44);
}

void TestArgumentForwarding() {
    struct S {
        int call_count = 0;
        int i = 0;
        std::unique_ptr<int> p;
        void operator()(int i) {
            this->i = i;
            ++call_count;
        }
        void operator()(std::unique_ptr<int>&& p) {
            this->p = std::move(p);
            ++call_count;
        }
    };

    S s;

    ApplyToMany(s, 1, std::make_unique<int>(42));
    assert(s.call_count == 2);
    assert(s.i == 1);
    assert(s.p != nullptr && *s.p == 42);
}

void TestArgumentForwardingToConstFunction() {
    struct S {
        mutable int call_count = 0;
        mutable int i = 0;
        mutable std::unique_ptr<int> p;
        void operator()(int i) const {
            this->i = i;
            ++call_count;
        }
        void operator()(std::unique_ptr<int>&& p) const {
            this->p = std::move(p);
            ++call_count;
        }
    };

    const S s;
    ApplyToMany(s, 1, std::make_unique<int>(42));
    assert(s.call_count == 2);
    assert(s.i == 1);
    assert(s.p != nullptr && *s.p == 42);
}

int main() {
    TestSum();
    TestConcatenate();
    TestIncrement();
    TestArgumentForwarding();
    TestArgumentForwardingToConstFunction();
    return 0;
}
