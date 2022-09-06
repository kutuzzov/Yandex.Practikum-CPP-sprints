using namespace std;

class Rational {
public:
    Rational(int numerator = 0, int denominator = 1) 
    : numerator_(numerator), denominator_(denominator) 
    {}
    
    int Numerator() const {
        return numerator_;
    }

    int Denominator() const {
        return denominator_;
    }
    
    void SetNumerator(int numerator) {
        numerator_ = numerator;
    }

    void SetDenominator(int denominator) {
        if (denominator != 0) {
            denominator_ = denominator;
        }
    }

private:
    int numerator_;
    int denominator_;
};
