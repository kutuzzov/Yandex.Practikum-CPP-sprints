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

    Rational& operator+=(Rational right) {
        numerator_ = numerator_ * right.denominator_ + denominator_ * right.numerator_;
        denominator_ = denominator_ * right.denominator_;
        Normalize();
        return *this;
    }

    Rational& operator-=(Rational right) {
        numerator_ = numerator_ * right.denominator_ - denominator_ * right.numerator_;
        denominator_ = denominator_ * right.denominator_;
        Normalize();
        return *this;
    }

    Rational& operator*=(Rational right) {
        numerator_ *= right.numerator_;
        denominator_ *= right.denominator_;
        Normalize();
        return *this;
    }

    Rational& operator/=(Rational right) {
        numerator_ *= right.denominator_;
        denominator_ *= right.numerator_;
        Normalize();
        return *this;
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
    return left += right;
}

Rational operator-(Rational left, Rational right) {
    return left -= right;
}

Rational operator*(Rational left, Rational right) {
    return left *= right;
}

Rational operator/(Rational left, Rational right) {
    return left /= right;
}

Rational operator-(Rational value) {
    return { -value.Numerator(), value.Denominator()};
}

Rational operator+(Rational value) {
    return value;
}

bool operator==(Rational left, Rational right) {
    return left.Numerator() == right.Numerator() &&
        left.Denominator() == right.Denominator();
}

bool operator!=(Rational left, Rational right) {
    return !(left == right);
}

bool operator<(Rational left, Rational right) {
    return (left.Numerator() * right.Denominator() < right.Numerator() * left.Denominator());
}

bool operator>(Rational left, Rational right) {
    return (left.Numerator() * right.Denominator() > right.Numerator() * left.Denominator());
}

bool operator<=(Rational left, Rational right) {
    return !(left > right);
}

bool operator>=(Rational left, Rational right) {
    return !(left < right);
}

int main() {
    Rational r1{ 1, 6 };
    Rational r2{ 1, 3 };
    Rational r3 = -r1;
    Rational r4 = r1+=r2;
    Rational r5 = r2 /= r1;
    Rational r6{ 1, 6 };
    Rational r7{ 1, 6 };

    cout << "r1 + r2 = "s << r1 + r2 << endl;
    cout << "r3 = "s << r3 << endl;
    cout << "r4 = "s << r4 << endl;
    cout << "r5 = "s << r5 << endl;

    if (r2 > r1) cout << "true"s << endl;
    if (r1 <= r2) cout << "true"s << endl;
    if (r1 != r2) cout << "true"s << endl;
    if (r7 == r6) cout << "true"s << endl;
    if (r7 <= r6) cout << "true"s << endl;
}
