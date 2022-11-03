#include "log_duration.h"

#include <iostream>

using namespace std;

class StreamUntier {
public:
    // добавьте конструктор, деструктор
    // и дополнительные поля класса при необходимости
    StreamUntier(istream& stream) 
        : stream_(stream)
    {
        tied_before_ = stream_.tie(nullptr);
    }

    ~StreamUntier() {
        stream_.tie(tied_before_);
    }

private:
    ostream* tied_before_;
    istream& stream_;
};

int main() {
    LOG_DURATION("\\n with tie"s);

    StreamUntier guard(cin);
    int i;
    while (cin >> i) {
        cout << i * i << "\n"s;
    }

    return 0;
}
