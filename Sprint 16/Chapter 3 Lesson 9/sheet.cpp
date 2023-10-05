#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text) {
    if (!pos.IsValid()) throw InvalidPositionException("Invalid position");
    
    cells_[pos].Set(text);
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if (!pos.IsValid()) throw InvalidPositionException("Invalid position");
    
    auto it = cells_.find(pos);
    if (it == cells_.end()) return nullptr;

    const Cell* cell = &cells_.at(pos);
    if (cell->GetText().size() == 0) return nullptr;

    return cell;
}

CellInterface* Sheet::GetCell(Position pos) {
    if (!pos.IsValid()) throw InvalidPositionException("Invalid position");

    auto it = cells_.find(pos);
    if (it == cells_.end()) return nullptr;

    Cell* cell = &cells_.at(pos);
    if (cell->GetText().size() == 0) return nullptr;

    return cell;
}

void Sheet::ClearCell(Position pos) {
    if (!pos.IsValid()) throw InvalidPositionException("Invalid position");

    cells_[pos].Clear();
}

Size Sheet::GetPrintableSize() const {
    Size result{ 0, 0 };
    
    if (cells_.begin() == cells_.end()) return result;
    for (auto it = cells_.begin(); it != cells_.end(); ++it) {
        if (it->second.GetText().size() == 0) continue;
        const int col = it->first.col;
        const int row = it->first.row;

        if (result.cols <= col) {
            result.cols = col + 1;
        }
        if (result.rows <= row) {
            result.rows = row + 1;
        }
    }
    return { result.rows, result.cols };
}

void Sheet::PrintValues(std::ostream& output) const {
    Size size = GetPrintableSize();
    for (int row = 0; row < size.rows; ++row) {
        bool first = true;
        for (int col = 0; col < size.cols; ++col) {
            if (!first) {
                output << "\t";
            }
            first = false;
            Position pos = { row, col };
            auto it = cells_.find(pos);
            if (it != cells_.end()) {
                auto value = cells_.at(pos).GetValue();
                std::visit([&output](auto&& arg) {output << arg; }, value);
            }
        }
        output << "\n";
    }
}
void Sheet::PrintTexts(std::ostream& output) const {
    Size size = GetPrintableSize();
    for (int row = 0; row < size.rows; ++row) {
        bool first = true;
        for (int col = 0; col < size.cols; ++col) {
            if (!first) {
                output << "\t";
            }
            first = false;
            Position pos = { row, col };
            auto it = cells_.find(pos);
            if (it != cells_.end()) {
                output << cells_.at(pos).GetText();
            }
        }
        output << "\n";
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}