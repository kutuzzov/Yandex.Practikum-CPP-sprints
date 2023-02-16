#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>

using namespace std;

class MoneyBox {
public:
    explicit MoneyBox(vector<int64_t> nominals)
        : nominals_(move(nominals))
        , counts_(nominals_.size()) {
    }

    const vector<int>& GetCounts() const {
        return counts_;
    }

    void PushCoin(int64_t value) {
        size_t pos = static_cast<size_t>(find(nominals_.begin(), nominals_.end(), value) - nominals_.begin());
        ++counts_[pos];
    }

    void PrintCoins(ostream& out) const {
        for (size_t i = 0; i < nominals_.size(); ++i) {
            if (counts_[i] > 0) out << nominals_[i] << ": " << counts_[i] << endl;
        }
    }

private:
    const vector<int64_t> nominals_;
    vector<int> counts_;
};

ostream& operator<<(ostream& out, const MoneyBox& cash) {
    cash.PrintCoins(out);
    return out;
}

int main() {
    MoneyBox cash({ 1, 500, 10000 });
    cash.PushCoin(500);
    cash.PushCoin(500);
    cash.PushCoin(10000);
    assert((cash.GetCounts() == vector<int>{0, 2, 1}));
    cout << cash << endl;
}
