#include "budget_manager.h"
#include <iostream>
// разработайте менеджер личного бюджета самостоятельно

void BudgetManager::ComputeIncome(const Date from, const Date to) const {
    double result = 0.0;
    const int start = from.ComputeDistance(START_DATE, from);
    const int end = to.ComputeDistance(START_DATE, to);
    for (int i = start; i <= end; ++i) {
        result += all_days_[i].earnings;
    }
    std::cout << result << std::endl;
}

void BudgetManager::Earn(const Date from, const Date to, double earnings) {
    const int start = from.ComputeDistance(START_DATE, from);
    const int end = to.ComputeDistance(START_DATE, to);
    int period = end - start + 1;
    for (int i = start; i <= end; ++i) {
        all_days_[i].earnings += earnings / period;
    }
}

void BudgetManager::PayTax(const Date from, const Date to, double tax) {
    const int start = from.ComputeDistance(START_DATE, from);
    const int end = to.ComputeDistance(START_DATE, to);
    for (int i = start; i <= end; ++i) {
        all_days_[i].earnings *= tax;
    }
}