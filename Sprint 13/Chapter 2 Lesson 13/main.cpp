#include "optional.h"

#include <cassert>
#include <memory>

struct C {
    C() noexcept {
        ++def_ctor;
    }
    C(const C& /*other*/) noexcept {
        ++copy_ctor;
    }
    C(C&& /*other*/) noexcept {
        ++move_ctor;
    }
    C& operator=(const C& other) noexcept {
        if (this != &other) {
            ++copy_assign;
        }
        return *this;
    }
    C& operator=(C&& /*other*/) noexcept {
        ++move_assign;
        return *this;
    }
    ~C() {
        ++dtor;
    }

    void Update() const& {
        ++const_lvalue_call_count;
    }

    void Update() & {
        ++lvalue_call_count;
    }

    void Update() && {
        ++rvalue_call_count;
    }

    static size_t InstanceCount() {
        return def_ctor + copy_ctor + move_ctor - dtor;
    }

    static void Reset() {
        def_ctor = 0;
        copy_ctor = 0;
        move_ctor = 0;
        copy_assign = 0;
        move_assign = 0;
        dtor = 0;
        lvalue_call_count = 0;
        rvalue_call_count = 0;
        const_lvalue_call_count = 0;
    }

    inline static size_t def_ctor = 0;
    inline static size_t copy_ctor = 0;
    inline static size_t move_ctor = 0;
    inline static size_t copy_assign = 0;
    inline static size_t move_assign = 0;
    inline static size_t dtor = 0;

    inline static size_t lvalue_call_count = 0;
    inline static size_t rvalue_call_count = 0;
    inline static size_t const_lvalue_call_count = 0;
};

void TestInitialization() {
    C::Reset();
    {
        Optional<C> o;
        assert(!o.HasValue());
        assert(C::InstanceCount() == 0);
    }
    assert(C::InstanceCount() == 0);

    C::Reset();
    {
        C c;
        Optional<C> o(c);
        assert(o.HasValue());
        assert(C::def_ctor == 1 && C::copy_ctor == 1);
        assert(C::InstanceCount() == 2);
    }
    assert(C::InstanceCount() == 0);

    C::Reset();
    {
        C c;
        Optional<C> o(std::move(c));
        assert(o.HasValue());
        assert(C::def_ctor == 1 && C::move_ctor == 1 && C::copy_ctor == 0 && C::copy_assign == 0
               && C::move_assign == 0);
        assert(C::InstanceCount() == 2);
    }
    assert(C::InstanceCount() == 0);

    C::Reset();
    {
        C c;
        Optional<C> o1(c);
        const Optional<C> o2(o1);
        assert(o1.HasValue());
        assert(o2.HasValue());
        assert(C::def_ctor == 1 && C::move_ctor == 0 && C::copy_ctor == 2 && C::copy_assign == 0
               && C::move_assign == 0);
        assert(C::InstanceCount() == 3);
    }
    assert(C::InstanceCount() == 0);

    C::Reset();
    {
        C c;
        Optional<C> o1(c);
        const Optional<C> o2(std::move(o1));
        assert(C::def_ctor == 1 && C::copy_ctor == 1 && C::move_ctor == 1 && C::copy_assign == 0
               && C::move_assign == 0);
        assert(C::InstanceCount() == 3);
    }
    assert(C::InstanceCount() == 0);
}

void TestAssignment() {
    Optional<C> o1;
    Optional<C> o2;
    {  // Assign a value to empty
        C::Reset();
        C c;
        o1 = c;
        assert(C::def_ctor == 1 && C::copy_ctor == 1 && C::dtor == 0);
    }
    {  // Assign a non-empty to empty
        C::Reset();
        o2 = o1;
        assert(C::copy_ctor == 1 && C::copy_assign == 0 && C::dtor == 0);
    }
    {  // Assign non-empty to non-empty
        C::Reset();
        o2 = o1;
        assert(C::copy_ctor == 0 && C::copy_assign == 1 && C::dtor == 0);
    }
    {  // Assign empty to non-empty
        C::Reset();
        Optional<C> empty;
        o1 = empty;
        assert(C::copy_ctor == 0 && C::dtor == 1);
        assert(!o1.HasValue());
    }
}

