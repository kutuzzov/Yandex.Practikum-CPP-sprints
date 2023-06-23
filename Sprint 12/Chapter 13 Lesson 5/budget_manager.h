#pragma once
#include "bulk_updater.h"
#include "date.h"
#include "entities.h"

#include <vector>

class BudgetManager {
public:
    inline static const Date START_DATE{2000, 1, 1};
    inline static const Date END_DATE{2100, 1, 1};

    static size_t GetDayIndex(Date day) {
        return static_cast<size_t>(Date::ComputeDistance(START_DATE, day));
    }

    static IndexSegment MakeDateSegment(Date from, Date to) {
        return {GetDayIndex(from), GetDayIndex(to) + 1};
    }

    DayBudget ComputeSum(Date from, Date to) const {
        return tree_.ComputeSum(MakeDateSegment(from, to));
    }

    void AddBulkOperation(Date from, Date to, const BulkLinearUpdater& operation) {
        tree_.AddBulkOperation(MakeDateSegment(from, to), operation);
    }

private:
    SummingSegmentTree<DayBudget, BulkLinearUpdater> tree_{GetDayIndex(END_DATE)};
};
