#include "cell.h"
#include "common.h"
#include "formula.h"
#include "test_runner_p.h"

inline std::ostream& operator<<(std::ostream& output, const CellInterface::Value& value) {
    std::visit([&](const auto& x) { output << x; }, value);
    return output;
}

std::unique_ptr<CellInterface> CreateCell(const std::string& str) {
    std::unique_ptr<CellInterface> cell = std::make_unique<Cell>();
    cell->Set(str);
    return cell;
}

int main()
{

    auto simple_text = CreateCell("simple_text");
    ASSERT_EQUAL(simple_text->GetText(), "simple_text");
    ASSERT_EQUAL(std::get<std::string>(simple_text->GetValue()), "simple_text");

    auto empty_apostroph = CreateCell("'");
    ASSERT_EQUAL(empty_apostroph->GetText(), "'");
    ASSERT_EQUAL(std::get<std::string>(empty_apostroph->GetValue()), "");

    auto apostroph = CreateCell("'apostroph");
    ASSERT_EQUAL(apostroph->GetText(), "'apostroph");
    ASSERT_EQUAL(std::get<std::string>(apostroph->GetValue()), "apostroph");

    auto text_formula = CreateCell("'=1+2");
    ASSERT_EQUAL(text_formula->GetText(), "'=1+2");
    ASSERT_EQUAL(std::get<std::string>(text_formula->GetValue()), "=1+2");

    auto empty_formula = CreateCell("=");
    ASSERT_EQUAL(empty_formula->GetText(), "=");
    ASSERT_EQUAL(std::get<std::string>(empty_formula->GetValue()), "=");

    auto formula = CreateCell("=1+2");
    ASSERT_EQUAL(formula->GetText(), "=1+2");
    ASSERT_EQUAL(std::get<double>(formula->GetValue()), 3);

    auto switch_text = CreateCell("1+2");
    ASSERT_EQUAL(switch_text->GetText(), "1+2");
    ASSERT_EQUAL(std::get<std::string>(switch_text->GetValue()), "1+2");

    switch_text->Set("=1+2");
    ASSERT_EQUAL(switch_text->GetText(), "=1+2");
    ASSERT_EQUAL(std::get<double>(switch_text->GetValue()), 3);

    switch_text->Set("=1/0");
    ASSERT_EQUAL(switch_text->GetText(), "=1/0");
    std::cout << std::get<FormulaError>(switch_text->GetValue()) << std::endl;

    std::cout << "All tests passed" << std::endl;
    return 0;
}
