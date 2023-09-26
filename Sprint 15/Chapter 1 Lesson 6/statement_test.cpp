#include "statement.h"
#include "test_runner_p.h"

using namespace std;

namespace ast {

using runtime::Closure;
using runtime::ObjectHolder;

namespace {

template <typename T>
void AssertObjectValueEqual(const ObjectHolder& obj, const T& expected, const string& msg) {
    ostringstream one;
    runtime::DummyContext context;
    obj->Print(one, context);

    ostringstream two;
    two << expected;

    AssertEqual(one.str(), two.str(), msg);
}

#define ASSERT_OBJECT_VALUE_EQUAL(obj, expected)                                          \
    {                                                                                     \
        std::ostringstream __assert_equal_private_os;                                     \
        __assert_equal_private_os << #obj << "'s value_ "sv                               \
                                  << " != "sv << #expected << "s, "sv << FILE_NAME << ':' \
                                  << __LINE__;                                            \
        AssertObjectValueEqual(obj, expected, __assert_equal_private_os.str());           \
    }

void TestNumericConst() {
    runtime::DummyContext context;

    NumericConst num(runtime::Number(57));
    Closure empty;

    ObjectHolder o = num.Execute(empty, context);
    ASSERT(o);
    ASSERT(empty.empty());

    ostringstream os;
    o->Print(os, context);
    ASSERT_EQUAL(os.str(), "57"s);

    ASSERT(context.output.str().empty());
}

void TestStringConst() {
    runtime::DummyContext context;

    StringConst value_(runtime::String("Hello!"s));
    Closure empty;

    ObjectHolder o = value_.Execute(empty, context);
    ASSERT(o);
    ASSERT(empty.empty());

    ostringstream os;
    o->Print(os, context);
    ASSERT_EQUAL(os.str(), "Hello!"s);

    ASSERT(context.output.str().empty());
}

void TestVariable() {
    runtime::DummyContext context;

    runtime::Number num(42);
    runtime::String word("Hello"s);

    Closure closure = {{"x"s, ObjectHolder::Share(num)}, {"w"s, ObjectHolder::Share(word)}};
    ASSERT(VariableValue("x"s).Execute(closure, context).Get() == &num);
    ASSERT(VariableValue("w"s).Execute(closure, context).Get() == &word);
    ASSERT_THROWS(VariableValue("unknown"s).Execute(closure, context), std::runtime_error);

    ASSERT(context.output.str().empty());
}

void TestAssignment() {
    runtime::DummyContext context;

    Assignment assign_x("x"s, make_unique<NumericConst>(runtime::Number(57)));
    Assignment assign_y("y"s, make_unique<StringConst>(runtime::String("Hello"s)));

    Closure closure = {{"y"s, ObjectHolder::Own(runtime::Number(42))}};

    {
        ObjectHolder o = assign_x.Execute(closure, context);
        ASSERT(o);
        ASSERT_OBJECT_VALUE_EQUAL(o, 57);
    }
    ASSERT(closure.find("x"s) != closure.end());
    ASSERT_OBJECT_VALUE_EQUAL(closure.at("x"s), 57);

    {
        ObjectHolder o = assign_y.Execute(closure, context);
        ASSERT(o);
        ASSERT_OBJECT_VALUE_EQUAL(o, "Hello"s);
    }
    ASSERT(closure.find("y"s) != closure.end());
    ASSERT_OBJECT_VALUE_EQUAL(closure.at("y"s), "Hello"s);

    ASSERT(context.output.str().empty());
}

void TestFieldAssignment() {
    runtime::DummyContext context;

    runtime::Class empty("Empty"s, {}, nullptr);
    runtime::ClassInstance object{empty};

    FieldAssignment assign_x(VariableValue{"self"s}, "x"s,
                             make_unique<NumericConst>(runtime::Number(57)));
    FieldAssignment assign_y(VariableValue{"self"s}, "y"s, make_unique<NewInstance>(empty));

    Closure closure = {{"self"s, ObjectHolder::Share(object)}};

    {
        ObjectHolder o = assign_x.Execute(closure, context);
        ASSERT(o);
        ASSERT_OBJECT_VALUE_EQUAL(o, 57);
    }
    ASSERT(object.Fields().find("x"s) != object.Fields().end());
    ASSERT_OBJECT_VALUE_EQUAL(object.Fields().at("x"s), 57);

    assign_y.Execute(closure, context);
    FieldAssignment assign_yz(
        VariableValue{vector<string>{"self"s, "y"s}}, "z"s,
        make_unique<StringConst>(runtime::String("Hello, world! Hooray! Yes-yes!!!"s)));
    {
        ObjectHolder o = assign_yz.Execute(closure, context);
        ASSERT(o);
        ASSERT_OBJECT_VALUE_EQUAL(o, "Hello, world! Hooray! Yes-yes!!!"s);
    }

    ASSERT(object.Fields().find("y"s) != object.Fields().end());
    const auto* subobject = object.Fields().at("y"s).TryAs<runtime::ClassInstance>();
    ASSERT(subobject != nullptr && subobject->Fields().find("z"s) != subobject->Fields().end());
    ASSERT_OBJECT_VALUE_EQUAL(subobject->Fields().at("z"s), "Hello, world! Hooray! Yes-yes!!!"s);

    ASSERT(context.output.str().empty());
}

void TestPrintVariable() {
    runtime::DummyContext context;

    Closure closure = {{"y"s, ObjectHolder::Own(runtime::Number(42))}};

    auto print_statement = Print::Variable("y"s);
    print_statement->Execute(closure, context);

    ASSERT_EQUAL(context.output.str(), "42\n"s);
}

void TestPrintMultipleStatements() {
    runtime::DummyContext context;

    runtime::String hello("hello"s);
    Closure closure = {{"word"s, ObjectHolder::Share(hello)}, {"empty"s, ObjectHolder::None()}};

    vector<unique_ptr<Statement>> args;
    args.push_back(make_unique<VariableValue>("word"s));
    args.push_back(make_unique<NumericConst>(57));
    args.push_back(make_unique<StringConst>("Python"s));
    args.push_back(make_unique<VariableValue>("empty"s));

    Print(std::move(args)).Execute(closure, context);

    ASSERT_EQUAL(context.output.str(), "hello 57 Python None\n"s);
}

void TestStringify() {
    runtime::DummyContext context;

    Closure empty;

    {
        auto result = Stringify(make_unique<NumericConst>(57)).Execute(empty, context);
        ASSERT_OBJECT_VALUE_EQUAL(result, "57"s);
        ASSERT(result.TryAs<runtime::String>());
    }
    {
        auto result = Stringify(make_unique<StringConst>("Wazzup!"s)).Execute(empty, context);
        ASSERT_OBJECT_VALUE_EQUAL(result, "Wazzup!"s);
        ASSERT(result.TryAs<runtime::String>());
    }
    {
        vector<runtime::Method> methods;
        methods.push_back({"__str__"s, {}, make_unique<NumericConst>(842)});

        runtime::Class cls("BoxedValue"s, std::move(methods), nullptr);

        auto result = Stringify(make_unique<NewInstance>(cls)).Execute(empty, context);
        ASSERT_OBJECT_VALUE_EQUAL(result, "842"s);
        ASSERT(result.TryAs<runtime::String>());
    }
    {
        runtime::Class cls("BoxedValue"s, {}, nullptr);
        runtime::Closure closure{{"x"s, ObjectHolder::Own(runtime::ClassInstance{cls})}};

        std::ostringstream expected_output;
        expected_output << closure.at("x"s).Get();

        Stringify str(make_unique<VariableValue>("x"s));
        ASSERT_OBJECT_VALUE_EQUAL(str.Execute(closure, context), expected_output.str());
    }
    {
        Stringify str(make_unique<None>());
        ASSERT_OBJECT_VALUE_EQUAL(str.Execute(empty, context), "None"s);
    }

    ASSERT(context.output.str().empty());
}

void TestNumbersAddition() {
    runtime::DummyContext context;

    Add sum(make_unique<NumericConst>(23), make_unique<NumericConst>(34));

    Closure empty;
    ASSERT_OBJECT_VALUE_EQUAL(sum.Execute(empty, context), 57);

    ASSERT(context.output.str().empty());
}

void TestStringsAddition() {
    runtime::DummyContext context;

    Add sum(make_unique<StringConst>("23"s), make_unique<StringConst>("34"s));

    Closure empty;
    ASSERT_OBJECT_VALUE_EQUAL(sum.Execute(empty, context), "2334"s);

    ASSERT(context.output.str().empty());
}

void TestBadAddition() {
    runtime::DummyContext context;

    Closure empty;

    ASSERT_THROWS(
        Add(make_unique<NumericConst>(42), make_unique<StringConst>("4"s)).Execute(empty, context),
        std::runtime_error);
    ASSERT_THROWS(
        Add(make_unique<StringConst>("4"s), make_unique<NumericConst>(42)).Execute(empty, context),
        std::runtime_error);
    ASSERT_THROWS(Add(make_unique<None>(), make_unique<StringConst>("4"s)).Execute(empty, context),
                  std::runtime_error);
    ASSERT_THROWS(Add(make_unique<None>(), make_unique<None>()).Execute(empty, context),
                  std::runtime_error);

    ASSERT(context.output.str().empty());
}

void TestSuccessfulClassInstanceAdd() {
    runtime::DummyContext context;

    vector<runtime::Method> methods;
    methods.push_back({"__add__"s,
                       {"value_"s},
                       make_unique<Add>(make_unique<StringConst>("hello, "s),
                                        make_unique<VariableValue>("value_"s))});

    runtime::Class cls("BoxedValue"s, std::move(methods), nullptr);

    Closure empty;
    auto result = Add(make_unique<NewInstance>(cls), make_unique<StringConst>("world"s))
                      .Execute(empty, context);
    ASSERT_OBJECT_VALUE_EQUAL(result, "hello, world"s);

    ASSERT(context.output.str().empty());
}

void TestClassInstanceAddWithoutMethod() {
    runtime::DummyContext context;

    runtime::Class cls("BoxedValue"s, {}, nullptr);

    Closure empty;
    Add addition(make_unique<NewInstance>(cls), make_unique<StringConst>("world"s));
    ASSERT_THROWS(addition.Execute(empty, context), std::runtime_error);

    ASSERT(context.output.str().empty());
}

void TestCompound() {
    runtime::DummyContext context;

    Compound cpd{
        make_unique<Assignment>("x"s, make_unique<StringConst>("one"s)),
        make_unique<Assignment>("y"s, make_unique<NumericConst>(2)),
        make_unique<Assignment>("z"s, make_unique<VariableValue>("x"s)),
    };

    Closure closure;
    auto result = cpd.Execute(closure, context);

    ASSERT_OBJECT_VALUE_EQUAL(closure.at("x"s), "one"s);
    ASSERT_OBJECT_VALUE_EQUAL(closure.at("y"s), 2);
    ASSERT_OBJECT_VALUE_EQUAL(closure.at("z"s), "one"s);

    ASSERT(!result);

    ASSERT(context.output.str().empty());
}

void TestFields() {
    runtime::DummyContext context;

    vector<runtime::Method> methods;

    methods.push_back({"__init__"s,
                       {},
                       {make_unique<FieldAssignment>(VariableValue{"self"s}, "value"s,
                                                     make_unique<NumericConst>(0))}});
    methods.push_back(
        {"value"s, {}, {make_unique<VariableValue>(vector<string>{"self"s, "value"s})}});
    methods.push_back(
        {"add"s,
         {"x"s},
         {make_unique<FieldAssignment>(
             VariableValue{"self"s}, "value"s,
             make_unique<Add>(make_unique<VariableValue>(vector<string>{"self"s, "value"s}),
                              make_unique<VariableValue>("x"s)))}});

    runtime::Class cls("BoxedValue"s, std::move(methods), nullptr);
    runtime::ClassInstance inst(cls);

    inst.Call("__init__"s, {}, context);

    for (int i = 1, expected = 0; i < 10; expected += i, ++i) {
        auto fv = inst.Call("value"s, {}, context);
        auto* obj = fv.TryAs<runtime::Number>();
        ASSERT(obj);
        ASSERT_EQUAL(obj->GetValue(), expected);

        inst.Call("add"s, {ObjectHolder::Own(runtime::Number(i))}, context);
    }

    ASSERT(context.output.str().empty());
}

void TestBaseClass() {
    vector<runtime::Method> methods;
    methods.push_back({"GetValue"s, {}, make_unique<VariableValue>(vector{"self"s, "value"s})});
    methods.push_back({"SetValue"s,
                       {"x"s},
                       make_unique<FieldAssignment>(VariableValue{"self"s}, "value"s,
                                                    make_unique<ast::VariableValue>("x"s))});

    runtime::Class cls("BoxedValue"s, move(methods), nullptr);

    ASSERT_EQUAL(cls.GetName(), "BoxedValue"s);
    {
        const auto* m = cls.GetMethod("GetValue"s);
        ASSERT(m != nullptr);
        ASSERT_EQUAL(m->name, "GetValue"s);
        ASSERT(m->formal_params.empty());
    }
    {
        const auto* m = cls.GetMethod("SetValue"s);
        ASSERT(m != nullptr);
        ASSERT_EQUAL(m->name, "SetValue"s);
        ASSERT_EQUAL(m->formal_params.size(), 1U);
    }
    ASSERT(!cls.GetMethod("AsString"s));
}

void TestInheritance() {
    vector<runtime::Method> methods;
    methods.push_back({"GetValue"s, {}, make_unique<VariableValue>(vector{"self"s, "value"s})});
    methods.push_back({"SetValue"s,
                       {"x"s},
                       make_unique<FieldAssignment>(VariableValue{"self"s}, "value"s,
                                                    make_unique<VariableValue>("x"s))});

    runtime::Class base("BoxedValue"s, std::move(methods), nullptr);

    methods.clear();
    methods.push_back({"GetValue"s, {"z"s}, make_unique<VariableValue>("z"s)});
    methods.push_back({"AsString"s, {}, make_unique<StringConst>("value"s)});
    runtime::Class cls("StringableValue"s, std::move(methods), &base);

    ASSERT_EQUAL(cls.GetName(), "StringableValue"s);
    {
        const auto* m = cls.GetMethod("GetValue"s);
        ASSERT(m != nullptr);
        ASSERT_EQUAL(m->name, "GetValue"s);
        ASSERT_EQUAL(m->formal_params.size(), 1U);
    }
    {
        const auto* m = cls.GetMethod("SetValue"s);
        ASSERT(m != nullptr);
        ASSERT_EQUAL(m->name, "SetValue"s);
        ASSERT_EQUAL(m->formal_params.size(), 1U);
    }
    {
        const auto* m = cls.GetMethod("AsString"s);
        ASSERT(m != nullptr);
        ASSERT_EQUAL(m->name, "AsString"s);
        ASSERT(m->formal_params.empty());
    }
    ASSERT(!cls.GetMethod("AsStringValue"s));
}

void TestOr() {
    auto test_or = [](bool lhs, bool rhs) {
        Or or_statement{make_unique<BoolConst>(lhs), make_unique<BoolConst>(rhs)};
        Closure closure;
        runtime::DummyContext context;
        ASSERT_EQUAL(runtime::Equal(or_statement.Execute(closure, context),
                                    ObjectHolder::Own(runtime::Bool(true)), context),
                     lhs || rhs);
    };

    test_or(true, true);
    test_or(true, false);
    test_or(false, true);
    test_or(false, false);
}

void TestAnd() {
    auto test_and = [](bool lhs, bool rhs) {
        And and_statement{make_unique<BoolConst>(lhs), make_unique<BoolConst>(rhs)};
        Closure closure;
        runtime::DummyContext context;
        ASSERT_EQUAL(runtime::Equal(and_statement.Execute(closure, context),
                                    ObjectHolder::Own(runtime::Bool(true)), context),
                     lhs && rhs);
    };

    test_and(true, true);
    test_and(true, false);
    test_and(false, true);
    test_and(false, false);
}

void TestNot() {
    auto test_not = [](bool arg) {
        Not not_statement{make_unique<BoolConst>(arg)};
        Closure closure;
        runtime::DummyContext context;
        ASSERT_EQUAL(runtime::Equal(not_statement.Execute(closure, context),
                                    ObjectHolder::Own(runtime::Bool(true)), context),
                     !arg);
    };

    test_not(true);
    test_not(false);
}

}  // namespace

void RunUnitTests(TestRunner& tr) {
    RUN_TEST(tr, ast::TestNumericConst);
    RUN_TEST(tr, ast::TestStringConst);
    RUN_TEST(tr, ast::TestVariable);
    RUN_TEST(tr, ast::TestAssignment);
    RUN_TEST(tr, ast::TestFieldAssignment);
    RUN_TEST(tr, ast::TestPrintVariable);
    RUN_TEST(tr, ast::TestPrintMultipleStatements);
    RUN_TEST(tr, ast::TestStringify);
    RUN_TEST(tr, ast::TestNumbersAddition);
    RUN_TEST(tr, ast::TestStringsAddition);
    RUN_TEST(tr, ast::TestBadAddition);
    RUN_TEST(tr, ast::TestSuccessfulClassInstanceAdd);
    RUN_TEST(tr, ast::TestClassInstanceAddWithoutMethod);
    RUN_TEST(tr, ast::TestCompound);
    RUN_TEST(tr, ast::TestFields);
    RUN_TEST(tr, ast::TestBaseClass);
    RUN_TEST(tr, ast::TestInheritance);
    RUN_TEST(tr, ast::TestOr);
    RUN_TEST(tr, ast::TestAnd);
    RUN_TEST(tr, ast::TestNot);
}

}  // namespace ast