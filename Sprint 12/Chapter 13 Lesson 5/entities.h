#pragma once

struct DayBudget {
    double income = 0.0;
    double spend = 0.0;
};

template <typename Data>
Data operator+(const Data& lhs, const Data& rhs) {
    Data result;
    result.income = lhs.income + rhs.income;
    result.spend = lhs.spend + rhs.spend;
    return result;
}
