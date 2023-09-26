#include "runtime.h"
#include "test_runner_p.h"

#include <functional>

using namespace std;

namespace runtime {

namespace {
class Logger : public Object {
public:
    static int instance_count;

    explicit Logger(int value_ = 0)
        : id_(value_)  //
    {
        ++instance_count;
    }

    Logger(const Logger& rhs)
        : id_(rhs.id_)  //
    {
        ++instance_count;
    }

    Logger(Logger&& rhs) noexcept
        : id_(rhs.id_)  //
    {
        ++instance_count;
    }

    Logger& operator=(const Logger& /*rhs*/) = default;
    Logger& operator=(Logger&& /*rhs*/) = default;

    [[nodiscard]] int GetId() const {
        return id_;
    }

    ~Logger()  // NOLINT(hicpp-use-override,modernize-use-override)
    {
        --instance_count;
    }

    void Print(ostream& os, [[maybe_unused]] Context& context) override {
        os << id_;
    }

private:
    int id_;
};

int Logger::instance_count = 0;

void TestNumber() {
    Number num(127);

    DummyContext context;

    num.Print(context.output, context);
    ASSERT_EQUAL(context.output.str(), "127"s);
    ASSERT_EQUAL(num.GetValue(), 127);
}

void TestString() {
    String word("hello!"s);

    DummyContext context;
    word.Print(context.output, context);
    ASSERT_EQUAL(context.output.str(), "hello!"s);
    ASSERT_EQUAL(word.GetValue(), "hello!"s);
}

void TestBool() {
    Bool t(true);
    ASSERT_EQUAL(t.GetValue(), true);

    DummyContext context;
    ostringstream out;
    t.Print(out, context);
    ASSERT_EQUAL(out.str(), "True"s);
    out.str({});

    Bool f(false);
    ASSERT_EQUAL(f.GetValue(), false);

    f.Print(out, context);
    ASSERT_EQUAL(out.str(), "False"s);

    ASSERT(context.output.str().empty());
}

struct TestMethodBody : Executable {
    using Fn = std::function<ObjectHolder(Closure& closure, Context& context)>;
    Fn body;

    explicit TestMethodBody(Fn body)
        : body(std::move(body)) {
    }

