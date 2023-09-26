#include "lexer.h"
#include "test_runner_p.h"

#include <sstream>
#include <string>

using namespace std;

namespace parse {

namespace {
void TestSimpleAssignment() {
    istringstream input("x = 42\n"s);
    Lexer lexer(input);

    ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Id{"x"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Number{42}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
}

void TestKeywords() {
    istringstream input("class return if else def print or None and not True False"s);
    Lexer lexer(input);

    ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Class{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Return{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::If{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Else{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Def{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Print{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Or{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::None{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::And{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Not{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::True{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::False{}));
}

void TestNumbers() {
    istringstream input("42 15 -53"s);
    Lexer lexer(input);

    ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Number{42}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Number{15}));
    // Отрицательные числа формируются на этапе синтаксического анализа
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'-'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Number{53}));
}

void TestIds() {
    istringstream input("x    _42 big_number   Return Class  dEf"s);
    Lexer lexer(input);

    ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Id{"x"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"_42"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"big_number"s}));
    ASSERT_EQUAL(lexer.NextToken(),
                 Token(token_type::Id{"Return"s}));  // keywords are case-sensitive
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"Class"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"dEf"s}));
}

void TestStrings() {
    istringstream input(
        R"('word' "two words" 'long string with a double quote " inside' "another long string with single quote ' inside")"s);
    Lexer lexer(input);

    ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::String{"word"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::String{"two words"s}));
    ASSERT_EQUAL(lexer.NextToken(),
                 Token(token_type::String{"long string with a double quote \" inside"s}));
    ASSERT_EQUAL(lexer.NextToken(),
                 Token(token_type::String{"another long string with single quote ' inside"s}));
}

void TestOperations() {
    istringstream input("+-*/= > < != == <> <= >="s);
    Lexer lexer(input);

    ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Char{'+'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'-'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'*'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'/'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'>'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'<'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::NotEq{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eq{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'<'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'>'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::LessOrEq{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::GreaterOrEq{}));
}

void TestIndentsAndNewlines() {
    istringstream input(R"(
no_indent
  indent_one
    indent_two
      indent_three
      indent_three
      indent_three
    indent_two
  indent_one
    indent_two
no_indent
)"s);

    Lexer lexer(input);

    ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Id{"no_indent"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Indent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"indent_one"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Indent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"indent_two"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Indent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"indent_three"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"indent_three"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"indent_three"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Dedent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"indent_two"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Dedent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"indent_one"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Indent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"indent_two"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Dedent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Dedent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"no_indent"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
}

void TestEmptyLinesAreIgnored() {
    istringstream input(R"(
x = 1
  y = 2

  z = 3


)"s);
    Lexer lexer(input);

    ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Id{"x"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Number{1}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Indent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"y"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Number{2}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    // Пустая строка, состоящая только из пробельных символов не меняет текущий отступ,
    // поэтому следующая лексема — это Id, а не Dedent
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"z"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Number{3}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Dedent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
}

void TestMythonProgram() {
    istringstream input(R"(
x = 4
y = "hello"

class Point:
  def __init__(self, x, y):
    self.x = x
    self.y = y

  def __str__(self):
    return str(x) + ' ' + str(y)

p = Point(1, 2)
print str(p)
)"s);
    Lexer lexer(input);

    ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Id{"x"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Number{4}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"y"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::String{"hello"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Class{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"Point"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{':'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Indent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Def{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"__init__"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'('}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"self"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{','}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"x"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{','}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"y"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{')'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{':'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Indent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"self"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'.'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"x"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"x"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"self"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'.'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"y"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"y"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Dedent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Def{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"__str__"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'('}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"self"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{')'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{':'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Indent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Return{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"str"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'('}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"x"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{')'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'+'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::String{" "s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'+'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"str"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'('}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"y"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{')'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Dedent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Dedent{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"p"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'='}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"Point"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'('}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Number{1}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{','}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Number{2}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{')'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Print{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"str"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{'('}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"p"s}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Char{')'}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
    ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
}

void TestExpect() {
    istringstream is("bugaga"s);
    Lexer lex(is);

    ASSERT_DOESNT_THROW(lex.Expect<token_type::Id>());
    ASSERT_EQUAL(lex.Expect<token_type::Id>().value, "bugaga"s);
    ASSERT_DOESNT_THROW(lex.Expect<token_type::Id>("bugaga"s));
    ASSERT_THROWS(lex.Expect<token_type::Id>("widget"s), LexerError);
    ASSERT_THROWS(lex.Expect<token_type::Return>(), LexerError);
}

void TestExpectNext() {
    istringstream is("+ bugaga + def 52"s);
    Lexer lex(is);

    ASSERT_EQUAL(lex.CurrentToken(), Token(token_type::Char{'+'}));
    ASSERT_DOESNT_THROW(lex.ExpectNext<token_type::Id>());
    ASSERT_DOESNT_THROW(lex.ExpectNext<token_type::Char>('+'));
    ASSERT_THROWS(lex.ExpectNext<token_type::Newline>(), LexerError);
    ASSERT_THROWS(lex.ExpectNext<token_type::Number>(57), LexerError);
}

void TestAlwaysEmitsNewlineAtTheEndOfNonemptyLine() {
    {
        istringstream is("a b"s);
        Lexer lexer(is);

        ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Id{"a"s}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"b"s}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
    }
    {
        istringstream is("+"s);
        Lexer lexer(is);

        ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Char{'+'}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
    }
}
void TestCommentsAreIgnored() {
    {
        istringstream is(R"(# comment
)"s);
        Lexer lexer(is);

        ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Eof{}));
    }
    {
        istringstream is(R"(# comment

)"s);
        Lexer lexer(is);
        ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Eof{}));
    }
    {
        istringstream is(R"(# comment
x #another comment
abc#
'#'
"#123"
#)"s);

        Lexer lexer(is);
        ASSERT_EQUAL(lexer.CurrentToken(), Token(token_type::Id{"x"s}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Id{"abc"s}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::String{"#"s}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::String{"#123"s}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Newline{}));
        ASSERT_EQUAL(lexer.NextToken(), Token(token_type::Eof{}));
    }
}
}  // namespace

void RunOpenLexerTests(TestRunner& tr) {
    RUN_TEST(tr, parse::TestSimpleAssignment);
    RUN_TEST(tr, parse::TestKeywords);
    RUN_TEST(tr, parse::TestNumbers);
    RUN_TEST(tr, parse::TestIds);
    RUN_TEST(tr, parse::TestStrings);
    RUN_TEST(tr, parse::TestOperations);
    RUN_TEST(tr, parse::TestIndentsAndNewlines);
    RUN_TEST(tr, parse::TestEmptyLinesAreIgnored);
    RUN_TEST(tr, parse::TestExpect);
    RUN_TEST(tr, parse::TestExpectNext);
    RUN_TEST(tr, parse::TestMythonProgram);
    RUN_TEST(tr, parse::TestAlwaysEmitsNewlineAtTheEndOfNonemptyLine);
    RUN_TEST(tr, parse::TestCommentsAreIgnored);
}

}  // namespace parse