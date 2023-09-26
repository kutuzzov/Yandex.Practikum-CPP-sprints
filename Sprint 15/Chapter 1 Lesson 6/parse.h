#pragma once

#include <memory>
#include <stdexcept>

namespace parse {
class Lexer;
}

namespace runtime {
class Executable;
}

struct ParseError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

std::unique_ptr<runtime::Executable> ParseProgram(parse::Lexer& lexer);