    ObjectHolder Execute(Closure& closure, Context& context) override {
        if (body) {
            return body(closure, context);
        }
        return {};
    }
};

void TestMethodInvocation() {
    DummyContext context;
    Closure base_closure;
    auto base_method_1 = [&base_closure, &context](Closure& closure, Context& ctx) {
        ASSERT_EQUAL(&context, &ctx);
        base_closure = closure;
        return ObjectHolder::Own(Number{123});
    };
    auto base_method_2 = [&base_closure, &context](Closure& closure, Context& ctx) {
        ASSERT_EQUAL(&context, &ctx);
        base_closure = closure;
        return ObjectHolder::Own(Number{456});
    };
    vector<Method> base_methods;
    base_methods.push_back(
        {"test"s, {"arg1"s, "arg2"s}, make_unique<TestMethodBody>(base_method_1)});
    base_methods.push_back({"test_2"s, {"arg1"s}, make_unique<TestMethodBody>(base_method_2)});
    Class base_class{"Base"s, std::move(base_methods), nullptr};
    ClassInstance base_inst{base_class};
    base_inst.Fields()["base_field"s] = ObjectHolder::Own(String{"hello"s});
    ASSERT(base_inst.HasMethod("test"s, 2U));
    auto res = base_inst.Call(
        "test"s, {ObjectHolder::Own(Number{1}), ObjectHolder::Own(String{"abc"s})}, context);
    ASSERT(Equal(res, ObjectHolder::Own(Number{123}), context));
    ASSERT_EQUAL(base_closure.size(), 3U);
    ASSERT_EQUAL(base_closure.count("self"s), 1U);
    ASSERT_EQUAL(base_closure.at("self"s).Get(), &base_inst);
    ASSERT_EQUAL(base_closure.count("self"s), 1U);
    ASSERT_EQUAL(base_closure.count("arg1"s), 1U);
    ASSERT(Equal(base_closure.at("arg1"s), ObjectHolder::Own(Number{1}), context));
    ASSERT_EQUAL(base_closure.count("arg2"s), 1U);
    ASSERT(Equal(base_closure.at("arg2"s), ObjectHolder::Own(String{"abc"s}), context));
    ASSERT_EQUAL(base_closure.count("base_field"s), 0U);

    Closure child_closure;
    auto child_method_1 = [&child_closure, &context](Closure& closure, Context& ctx) {
        ASSERT_EQUAL(&context, &ctx);
        child_closure = closure;
        return ObjectHolder::Own(String("child"s));
    };
    vector<Method> child_methods;
    child_methods.push_back(
        {"test"s, {"arg1_child"s, "arg2_child"s}, make_unique<TestMethodBody>(child_method_1)});
    Class child_class{"Child"s, std::move(child_methods), &base_class};
    ClassInstance child_inst{child_class};
    ASSERT(child_inst.HasMethod("test"s, 2U));
    base_closure.clear();
    res = child_inst.Call(
        "test"s, {ObjectHolder::Own(String{"value1"s}), ObjectHolder::Own(String{"value2"s})},
        context);
    ASSERT(Equal(res, ObjectHolder::Own(String{"child"s}), context));
    ASSERT(base_closure.empty());
    ASSERT_EQUAL(child_closure.size(), 3U);
    ASSERT_EQUAL(child_closure.count("self"s), 1U);
    ASSERT_EQUAL(child_closure.at("self"s).Get(), &child_inst);
    ASSERT_EQUAL(child_closure.count("arg1_child"s), 1U);
    ASSERT(Equal(child_closure.at("arg1_child"s), (ObjectHolder::Own(String{"value1"s})), context));
    ASSERT_EQUAL(child_closure.count("arg2_child"s), 1U);
    ASSERT(Equal(child_closure.at("arg2_child"s), (ObjectHolder::Own(String{"value2"s})), context));

    ASSERT(child_inst.HasMethod("test_2"s, 1U));
    child_closure.clear();
    res = child_inst.Call("test_2"s, {ObjectHolder::Own(String{":)"s})}, context);
    ASSERT(Equal(res, ObjectHolder::Own(Number{456}), context));
    ASSERT_EQUAL(base_closure.size(), 2U);
    ASSERT_EQUAL(base_closure.count("self"s), 1U);
    ASSERT_EQUAL(base_closure.at("self"s).Get(), &child_inst);
    ASSERT_EQUAL(base_closure.count("arg1"s), 1U);
    ASSERT(Equal(base_closure.at("arg1"s), (ObjectHolder::Own(String{":)"s})), context));

    ASSERT(!child_inst.HasMethod("test"s, 1U));
    ASSERT_THROWS(child_inst.Call("test"s, {ObjectHolder::None()}, context), runtime_error);
}

void TestNonowning() {
    ASSERT_EQUAL(Logger::instance_count, 0);
    Logger logger(784);
    {
        auto oh = ObjectHolder::Share(logger);
        ASSERT(oh);
    }
    ASSERT_EQUAL(Logger::instance_count, 1);

    auto oh = ObjectHolder::Share(logger);
    ASSERT(oh);
    ASSERT(oh.Get() == &logger);

    DummyContext context;
    oh->Print(context.output, context);

    ASSERT_EQUAL(context.output.str(), "784"sv);
}

void TestOwning() {
    ASSERT_EQUAL(Logger::instance_count, 0);
    {
        auto oh = ObjectHolder::Own(Logger());
        ASSERT(oh);
        ASSERT_EQUAL(Logger::instance_count, 1);
    }
    ASSERT_EQUAL(Logger::instance_count, 0);

    auto oh = ObjectHolder::Own(Logger(312));
    ASSERT(oh);
    ASSERT_EQUAL(Logger::instance_count, 1);

    DummyContext context;
    oh->Print(context.output, context);

    ASSERT_EQUAL(context.output.str(), "312"sv);
}

void TestMove() {
    {
        ASSERT_EQUAL(Logger::instance_count, 0);
        Logger logger;

        auto one = ObjectHolder::Share(logger);
        ObjectHolder two = std::move(one);

        ASSERT_EQUAL(Logger::instance_count, 1);
        ASSERT(two.Get() == &logger);
    }
    {
        ASSERT_EQUAL(Logger::instance_count, 0);
        auto one = ObjectHolder::Own(Logger());
        ASSERT_EQUAL(Logger::instance_count, 1);
        Object* stored = one.Get();
        ObjectHolder two = std::move(one);
        ASSERT_EQUAL(Logger::instance_count, 1);

        ASSERT(two.Get() == stored);
        ASSERT(!one);  // NOLINT
    }
}

void TestNullptr() {
    ObjectHolder oh;
    ASSERT(!oh);
    ASSERT(!oh.Get());
}

void TestIsTrue() {
    {
        ASSERT(!IsTrue(ObjectHolder::Own(Bool{false})));
        ASSERT(IsTrue(ObjectHolder::Own(Bool{true})));
    }

    ASSERT(!IsTrue(ObjectHolder::None()));  // None is false

    // Number equal to 0 is false. The rest are true
    {
        ASSERT(!IsTrue(ObjectHolder::Own(Number{0})));
        ASSERT(IsTrue(ObjectHolder::Own(Number{1})));
        ASSERT(IsTrue(ObjectHolder::Own(Number{-1})));
        ASSERT(IsTrue(ObjectHolder::Own(Number{42})));
    }

    // Empty string is equal to false. Non empty string is equal to true
    {
        ASSERT(!IsTrue(ObjectHolder::Own(String{""s})));
        ASSERT(IsTrue(ObjectHolder::Own(String{"0"s})));
        ASSERT(IsTrue(ObjectHolder::Own(String{"1"s})));
        ASSERT(IsTrue(ObjectHolder::Own(String{"abc"s})));
        ASSERT(IsTrue(ObjectHolder::Own(String{"True"s})));
        ASSERT(IsTrue(ObjectHolder::Own(String{"False"s})));
    }

    // Class and ClassInstance objects are converted to false
    {
        Class cls{"Test"s, {}, nullptr};
        ASSERT(!IsTrue(ObjectHolder::Share(cls)));
        ASSERT(!IsTrue(ObjectHolder::Own(ClassInstance{cls})));
    }
}

void TestComparison() {
    auto test_equal = [](const ObjectHolder& lhs, const ObjectHolder& rhs, bool equality_result) {
        DummyContext ctx;
        ASSERT(Equal(lhs, rhs, ctx) == equality_result);
        ASSERT(NotEqual(lhs, rhs, ctx) == !equality_result);
    };

    auto test_less = [](const ObjectHolder& lhs, const ObjectHolder& rhs, bool less_result) {
        DummyContext ctx;
        ASSERT(Less(lhs, rhs, ctx) == less_result);
        ASSERT(GreaterOrEqual(lhs, rhs, ctx) == !less_result);
    };

    auto test_greater = [](const ObjectHolder& lhs, const ObjectHolder& rhs, bool greater_result) {
        DummyContext ctx;
        ASSERT(Greater(lhs, rhs, ctx) == greater_result);
        ASSERT(LessOrEqual(lhs, rhs, ctx) == !greater_result);
    };

    auto test_eq_uncomparable = [](const ObjectHolder& lhs, const ObjectHolder& rhs) {
        DummyContext ctx;
        ASSERT_THROWS(Equal(lhs, rhs, ctx), runtime_error);
        ASSERT_THROWS(NotEqual(lhs, rhs, ctx), runtime_error);
    };

    auto test_lt_uncomparable = [](const ObjectHolder& lhs, const ObjectHolder& rhs) {
        DummyContext ctx;
        ASSERT_THROWS(Less(lhs, rhs, ctx), runtime_error);
        ASSERT_THROWS(GreaterOrEqual(lhs, rhs, ctx), runtime_error);
    };

    auto test_gt_uncomparable = [](const ObjectHolder& lhs, const ObjectHolder& rhs) {
        DummyContext ctx;
        ASSERT_THROWS(Greater(lhs, rhs, ctx), runtime_error);
        ASSERT_THROWS(LessOrEqual(lhs, rhs, ctx), runtime_error);
    };

    // Numbers
    {
        test_equal(ObjectHolder::Own(Number{3}), ObjectHolder::Own(Number{3}), true);
        test_equal(ObjectHolder::Own(Number{3}), ObjectHolder::Own(Number{4}), false);
        test_equal(ObjectHolder::Own(Number{4}), ObjectHolder::Own(Number{3}), false);

        test_less(ObjectHolder::Own(Number{3}), ObjectHolder::Own(Number{4}), true);
        test_less(ObjectHolder::Own(Number{3}), ObjectHolder::Own(Number{3}), false);
        test_less(ObjectHolder::Own(Number{3}), ObjectHolder::Own(Number{2}), false);

        test_greater(ObjectHolder::Own(Number{4}), ObjectHolder::Own(Number{2}), true);
        test_greater(ObjectHolder::Own(Number{3}), ObjectHolder::Own(Number{3}), false);
        test_greater(ObjectHolder::Own(Number{2}), ObjectHolder::Own(Number{3}), false);

        // Numbers can not be compared to other types
        test_eq_uncomparable(ObjectHolder::Own(Number{4}), ObjectHolder::Own(String{"4"s}));
        test_eq_uncomparable(ObjectHolder::Own(Number{0}), ObjectHolder::Own(Bool{false}));

        test_lt_uncomparable(ObjectHolder::Own(Number{0}), ObjectHolder::Own(Bool{false}));
        test_gt_uncomparable(ObjectHolder::Own(Number{0}), ObjectHolder::Own(String{"test"s}));
    }

    // Strings
    {
        test_equal(ObjectHolder::Own(String{""s}), ObjectHolder::Own(String{""s}), true);
        test_equal(ObjectHolder::Own(String{"1"s}), ObjectHolder::Own(String{""s}), false);
        test_equal(ObjectHolder::Own(String{""s}), ObjectHolder::Own(String{"1"s}), false);
        test_equal(ObjectHolder::Own(String{"Hello"s}), ObjectHolder::Own(String{"Hello"s}), true);
        test_equal(ObjectHolder::Own(String{"Hello"s}), ObjectHolder::Own(String{"hello"s}), false);

        test_less(ObjectHolder::Own(String{""s}), ObjectHolder::Own(String{"1"s}), true);
        test_less(ObjectHolder::Own(String{"abb"s}), ObjectHolder::Own(String{"abc"s}), true);
        test_less(ObjectHolder::Own(String{""s}), ObjectHolder::Own(String{""s}), false);
        test_less(ObjectHolder::Own(String{"abc"s}), ObjectHolder::Own(String{"abc"s}), false);
        test_less(ObjectHolder::Own(String{"abc"s}), ObjectHolder::Own(String{"abb"s}), false);

        test_greater(ObjectHolder::Own(String{"1"s}), ObjectHolder::Own(String{""s}), true);
        test_greater(ObjectHolder::Own(String{"abc"s}), ObjectHolder::Own(String{"abb"s}), true);
        test_greater(ObjectHolder::Own(String{""s}), ObjectHolder::Own(String{""s}), false);
        test_greater(ObjectHolder::Own(String{"abc"s}), ObjectHolder::Own(String{"abc"s}), false);
        test_greater(ObjectHolder::Own(String{"abb"s}), ObjectHolder::Own(String{"abc"s}), false);

        // Strings can not be compared to other types
        test_eq_uncomparable(ObjectHolder::Own(String{"123"s}), ObjectHolder::Own(Number{123}));
        test_eq_uncomparable(ObjectHolder::Own(String{"True"s}), ObjectHolder::Own(Bool{true}));

        test_lt_uncomparable(ObjectHolder::Own(String{"123"s}), ObjectHolder::Own(Number{123}));
        test_lt_uncomparable(ObjectHolder::Own(String{"True"s}), ObjectHolder::Own(Bool{true}));

        test_gt_uncomparable(ObjectHolder::Own(String{"123"s}), ObjectHolder::Own(Number{123}));
        test_gt_uncomparable(ObjectHolder::Own(String{"True"s}), ObjectHolder::Own(Bool{true}));
    }

    // Booleans
    {
        test_equal(ObjectHolder::Own(Bool{true}), ObjectHolder::Own(Bool{true}), true);
        test_equal(ObjectHolder::Own(Bool{false}), ObjectHolder::Own(Bool{false}), true);
        test_equal(ObjectHolder::Own(Bool{false}), ObjectHolder::Own(Bool{true}), false);
        test_equal(ObjectHolder::Own(Bool{true}), ObjectHolder::Own(Bool{false}), false);

        test_less(ObjectHolder::Own(Bool{false}), ObjectHolder::Own(Bool{true}), true);
        test_less(ObjectHolder::Own(Bool{true}), ObjectHolder::Own(Bool{true}), false);
        test_less(ObjectHolder::Own(Bool{false}), ObjectHolder::Own(Bool{false}), false);
        test_less(ObjectHolder::Own(Bool{true}), ObjectHolder::Own(Bool{false}), false);

        test_greater(ObjectHolder::Own(Bool{false}), ObjectHolder::Own(Bool{true}), false);
        test_greater(ObjectHolder::Own(Bool{true}), ObjectHolder::Own(Bool{true}), false);
        test_greater(ObjectHolder::Own(Bool{false}), ObjectHolder::Own(Bool{false}), false);
        test_greater(ObjectHolder::Own(Bool{true}), ObjectHolder::Own(Bool{false}), true);

        // Booleans can not be compared to other types
        test_eq_uncomparable(ObjectHolder::Own(Bool{true}), ObjectHolder::Own(String{"true"s}));
        test_eq_uncomparable(ObjectHolder::Own(Bool{false}), ObjectHolder::Own(Number{0}));

        test_lt_uncomparable(ObjectHolder::Own(Bool{true}), ObjectHolder::Own(String{"true"s}));
        test_lt_uncomparable(ObjectHolder::Own(Bool{false}), ObjectHolder::Own(Number{0}));

        test_gt_uncomparable(ObjectHolder::Own(Bool{true}), ObjectHolder::Own(String{"true"s}));
        test_gt_uncomparable(ObjectHolder::Own(Bool{false}), ObjectHolder::Own(Number{0}));
    }

    // None
    {
        test_equal(ObjectHolder::None(), ObjectHolder::None(), true);

        test_eq_uncomparable(ObjectHolder::None(), ObjectHolder::Own(Bool{false}));
        test_eq_uncomparable(ObjectHolder::None(), ObjectHolder::Own(String{"None"s}));

        test_lt_uncomparable(ObjectHolder::None(), ObjectHolder::None());
        test_gt_uncomparable(ObjectHolder::None(), ObjectHolder::None());
    }

    // Class instances
    {
        Closure eq_closure;
        auto eq_result = ObjectHolder::Own(Bool{true});
        auto eq_body = [&eq_closure, &eq_result](Closure& closure, [[maybe_unused]] Context& ctx) {
            eq_closure = closure;
            return eq_result;
        };

        Closure lt_closure;
        auto lt_result = ObjectHolder::Own(Bool{true});
        auto lt_body = [&lt_closure, &lt_result](Closure& closure, [[maybe_unused]] Context& ctx) {
            lt_closure = closure;
            return lt_result;
        };

        std::vector<Method> cls1_methods;
        cls1_methods.push_back({"__eq__"s, {"rhs"s}, std::make_unique<TestMethodBody>(eq_body)});
        cls1_methods.push_back({"__lt__"s, {"rhs"s}, std::make_unique<TestMethodBody>(lt_body)});
        Class cls1{"Class1"s, std::move(cls1_methods), nullptr};
        ClassInstance lhs{cls1};

        Class cls2{"Class2"s, {}, nullptr};
        ClassInstance rhs{cls2};

        // Equal / NotEqual
        eq_result = ObjectHolder::Own(Bool{true});
        test_equal(ObjectHolder::Share(lhs), ObjectHolder::Share(rhs), true);
        ASSERT(eq_closure.at("self"s).TryAs<ClassInstance>() == &lhs);
        ASSERT(eq_closure.at("rhs"s).TryAs<ClassInstance>() == &rhs);
        ASSERT(lt_closure.empty());
        eq_result = ObjectHolder::Own(Bool{false});
        test_equal(ObjectHolder::Share(lhs), ObjectHolder::Share(rhs), false);
        eq_closure.clear();
        lt_closure.clear();

        // Less / GreaterOrEqual
        eq_result = ObjectHolder::Own(Bool{false});
        lt_result = ObjectHolder::Own(Bool{true});
        test_less(ObjectHolder::Share(lhs), ObjectHolder::Share(rhs), true);
        ASSERT(lt_closure.at("self"s).TryAs<ClassInstance>() == &lhs);
        ASSERT(lt_closure.at("rhs"s).TryAs<ClassInstance>() == &rhs);
        ASSERT(eq_closure.empty());
        eq_result = ObjectHolder::Own(Bool{true});
        lt_result = ObjectHolder::Own(Bool{false});
        test_less(ObjectHolder::Share(lhs), ObjectHolder::Share(rhs), false);
        eq_closure.clear();
        lt_closure.clear();

        // Greater / LessOrEqual
        eq_result = ObjectHolder::Own(Bool{false});
        lt_result = ObjectHolder::Own(Bool{false});
        test_greater(ObjectHolder::Share(lhs), ObjectHolder::Share(rhs), true);
        ASSERT(eq_closure.at("self"s).TryAs<ClassInstance>() == &lhs);
        ASSERT(eq_closure.at("rhs"s).TryAs<ClassInstance>() == &rhs);
        ASSERT(lt_closure.at("self"s).TryAs<ClassInstance>() == &lhs);
        ASSERT(lt_closure.at("rhs"s).TryAs<ClassInstance>() == &rhs);
        eq_result = ObjectHolder::Own(Bool{true});
        lt_result = ObjectHolder::Own(Bool{true});
        test_greater(ObjectHolder::Share(lhs), ObjectHolder::Share(rhs), false);
    }
}

void TestClass() {
    vector<Method> methods;
    Closure* passed_closure = nullptr;
    Context* passed_context = nullptr;
    auto body = [&passed_closure, &passed_context](Closure& closure, Context& ctx) {
        passed_closure = &closure;
        passed_context = &ctx;
        return ObjectHolder::Own(Number{42});
    };
    methods.push_back({"method"s, {"arg1"s, "arg2"s}, make_unique<TestMethodBody>(body)});
    Class cls{"Test"s, move(methods), nullptr};
    ASSERT_EQUAL(cls.GetName(), "Test"s);
    ASSERT_EQUAL(cls.GetMethod("missing_method"s), nullptr);

    const Method* method = cls.GetMethod("method"s);
    ASSERT(method != nullptr);
    DummyContext ctx;
    Closure closure;
    auto result = method->body->Execute(closure, ctx);
    ASSERT_EQUAL(passed_context, &ctx);
    ASSERT_EQUAL(passed_closure, &closure);
    const Number* returned_number = result.TryAs<Number>();
    ASSERT(returned_number != nullptr && returned_number->GetValue() == 42);

    ostringstream out;
    cls.Print(out, ctx);
    ASSERT(ctx.output.str().empty());
    ASSERT_EQUAL(out.str(), "Class Test"s);
}

void TestClassInstance() {
    vector<Method> methods;

    Closure passed_closure;
    auto str_body = [&passed_closure](Closure& closure, [[maybe_unused]] Context& ctx) {
        passed_closure = closure;
        return ObjectHolder::Own(String{"result"s});
    };

    methods.push_back({"__str__", {}, make_unique<TestMethodBody>(str_body)});

    Class cls{"Test"s, move(methods), nullptr};
    ClassInstance instance{cls};

    ASSERT_EQUAL(&instance.Fields(), &const_cast<const ClassInstance&>(instance).Fields());
    ASSERT(instance.HasMethod("__str__"s, 0));

    ostringstream out;
    DummyContext ctx;
    instance.Print(out, ctx);
    ASSERT_EQUAL(out.str(), "result"s);

    ASSERT_THROWS(instance.Call("missing_method"s, {}, ctx), runtime_error);
}

}  // namespace

void RunObjectsTests(TestRunner& tr) {
    RUN_TEST(tr, runtime::TestNumber);
    RUN_TEST(tr, runtime::TestString);
    RUN_TEST(tr, runtime::TestBool);
    RUN_TEST(tr, runtime::TestMethodInvocation);
    RUN_TEST(tr, runtime::TestIsTrue);
    RUN_TEST(tr, runtime::TestComparison);
    RUN_TEST(tr, runtime::TestClass);
    RUN_TEST(tr, runtime::TestClassInstance);
}

void RunObjectHolderTests(TestRunner& tr) {
    RUN_TEST(tr, runtime::TestNonowning);
    RUN_TEST(tr, runtime::TestOwning);
    RUN_TEST(tr, runtime::TestMove);
    RUN_TEST(tr, runtime::TestNullptr);
}

}  // namespace runtime