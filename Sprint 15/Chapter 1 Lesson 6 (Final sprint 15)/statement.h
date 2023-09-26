#pragma once

#include "runtime.h"

#include <functional>
#include <utility>

namespace ast {

using Statement = runtime::Executable;

// Выражение, возвращающее значение типа T,
// используется как основа для создания констант
template <typename T>
class ValueStatement : public Statement {
public:
    explicit ValueStatement(T v)
        : value_(std::move(v)) {
    }

    runtime::ObjectHolder Execute(runtime::Closure& /*closure*/,
                                  runtime::Context& /*context*/) override {
        return runtime::ObjectHolder::Share(value_);
    }

private:
    T value_;
};

using NumericConst = ValueStatement<runtime::Number>;
using StringConst = ValueStatement<runtime::String>;
using BoolConst = ValueStatement<runtime::Bool>;

/*
Вычисляет значение переменной либо цепочки вызовов полей объектов id1.id2.id3.
Например, выражение circle.center.x - цепочка вызовов полей объектов в инструкции:
x = circle.center.x
*/
class VariableValue : public Statement {
public:
    explicit VariableValue(const std::string& var_name);
    explicit VariableValue(std::vector<std::string> dotted_ids);

    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;

private:
    std::vector<std::string> dotted_ids_;
};

// Присваивает переменной, имя которой задано в параметре var, значение выражения rv
class Assignment : public Statement {
public:
    Assignment(std::string var, std::unique_ptr<Statement> rv);

    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;

private:
    std::string var_;
    std::unique_ptr<Statement> rv_;
};

// Присваивает полю object.field_name значение выражения rv
class FieldAssignment : public Statement {
public:
    FieldAssignment(VariableValue object, std::string field_name, std::unique_ptr<Statement> rv);

    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;

private:
    VariableValue object_;
    std::string field_name_;
    std::unique_ptr<Statement> rv_;
};

// Значение None
class None : public Statement {
public:
    runtime::ObjectHolder Execute([[maybe_unused]] runtime::Closure& closure,
                                  [[maybe_unused]] runtime::Context& context) override {
        return {};
    }
};

// Команда print
class Print : public Statement {
public:
    // Инициализирует команду print для вывода значения выражения argument
    explicit Print(std::unique_ptr<Statement> argument);
    // Инициализирует команду print для вывода списка значений args
    explicit Print(std::vector<std::unique_ptr<Statement>> args);

    // Инициализирует команду print для вывода значения переменной name
    static std::unique_ptr<Print> Variable(const std::string& name);

    // Во время выполнения команды print вывод должен осуществляться в поток, возвращаемый из
    // context.GetOutputStream()
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;

private:
    std::vector<std::unique_ptr<Statement>> args_;
};

// Вызывает метод object.method со списком параметров args
class MethodCall : public Statement {
public:
    MethodCall(std::unique_ptr<Statement> object, std::string method,
               std::vector<std::unique_ptr<Statement>> args);

    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;

private:
    std::unique_ptr<Statement> object_;
    std::string method_;
    std::vector<std::unique_ptr<Statement>> args_;
};

/*
Создаёт новый экземпляр класса class_, передавая его конструктору набор параметров args.
Если в классе отсутствует метод __init__ с заданным количеством аргументов,
то экземпляр класса создаётся без вызова конструктора (поля объекта не будут проинициализированы):

class Person:
  def set_name(name):
    self.name = name

p = Person()
# Поле name будет иметь значение только после вызова метода set_name
p.set_name("Ivan")
*/
class NewInstance : public Statement {
public:
    explicit NewInstance(const runtime::Class& cls);
    NewInstance(const runtime::Class& cls, std::vector<std::unique_ptr<Statement>> args);
    // Возвращает объект, содержащий значение типа ClassInstance
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;

private:
    runtime::ClassInstance class_;
    std::vector<std::unique_ptr<Statement>> args_;
};

// Базовый класс для унарных операций
class UnaryOperation : public Statement {
public:
    explicit UnaryOperation(std::unique_ptr<Statement> argument)
        : argument_(move(argument)) {}

protected:
    std::unique_ptr<Statement> argument_;
};

// Операция str, возвращающая строковое значение своего аргумента
class Stringify : public UnaryOperation {
public:
    using UnaryOperation::UnaryOperation;
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
};

// Родительский класс Бинарная операция с аргументами lhs и rhs
class BinaryOperation : public Statement {
public:
    BinaryOperation(std::unique_ptr<Statement> lhs, std::unique_ptr<Statement> rhs)
        : lhs_(move(lhs)), rhs_(move(rhs)) {}

protected:
    std::unique_ptr<Statement> lhs_;
    std::unique_ptr<Statement> rhs_;
};

// Возвращает результат операции + над аргументами lhs и rhs
class Add : public BinaryOperation {
public:
    using BinaryOperation::BinaryOperation;

    // Поддерживается сложение:
    //  число + число
    //  строка + строка
    //  объект1 + объект2, если у объект1 - пользовательский класс с методом _add__(rhs)
    // В противном случае при вычислении выбрасывается runtime_error
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
};

// Возвращает результат вычитания аргументов lhs и rhs
class Sub : public BinaryOperation {
public:
    using BinaryOperation::BinaryOperation;

