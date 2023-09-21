#include "lexer.h"
#include "test_runner_p.h"

#include <iostream>

namespace parse {
void RunOpenLexerTests(TestRunner& tr);
}

int main() {
    try {
        TestRunner tr;
        parse::RunOpenLexerTests(tr);
        parse::Lexer lexer(std::cin);
        parse::Token t;
        while ((t = lexer.CurrentToken()) != parse::token_type::Eof{}) {
            std::cout << t << std::endl;
            lexer.NextToken();
        }
    } catch (const std::exception& e) {
        std::cerr << e.what();
        return 1;
    }
}
