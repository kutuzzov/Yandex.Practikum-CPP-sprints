#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>


// Реализуйте следующие методы
Cell::Cell() {
	impl_ = std::make_unique<EmptyImpl>();
}

Cell::~Cell() {}

void Cell::Set(std::string text) {
	if (text.size() == 0) impl_ = std::make_unique<EmptyImpl>();
	else if (text.size() > 1 && text[0] == '=') impl_ = std::make_unique<FormulaImpl>(std::move(text));
	else impl_ = std::make_unique<TextImpl>(std::move(text));
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