    // Поддерживается вычитание:
    //  число - число
    // Если lhs и rhs - не числа, выбрасывается исключение runtime_error
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
};

// Возвращает результат умножения аргументов lhs и rhs
class Mult : public BinaryOperation {
public:
    using BinaryOperation::BinaryOperation;

    // Поддерживается умножение:
    //  число * число
    // Если lhs и rhs - не числа, выбрасывается исключение runtime_error
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
};

// Возвращает результат деления lhs и rhs
class Div : public BinaryOperation {
public:
    using BinaryOperation::BinaryOperation;

    // Поддерживается деление:
    //  число / число
    // Если lhs и rhs - не числа, выбрасывается исключение runtime_error
    // Если rhs равен 0, выбрасывается исключение runtime_error
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
};

// Возвращает результат вычисления логической операции or над lhs и rhs
class Or : public BinaryOperation {
public:
    using BinaryOperation::BinaryOperation;
    // Значение аргумента rhs вычисляется, только если значение lhs
    // после приведения к Bool равно False
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
};

// Возвращает результат вычисления логической операции and над lhs и rhs
class And : public BinaryOperation {
public:
    using BinaryOperation::BinaryOperation;
    // Значение аргумента rhs вычисляется, только если значение lhs
    // после приведения к Bool равно True
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
};

// Возвращает результат вычисления логической операции not над единственным аргументом операции
class Not : public UnaryOperation {
public:
    using UnaryOperation::UnaryOperation;
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;
};

// Составная инструкция (например: тело метода, содержимое ветки if, либо else)
class Compound : public Statement {
public:
    // Конструирует Compound из нескольких инструкций типа unique_ptr<Statement>
    template <typename... Args>
    explicit Compound(Args&&... args) {
        if constexpr (sizeof...(args)) CompoundRecursion(args...);
    }

    // Добавляет очередную инструкцию в конец составной инструкции
    void AddStatement(std::unique_ptr<Statement> stmt) {
        statements_.push_back(std::move(stmt));
    }

    // Последовательно выполняет добавленные инструкции. Возвращает None
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;

private:
    template<typename T0, typename...Args>
    void CompoundRecursion(T0&& val0, Args&&...vals) {
        statements_.push_back(std::move(val0));

        if constexpr (sizeof...(vals)) {
            CompoundRecursion(vals...);
        }
    }

    std::vector<std::unique_ptr<Statement>>statements_;
};

// Тело метода. Как правило, содержит составную инструкцию
class MethodBody : public Statement {
public:
    explicit MethodBody(std::unique_ptr<Statement>&& body);

    // Вычисляет инструкцию, переданную в качестве body.
    // Если внутри body была выполнена инструкция return, возвращает результат return
    // В противном случае возвращает None
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;

private:
    std::unique_ptr<Statement>body_;
};

class ReturnException : public std::exception {
public:
    ReturnException(const runtime::ObjectHolder& object);

    runtime::ObjectHolder GetValue();

private:
    runtime::ObjectHolder object_;
};

// Выполняет инструкцию return с выражением statement
class Return : public Statement {
public:
    explicit Return(std::unique_ptr<Statement> statement)
        : statement_(move(statement)) {}

    // Останавливает выполнение текущего метода. После выполнения инструкции return метод,
    // внутри которого она была исполнена, должен вернуть результат вычисления выражения statement.
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;

private:
    std::unique_ptr<Statement> statement_;
};

// Объявляет класс
class ClassDefinition : public Statement {
public:
    // Гарантируется, что ObjectHolder содержит объект типа runtime::Class
    explicit ClassDefinition(runtime::ObjectHolder cls);

    // Создаёт внутри closure новый объект, совпадающий с именем класса и значением, переданным в
    // конструктор
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;

private:
    runtime::ObjectHolder class_;
};

// Инструкция if <condition> <if_body> else <else_body>
class IfElse : public Statement {
public:
    // Параметр else_body может быть равен nullptr
    IfElse(std::unique_ptr<Statement> condition, std::unique_ptr<Statement> if_body,
           std::unique_ptr<Statement> else_body);

    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;

private:
    std::unique_ptr<Statement> condition_;
    std::unique_ptr<Statement> if_body_;
    std::unique_ptr<Statement> else_body_;
};

// Операция сравнения
class Comparison : public BinaryOperation {
public:
    // Comparator задаёт функцию, выполняющую сравнение значений аргументов
    using Comparator = std::function<bool(const runtime::ObjectHolder&,
                                          const runtime::ObjectHolder&, runtime::Context&)>;

    Comparison(Comparator cmp, std::unique_ptr<Statement> lhs, std::unique_ptr<Statement> rhs);

    // Вычисляет значение выражений lhs и rhs и возвращает результат работы comparator,
    // приведённый к типу runtime::Bool
    runtime::ObjectHolder Execute(runtime::Closure& closure, runtime::Context& context) override;

private:
    Comparator cmp_;
};

}  // namespace ast