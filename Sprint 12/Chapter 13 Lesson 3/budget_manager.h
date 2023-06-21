#pragma once
#include "date.h"

inline static const double TAX = 0.87;

struct DayInfo {
    double earnings;
};

class BudgetManager {
public:
    inline static const Date START_DATE{2000, 1, 1};
    inline static const Date END_DATE{2100, 1, 1};

    // разработайте класс BudgetManager
    BudgetManager()
        : all_days_(START_DATE.ComputeDistance(START_DATE, END_DATE), DayInfo())
    {}

    void ComputeIncome(const Date from, const Date to) const;
    void Earn(const Date from, const Date to, double earnings);
    void PayTax(const Date from, const Date to, double tax);

private:
    std::vector<DayInfo> all_days_;
};