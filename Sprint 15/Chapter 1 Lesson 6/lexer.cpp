#include "lexer.h"

#include <algorithm>
#include <charconv>
#include <unordered_map>

using namespace std;

namespace parse {

bool operator==(const Token& lhs, const Token& rhs) {
    using namespace token_type;

    if (lhs.index() != rhs.index()) {
        return false;
    }
    if (lhs.Is<Char>()) {
        return lhs.As<Char>().value == rhs.As<Char>().value;
    }
    if (lhs.Is<Number>()) {
        return lhs.As<Number>().value == rhs.As<Number>().value;
    }
    if (lhs.Is<String>()) {
        return lhs.As<String>().value == rhs.As<String>().value;
    }
    if (lhs.Is<Id>()) {
        return lhs.As<Id>().value == rhs.As<Id>().value;
    }
    return true;
}

bool operator!=(const Token& lhs, const Token& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const Token& rhs) {
    using namespace token_type;

#define VALUED_OUTPUT(type) \
    if (auto p = rhs.TryAs<type>()) return os << #type << '{' << p->value << '}';

    VALUED_OUTPUT(Number);
    VALUED_OUTPUT(Id);
    VALUED_OUTPUT(String);
    VALUED_OUTPUT(Char);

#undef VALUED_OUTPUT

#define UNVALUED_OUTPUT(type) \
    if (rhs.Is<type>()) return os << #type;

    UNVALUED_OUTPUT(Class);
    UNVALUED_OUTPUT(Return);
    UNVALUED_OUTPUT(If);
    UNVALUED_OUTPUT(Else);
    UNVALUED_OUTPUT(Def);
    UNVALUED_OUTPUT(Newline);
    UNVALUED_OUTPUT(Print);
    UNVALUED_OUTPUT(Indent);
    UNVALUED_OUTPUT(Dedent);
    UNVALUED_OUTPUT(And);
    UNVALUED_OUTPUT(Or);
    UNVALUED_OUTPUT(Not);
    UNVALUED_OUTPUT(Eq);
    UNVALUED_OUTPUT(NotEq);
    UNVALUED_OUTPUT(LessOrEq);
    UNVALUED_OUTPUT(GreaterOrEq);
    UNVALUED_OUTPUT(None);
    UNVALUED_OUTPUT(True);
    UNVALUED_OUTPUT(False);
    UNVALUED_OUTPUT(Eof);

#undef UNVALUED_OUTPUT

    return os << "Unknown token :("sv;
}

Lexer::Lexer(std::istream& input) {
    string str;
    while (getline(input, str)) {
        if (str.empty()) continue;
        
        size_t i = 0;
        while (str[i] == ' ') ++i;

        if (i % 2 == 0) {
            while (i > indent_) {
                tokens_.push_back(token_type::Indent());
                indent_ += 2;
            }
            while (i < indent_) {
                tokens_.push_back(token_type::Dedent());
                indent_ -= 2;
            }
        }
        str.erase(0, i);

        if (str[i] == '#') continue;
        while (!str.empty()) Parse(str);
        
        tokens_.push_back(token_type::Newline());
    }
    while (indent_) {
        tokens_.push_back(token_type::Dedent());
        indent_ -= 2;
    }

    tokens_.push_back(token_type::Eof());
}

const Token& Lexer::CurrentToken() const {
    return tokens_[id_];

    throw std::logic_error("Not implemented"s);
}

Token Lexer::NextToken() {
    if (id_ + 1 < tokens_.size()) ++id_;
    return tokens_[id_];

    throw std::logic_error("Not implemented"s);
}

void Lexer::Parse(string& str) {
    size_t i = 0;
    while (i < str.size() && str[i] == ' ')
        ++i;
    str.erase(0, i);

    if (str[0] == '\'' || str[0] == '\"') {
        ParseString(str);
    }
    else if ((str[0] >= '0' && str[0] <= '9') || (str.size() > 1 && str[0] == '-' && str[1] >= '1' && str[1] <= '9')) {
        ParseNumber(str);
    }
    else if (str[0] == '_' || (str[0] >= 'a' && str[0] <= 'z') || (str[0] >= 'A' && str[0] <= 'Z') ||
        (str.size() > 1 && (str.substr(0, 2) == "==" || str.substr(0, 2) == "!=" || str.substr(0, 2) == "<=" || str.substr(0, 2) == ">="))) {
        ParseID(str);
    }
    else if (str[0] != '#') {
        tokens_.push_back(token_type::Char{ str[0] });
        str.erase(0, 1);
    }
    else {
        str.clear();
    }
}

void Lexer::ParseNumber(string& str) {
    string value;
    size_t i = 0;

    if (str[0] == '-') {
        tokens_.push_back(token_type::Char{ '-' });
        i = 1;
    }

    while (i < str.size() && str[i] >= '0' && str[i] <= '9') {
        value += str[i++];
    }
    tokens_.push_back(token_type::Number{ stoi(value) });
    str.erase(0, i);

}

void Lexer::ParseString(string& str) {
    string value;
    char separate = str[0];
    size_t i = 1;

    while (i < str.size() && str[i] != separate) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            ++i;
            if (str[i] == 'n')
                value += '\n';
            else if (str[i] == 't')
                value += '\t';
            else if (str[i] == '\'')
                value += '\'';
            else if (str[i] == '\"')
                value += '\"';
            else if (str[i] == '\\')
                value += '\\';
        }
        else
            value += str[i];
        ++i;
    }
    tokens_.push_back(token_type::String{ value });
    str.erase(0, i + 1);
}

void Lexer::ParseID(string& str) {
    string value;
    size_t i = 0;
    while (i < str.size() && str[i] != ' ' && str[i] != '(' && str[i] != ')' && str[i] != ':' && str[i] != ',' && str[i] != '.' && str[i] != '#') {
        value += str[i++];
    }

    if (value == "class")
        tokens_.push_back(token_type::Class());
    else if (value == "return")
        tokens_.push_back(token_type::Return());
    else if (value == "if")
        tokens_.push_back(token_type::If());
    else if (value == "else")
        tokens_.push_back(token_type::Else());
    else if (value == "def")
        tokens_.push_back(token_type::Def());
    else if (value == "print")
        tokens_.push_back(token_type::Print());
    else if (value == "  ")
        tokens_.push_back(token_type::Indent());
    else if (value == "  ")
        tokens_.push_back(token_type::Dedent());
    else if (value == "and")
        tokens_.push_back(token_type::And());
    else if (value == "or")
        tokens_.push_back(token_type::Or());
    else if (value == "not")
        tokens_.push_back(token_type::Not());
    else if (value == "==")
        tokens_.push_back(token_type::Eq());
    else if (value == "!=")
        tokens_.push_back(token_type::NotEq());
    else if (value == "<=")
        tokens_.push_back(token_type::LessOrEq());
    else if (value == ">=")
        tokens_.push_back(token_type::GreaterOrEq());
    else if (value == "None")
        tokens_.push_back(token_type::None());
    else if (value == "True")
        tokens_.push_back(token_type::True());
    else if (value == "False")
        tokens_.push_back(token_type::False());
    else
        tokens_.push_back(token_type::Id{ value });

    str.erase(0, i);
}

}  // namespace parse