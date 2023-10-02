#include "FormulaAST.h"
#include "common.h"
#include "test_runner_p.h"

namespace {
double ExecuteASTFormula(const std::string& expression) {
    return ParseFormulaAST(expression).Execute();
}
}  // namespace

int main() {
    ASSERT_EQUAL(ExecuteASTFormula("1"), 1.0);
    ASSERT_EQUAL(ExecuteASTFormula("1+2*3-4/5"), 6.2);
    try {
        ExecuteASTFormula("1/0");
    } catch (const FormulaError& fe) {
        std::cout << fe.what() << std::endl;
    }

    std::cout << "Tests Passed" << std::endl;
    return 0;
}
