#include "cell.h"
#include "sheet.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>
#include <stack>

class Cell::Impl {
public:
    virtual ~Impl() = default;
    virtual Value GetValue() const = 0;
    virtual std::string GetText() const = 0;
    virtual std::vector<Position> GetReferencedCells() const { return {}; }
    virtual bool IsCacheValid() const { return true; }
    virtual void InvalidateCache() {}
};

class Cell::EmptyImpl : public Impl {
public:
    Value GetValue() const override { return ""; }
    std::string GetText() const override { return ""; }
};

class Cell::TextImpl : public Impl {
public:
    TextImpl(std::string text) 
        : text_(std::move(text)) {
        if (text_.empty()) { throw std::logic_error(""); }
    }

    Value GetValue() const override {
        if (text_[0] == ESCAPE_SIGN) return text_.substr(1);
        return text_;
    }

    std::string GetText() const override {
        return text_;
    }

private:
    std::string text_;
};

class Cell::FormulaImpl : public Impl {
public:
    explicit FormulaImpl(std::string expression, const SheetInterface& sheet)
        : sheet_(sheet) {
        if (expression.empty() || expression[0] != FORMULA_SIGN) throw std::logic_error("");

        formula_ptr_ = ParseFormula(expression.substr(1));
    }

    Value GetValue() const override {
        if (!cache_) cache_ = formula_ptr_->Evaluate(sheet_);

        auto value = formula_ptr_->Evaluate(sheet_);
        if (std::holds_alternative<double>(value)) return std::get<double>(value);

        return std::get<FormulaError>(value);
    }

    std::string GetText() const override {
        return FORMULA_SIGN + formula_ptr_->GetExpression();
    }

    bool IsCacheValid() const override {
        return cache_.has_value();
    }

    void InvalidateCache() override {
        cache_.reset();
    }

    std::vector<Position> GetReferencedCells() const {
        return formula_ptr_->GetReferencedCells();
    }

private:
    std::unique_ptr<FormulaInterface> formula_ptr_;
    const SheetInterface& sheet_;
    mutable std::optional<FormulaInterface::Value> cache_;
};

bool Cell::WouldIntroduceCircularDependency(const Impl& new_impl) const {
    if (new_impl.GetReferencedCells().empty()) return false;

    std::unordered_set<const Cell*> referenced;
    for (const auto& pos : new_impl.GetReferencedCells()) {
        referenced.insert(sheet_.GetCellPtr(pos));
    }

    std::unordered_set<const Cell*> visited;
    std::stack<const Cell*> to_visit;
    to_visit.push(this);
    while (!to_visit.empty()) {
        const Cell* current = to_visit.top();
        to_visit.pop();
        visited.insert(current);

        if (referenced.find(current) != referenced.end()) return true;

        for (const Cell* incoming : current->l_nodes_) {
            if (visited.find(incoming) == visited.end()) to_visit.push(incoming);
        }
    }

    return false;
}

void Cell::InvalidateCacheRecursive(bool force) {
    if (impl_->IsCacheValid() || force) {
        impl_->InvalidateCache();
        for (Cell* incoming : l_nodes_) {
            incoming->InvalidateCacheRecursive();
        }
    }
}

Cell::Cell(Sheet& sheet)
    : impl_(std::make_unique<EmptyImpl>())
    , sheet_(sheet) {}

Cell::~Cell() {}

void Cell::Set(std::string text) {
    std::unique_ptr<Impl> impl;

    if (text.empty()) impl = std::make_unique<EmptyImpl>();
    else if (text.size() > 1 && text[0] == FORMULA_SIGN) impl = std::make_unique<FormulaImpl>(std::move(text), sheet_);
    else impl = std::make_unique<TextImpl>(std::move(text));

    if (WouldIntroduceCircularDependency(*impl)) throw CircularDependencyException("");
    impl_ = std::move(impl);

    for (Cell* outgoing : r_nodes_) {
        outgoing->l_nodes_.erase(this);
    }

    r_nodes_.clear();

    for (const auto& pos : impl_->GetReferencedCells()) {
        Cell* outgoing = sheet_.GetCellPtr(pos);
        if (!outgoing) {
            sheet_.SetCell(pos, "");
            outgoing = sheet_.GetCellPtr(pos);
        }
        r_nodes_.insert(outgoing);
        outgoing->l_nodes_.insert(this);
    }

    InvalidateCacheRecursive(true);
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const {
    return impl_->GetValue();
}

std::string Cell::GetText() const {
    return impl_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const {
    return impl_->GetReferencedCells();
}

bool Cell::IsReferenced() const {
    return !l_nodes_.empty();
}