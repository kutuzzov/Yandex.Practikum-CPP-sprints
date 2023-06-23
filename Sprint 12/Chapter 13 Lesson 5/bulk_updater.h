#pragma once

#include "entities.h"
#include "summing_segment_tree.h"

#include <cstdint>
#include <cmath>

struct BulkMoneyAdder {
    double delta = {};
};

struct BulkMoneySpender {
    double delta = {};
};

struct BulkTaxApplier {
    double multiplier = 1.0;
};

class BulkLinearUpdater {
public:
    BulkLinearUpdater() = default;

    BulkLinearUpdater(const BulkMoneyAdder& add)
        : add_(add) {
    }

    BulkLinearUpdater(const BulkMoneySpender& spend)
        : spend_(spend) {
    }
    
    BulkLinearUpdater(const BulkTaxApplier& tax)
        : tax_(tax) {
    }

    void CombineWith(const BulkLinearUpdater& other) {
        tax_.multiplier *= other.tax_.multiplier;
        add_.delta = add_.delta * other.tax_.multiplier + other.add_.delta;
        spend_.delta = spend_.delta + other.spend_.delta;
    }

    DayBudget Collapse(DayBudget origin, IndexSegment segment) const {
        DayBudget result;
        result.income = origin.income * tax_.multiplier + add_.delta * static_cast<double>(segment.length());
        result.spend = origin.spend + spend_.delta * static_cast<double>(segment.length());
        return result;
    }

private:
    // apply tax first, then add
    BulkTaxApplier tax_;
    BulkMoneyAdder add_;
    BulkMoneySpender spend_;
};