void TestMoveAssignment() {
    {  // Assign a value to empty
        Optional<C> o1;
        C::Reset();
        C c;
        o1 = std::move(c);
        assert(C::def_ctor == 1 && C::move_ctor == 1 && C::dtor == 0);
    }
    {  // Assign a non-empty to empty
        Optional<C> o1;
        Optional<C> o2{C{}};
        C::Reset();
        o1 = std::move(o2);
        assert(C::move_ctor == 1 && C::move_assign == 0 && C::dtor == 0);
    }
    {  // Assign non-empty to non-empty
        Optional<C> o1{C{}};
        Optional<C> o2{C{}};
        C::Reset();
        o2 = std::move(o1);
        assert(C::copy_ctor == 0 && C::move_assign == 1 && C::dtor == 0);
    }
    {  // Assign empty to non-empty
        Optional<C> o1{C{}};
        C::Reset();
        Optional<C> empty;
        o1 = std::move(empty);
        assert(C::copy_ctor == 0 && C::move_ctor == 0 && C::move_assign == 0 && C::dtor == 1);
        assert(!o1.HasValue());
    }
}

void TestValueAccess() {
    using namespace std::literals;
    {
        Optional<std::string> o;
        o = "hello"s;
        assert(o.HasValue());
        assert(o.Value() == "hello"s);
        assert(&*o == &o.Value());
        assert(o->length() == 5);
    }
    {
        try {
            Optional<int> o;
            [[maybe_unused]] int v = o.Value();
            assert(false);
        } catch (const BadOptionalAccess& /*e*/) {
        } catch (...) {
            assert(false);
        }
    }
}

void TestReset() {
    C::Reset();
    {
        Optional<C> o{C()};
        assert(o.HasValue());
        o.Reset();
        assert(!o.HasValue());
    }
}

void TestEmplace() {
    struct S {
        S(int i, std::unique_ptr<int>&& p)
            : i(i)
            , p(std::move(p))  //
        {
        }
        int i;
        std::unique_ptr<int> p;
    };

    Optional<S> o;
    o.Emplace(1, std::make_unique<int>(2));
    assert(o.HasValue());
    assert(o->i == 1);
    assert(*(o->p) == 2);

    o.Emplace(3, std::make_unique<int>(4));
    assert(o.HasValue());
    assert(o->i == 3);
    assert(*(o->p) == 4);
}

void TestRefQualifiedMethodOverloading() {
    {
        C::Reset();
        C val = *Optional<C>(C{});
        assert(C::copy_ctor == 0);
        assert(C::move_ctor == 2);
        assert(C::def_ctor == 1);
        assert(C::copy_assign == 0);
        assert(C::move_assign == 0);
    }
    {
        C::Reset();
        C val = Optional<C>(C{}).Value();
        assert(C::copy_ctor == 0);
        assert(C::move_ctor == 2);
        assert(C::def_ctor == 1);
        assert(C::copy_assign == 0);
        assert(C::move_assign == 0);
    }
    {
        C::Reset();
        Optional<C> opt(C{});
        (*opt).Update();
        assert(C::lvalue_call_count == 1);
        assert(C::rvalue_call_count == 0);
        (*std::move(opt)).Update();
        assert(C::lvalue_call_count == 1);
        assert(C::rvalue_call_count == 1);
    }
    {
        C::Reset();
        const Optional<C> opt(C{});
        (*opt).Update();
        assert(C::const_lvalue_call_count == 1);
    }
    {
        C::Reset();
        Optional<C> opt(C{});
        opt.Value().Update();
        assert(C::lvalue_call_count == 1);
        assert(C::rvalue_call_count == 0);
        std::move(opt).Value().Update();
        assert(C::lvalue_call_count == 1);
    }
    {
        C::Reset();
        const Optional<C> opt(C{});
        opt.Value().Update();
        assert(C::const_lvalue_call_count == 1);
    }
}

int main() {
    try {
        TestInitialization();
        TestAssignment();
        TestMoveAssignment();
        TestValueAccess();
        TestReset();
        TestEmplace();
        TestRefQualifiedMethodOverloading();
    } catch (...) {
        assert(false);
    }
}
