#include <numeric>
#include <ostream>
#include <vector>
#include <iostream>

using namespace std;

class Rational {
public:
    Rational() = default;

    Rational(int numerator)
        : numerator_(numerator)
        , denominator_(1) {
    }

    Rational(int numerator, int denominator)
        : numerator_(numerator)
        , denominator_(denominator) {
        Normalize();
    }

    int Numerator() const {
        return numerator_;
    }

    int Denominator() const {
        return denominator_;
    }


private:
    void Normalize() {
        if (denominator_ < 0) {
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }
        const int divisor = gcd(numerator_, denominator_);
        numerator_ /= divisor;
        denominator_ /= divisor;
    }

    int numerator_ = 0;
    int denominator_ = 1;
};

ostream& operator<<(ostream& out, Rational value) {
    out << value.Numerator() << "/"s << value.Denominator();
    return out;
}

istream& operator>>(istream& in, Rational& value) {
    int numerator, denominator;
    char drob;
    in >> numerator >> drob >> denominator;
    value = Rational{ numerator, denominator };
    return in;
}

Rational operator+(Rational left, Rational right) {
    const int numerator = left.Numerator() * right.Denominator()
        + right.Numerator() * left.Denominator();
    const int denominator = left.Denominator() * right.Denominator();

    return { numerator, denominator };
}

Rational operator-(Rational left, Rational right) {
    const int numerator = left.Numerator() * right.Denominator()
        - right.Numerator() * left.Denominator();
    const int denominator = left.Denominator() * right.Denominator();

    return { numerator, denominator };
}

Rational operator-(Rational value) {
    return { -value.Numerator(), value.Denominator()};
}

Rational operator+(Rational value) {
    return value;
}

int main() {
    Rational r1{ 1, 6 };
    Rational r2{ 1, 3 };
    Rational r3 = -r1;

    cout << "r1 + r2 = "s << r1 + r2 << endl;
    cout << "r3 = "s << r3 << endl;
}
