#include "parser.h"

using namespace std;

namespace queries {
class ComputeIncome : public ComputeQuery {
public:
    using ComputeQuery::ComputeQuery;
    ReadResult Process(const BudgetManager& budget) const override {
        return {budget.ComputeSum(GetFrom(), GetTo())};
    }

    class Factory : public QueryFactory {
    public:
        std::unique_ptr<Query> Construct(std::string_view config) const override {
            auto parts = Split(config, ' ');
            return std::make_unique<ComputeIncome>(Date::FromString(parts[0]), Date::FromString(parts[1]));
        }
    };
};

class Alter : public ModifyQuery {
public:
    Alter(Date from, Date to, double amount)
        : ModifyQuery(from, to)
        , amount_(amount) {
    }

    void Process(BudgetManager& budget) const override {
        double day_income = amount_ / (Date::ComputeDistance(GetFrom(), GetTo()) + 1);

        budget.AddBulkOperation(GetFrom(), GetTo(), BulkMoneyAdder{day_income});
    }

    class Factory : public QueryFactory {
    public:
        std::unique_ptr<Query> Construct(std::string_view config) const override {
            auto parts = Split(config, ' ');
            double payload = std::stod(std::string(parts[2]));
            return std::make_unique<Alter>(Date::FromString(parts[0]), Date::FromString(parts[1]), payload);
        }
    };

private:
    double amount_;
};

class PayTax : public ModifyQuery {
public:
    PayTax(Date from, Date to, double tax)
        : ModifyQuery(from, to)
        , tax_(tax) {
    }

    void Process(BudgetManager& budget) const override {
        budget.AddBulkOperation(GetFrom(), GetTo(), BulkTaxApplier{1 - (double)tax_ / 100});
    }

    class Factory : public QueryFactory {
    public:
        std::unique_ptr<Query> Construct(std::string_view config) const override {
            auto parts = Split(config, ' ');
            double tax = std::stod(std::string(parts[2]));
            return std::make_unique<PayTax>(Date::FromString(parts[0]), Date::FromString(parts[1]), tax);
        }
    };

private:
    double tax_;
};

class Spend : public ModifyQuery {
public:
    Spend(Date from, Date to, double spend)
        : ModifyQuery(from, to)
        , spend_(spend) {
    }

    void Process(BudgetManager& budget) const override {
        double day_spend = spend_ / (Date::ComputeDistance(GetFrom(), GetTo()) + 1);
        budget.AddBulkOperation(GetFrom(), GetTo(), BulkMoneySpender{ day_spend });
    }

    class Factory : public QueryFactory {
    public:
        std::unique_ptr<Query> Construct(std::string_view config) const override {
            auto parts = Split(config, ' ');
            double spend = std::stod(std::string(parts[2]));
            return std::make_unique<Spend>(Date::FromString(parts[0]), Date::FromString(parts[1]), spend);
        }
    };

private:
    double spend_;
};

}  // namespace queries

const QueryFactory& QueryFactory::GetFactory(std::string_view id) {
    static queries::ComputeIncome::Factory compute_income;
    static queries::Alter::Factory earn;
    static queries::PayTax::Factory pay_tax;
    static queries::Spend::Factory spend;
    static std::unordered_map<std::string_view, const QueryFactory&> factories
    = {{"ComputeIncome"sv, compute_income}, {"Earn"sv, earn}, {"PayTax"sv, pay_tax}, {"Spend"sv, spend}};

    return factories.at(id);
}
