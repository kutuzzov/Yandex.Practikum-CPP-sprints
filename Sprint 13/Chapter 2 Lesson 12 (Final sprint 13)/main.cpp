#include "vector.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

    // "Магическое" число, используемое для отслеживания живости объекта
    inline const uint32_t DEFAULT_COOKIE = 0xdeadbeef;

    struct TestObj {
        TestObj() = default;
        TestObj(const TestObj& other) = default;
        TestObj& operator=(const TestObj& other) = default;
        TestObj(TestObj&& other) = default;
        TestObj& operator=(TestObj&& other) = default;
        ~TestObj() {
            cookie = 0;
        }
        [[nodiscard]] bool IsAlive() const noexcept {
            return cookie == DEFAULT_COOKIE;
        }
        uint32_t cookie = DEFAULT_COOKIE;
    };

    struct Obj {
        Obj() {
            if (default_construction_throw_countdown > 0) {
                if (--default_construction_throw_countdown == 0) {
                    throw std::runtime_error("Oops");
                }
            }
            ++num_default_constructed;
        }

        explicit Obj(int id)
            : id(id)  //
        {
            ++num_constructed_with_id;
        }

        Obj(int id, std::string name)
            : id(id)
            , name(std::move(name))  //
        {
            ++num_constructed_with_id_and_name;
        }

        Obj(const Obj& other)
            : id(other.id)  //
        {
            if (other.throw_on_copy) {
                throw std::runtime_error("Oops");
            }
            ++num_copied;
        }

        Obj(Obj&& other) noexcept
            : id(other.id)  //
        {
            ++num_moved;
        }

        Obj& operator=(const Obj& other) {
            if (this != &other) {
                id = other.id;
                name = other.name;
                ++num_assigned;
            }
            return *this;
        }

        Obj& operator=(Obj&& other) noexcept {
            id = other.id;
            name = std::move(other.name);
            ++num_move_assigned;
            return *this;
        }

        ~Obj() {
            ++num_destroyed;
            id = 0;
        }

        static int GetAliveObjectCount() {
            return num_default_constructed + num_copied + num_moved + num_constructed_with_id
                + num_constructed_with_id_and_name - num_destroyed;
        }

        static void ResetCounters() {
            default_construction_throw_countdown = 0;
            num_default_constructed = 0;
            num_copied = 0;
            num_moved = 0;
            num_destroyed = 0;
            num_constructed_with_id = 0;
            num_constructed_with_id_and_name = 0;
            num_assigned = 0;
            num_move_assigned = 0;
        }

        bool throw_on_copy = false;
        int id = 0;
        std::string name;

        static inline int default_construction_throw_countdown = 0;
        static inline int num_default_constructed = 0;
        static inline int num_constructed_with_id = 0;
        static inline int num_constructed_with_id_and_name = 0;
        static inline int num_copied = 0;
        static inline int num_moved = 0;
        static inline int num_destroyed = 0;
        static inline int num_assigned = 0;
        static inline int num_move_assigned = 0;
    };

}  // namespace

void Test1() {
    Obj::ResetCounters();
    const size_t SIZE = 100500;
    const size_t INDEX = 10;
    const int MAGIC = 42;
    {
        Vector<int> v;
        assert(v.Capacity() == 0);
        assert(v.Size() == 0);

        v.Reserve(SIZE);
        assert(v.Capacity() == SIZE);
        assert(v.Size() == 0);
    }
    {
        Vector<int> v(SIZE);
        const auto& cv(v);
        assert(v.Capacity() == SIZE);
        assert(v.Size() == SIZE);
        assert(v[0] == 0);
        assert(&v[0] == &cv[0]);
        v[INDEX] = MAGIC;
        assert(v[INDEX] == MAGIC);
        assert(&v[100] - &v[0] == 100);

        v.Reserve(SIZE * 2);
        assert(v.Size() == SIZE);
        assert(v.Capacity() == SIZE * 2);
        assert(v[INDEX] == MAGIC);
    }
    {
        Vector<int> v(SIZE);
        v[INDEX] = MAGIC;
        const auto v_copy(v);
        assert(&v[INDEX] != &v_copy[INDEX]);
        assert(v[INDEX] == v_copy[INDEX]);
    }
    {
        Vector<Obj> v;
        v.Reserve(SIZE);
        assert(Obj::GetAliveObjectCount() == 0);
    }
    {
        Vector<Obj> v(SIZE);
        assert(Obj::GetAliveObjectCount() == SIZE);
        const int old_copy_count = Obj::num_copied;
        const int old_move_count = Obj::num_moved;
        v.Reserve(SIZE * 2);
        assert(Obj::GetAliveObjectCount() == SIZE);
        assert(Obj::num_copied == old_copy_count);
        assert(Obj::num_moved == old_move_count + static_cast<int>(SIZE));
    }
    assert(Obj::GetAliveObjectCount() == 0);
}

void Test2() {
    const size_t SIZE = 100;
    Obj::ResetCounters();
    {
        Obj::default_construction_throw_countdown = SIZE / 2;
        try {
            Vector<Obj> v(SIZE);
            assert(false && "Exception is expected");
        }
        catch (const std::runtime_error&) {
        }
        catch (...) {
            // Unexpected error
            assert(false && "Unexpected exception");
        }
        assert(Obj::num_default_constructed == SIZE / 2 - 1);
        assert(Obj::GetAliveObjectCount() == 0);
    }
    Obj::ResetCounters();
    {
        Vector<Obj> v(SIZE);
        try {
            v[SIZE / 2].throw_on_copy = true;
            Vector<Obj> v_copy(v);
            assert(false && "Exception is expected");
        }
        catch (const std::runtime_error&) {
            assert(Obj::num_copied == SIZE / 2);
        }
        catch (...) {
            // Unexpected error
            assert(false && "Unexpected exception");
        }
        assert(Obj::GetAliveObjectCount() == SIZE);
    }
    Obj::ResetCounters();
    {
        Vector<Obj> v(SIZE);
        try {
            v[SIZE - 1].throw_on_copy = true;
            v.Reserve(SIZE * 2);
        }
        catch (...) {
            // Unexpected error
            assert(false && "Unexpected exception");
        }
        assert(v.Capacity() == SIZE * 2);
        assert(v.Size() == SIZE);
        assert(Obj::GetAliveObjectCount() == SIZE);
    }
}

void Test3() {
    const size_t MEDIUM_SIZE = 100;
    const size_t LARGE_SIZE = 250;
    const int ID = 42;
    {
        Obj::ResetCounters();
        Vector<int> v(MEDIUM_SIZE);
        {
            auto v_copy(std::move(v));

            assert(v_copy.Size() == MEDIUM_SIZE);
            assert(v_copy.Capacity() == MEDIUM_SIZE);
        }
        assert(Obj::GetAliveObjectCount() == 0);
    }
    {
        Obj::ResetCounters();
        {
            Vector<Obj> v(MEDIUM_SIZE);
            v[MEDIUM_SIZE / 2].id = ID;
            assert(Obj::num_default_constructed == MEDIUM_SIZE);
            Vector<Obj> moved_from_v(std::move(v));
            assert(moved_from_v.Size() == MEDIUM_SIZE);
            assert(moved_from_v[MEDIUM_SIZE / 2].id == ID);
        }
        assert(Obj::GetAliveObjectCount() == 0);

        assert(Obj::num_moved == 0);
        assert(Obj::num_copied == 0);
        assert(Obj::num_default_constructed == MEDIUM_SIZE);
    }
    {
        Obj::ResetCounters();
        Vector<Obj> v_medium(MEDIUM_SIZE);
        v_medium[MEDIUM_SIZE / 2].id = ID;
        Vector<Obj> v_large(LARGE_SIZE);
        v_large = v_medium;
        assert(v_large.Size() == MEDIUM_SIZE);
        assert(v_large.Capacity() == LARGE_SIZE);
        assert(v_large[MEDIUM_SIZE / 2].id == ID);
        assert(Obj::GetAliveObjectCount() == MEDIUM_SIZE + MEDIUM_SIZE);
    }
    {
        Obj::ResetCounters();
        Vector<Obj> v(MEDIUM_SIZE);
        {
            Vector<Obj> v_large(LARGE_SIZE);
            v_large[LARGE_SIZE - 1].id = ID;
            v = v_large;
            assert(v.Size() == LARGE_SIZE);
            assert(v_large.Capacity() == LARGE_SIZE);
            assert(v_large[LARGE_SIZE - 1].id == ID);
            assert(Obj::GetAliveObjectCount() == LARGE_SIZE + LARGE_SIZE);
        }
        assert(Obj::GetAliveObjectCount() == LARGE_SIZE);
    }
    assert(Obj::GetAliveObjectCount() == 0);
    {
        Obj::ResetCounters();
        Vector<Obj> v(MEDIUM_SIZE);
        v[MEDIUM_SIZE - 1].id = ID;
        Vector<Obj> v_small(MEDIUM_SIZE / 2);
        v_small.Reserve(MEDIUM_SIZE + 1);
        const size_t num_copies = Obj::num_copied;
        v_small = v;
        assert(v_small.Size() == v.Size());
        assert(v_small.Capacity() == MEDIUM_SIZE + 1);
        v_small[MEDIUM_SIZE - 1].id = ID;
        assert(Obj::num_copied - num_copies == MEDIUM_SIZE - (MEDIUM_SIZE / 2));
    }
}

void Test4() {
    const size_t ID = 42;
    const size_t SIZE = 100'500;
    {
        Obj::ResetCounters();
        Vector<Obj> v;
        v.Resize(SIZE);
        assert(v.Size() == SIZE);
        assert(v.Capacity() == SIZE);
        assert(Obj::num_default_constructed == SIZE);
    }
    assert(Obj::GetAliveObjectCount() == 0);

    {
        const size_t NEW_SIZE = 10'000;
        Obj::ResetCounters();
        Vector<Obj> v(SIZE);
        v.Resize(NEW_SIZE);
        assert(v.Size() == NEW_SIZE);
        assert(v.Capacity() == SIZE);
        assert(Obj::num_destroyed == SIZE - NEW_SIZE);
    }
    assert(Obj::GetAliveObjectCount() == 0);
    {
        Obj::ResetCounters();
        Vector<Obj> v(SIZE);
        Obj o{ ID };
        v.PushBack(o);
        assert(v.Size() == SIZE + 1);
        assert(v.Capacity() == SIZE * 2);
        assert(v[SIZE].id == ID);
        assert(Obj::num_default_constructed == SIZE);
        assert(Obj::num_copied == 1);
        assert(Obj::num_constructed_with_id == 1);
        assert(Obj::num_moved == SIZE);
    }
    assert(Obj::GetAliveObjectCount() == 0);
    {
        Obj::ResetCounters();
        Vector<Obj> v(SIZE);
        v.PushBack(Obj{ ID });
        assert(v.Size() == SIZE + 1);
        assert(v.Capacity() == SIZE * 2);
        assert(v[SIZE].id == ID);
        assert(Obj::num_default_constructed == SIZE);
        assert(Obj::num_copied == 0);
        assert(Obj::num_constructed_with_id == 1);
        assert(Obj::num_moved == SIZE + 1);
    }
    {
        Obj::ResetCounters();
        Vector<Obj> v;
        v.PushBack(Obj{ ID });
        v.PopBack();
        assert(v.Size() == 0);
        assert(v.Capacity() == 1);
        assert(Obj::GetAliveObjectCount() == 0);
    }

    {
        Vector<TestObj> v(1);
        assert(v.Size() == v.Capacity());
        // Операция PushBack существующего элемента вектора должна быть безопасна
        // даже при реаллокации памяти
        v.PushBack(v[0]);
        assert(v[0].IsAlive());
        assert(v[1].IsAlive());
    }
    {
        Vector<TestObj> v(1);
        assert(v.Size() == v.Capacity());
        // Операция PushBack для перемещения существующего элемента вектора должна быть безопасна
        // даже при реаллокации памяти
        v.PushBack(std::move(v[0]));
        assert(v[0].IsAlive());
        assert(v[1].IsAlive());
    }
}

void Test5() {
    const int ID = 42;
    using namespace std::literals;
    {
        Obj::ResetCounters();
        Vector<Obj> v;
        auto& elem = v.EmplaceBack(ID, "Ivan"s);
        assert(v.Capacity() == 1);
        assert(v.Size() == 1);
        assert(&elem == &v[0]);
        assert(v[0].id == ID);
        assert(v[0].name == "Ivan"s);
        assert(Obj::num_constructed_with_id_and_name == 1);
        assert(Obj::GetAliveObjectCount() == 1);
    }
    assert(Obj::GetAliveObjectCount() == 0);
    {
        Vector<TestObj> v(1);
        assert(v.Size() == v.Capacity());
        // Операция EmplaceBack существующего элемента вектора должна быть безопасна
        // даже при реаллокации памяти
        v.EmplaceBack(v[0]);
        assert(v[0].IsAlive());
        assert(v[1].IsAlive());
    }
}

void Test6() {
    using namespace std::literals;
    const size_t SIZE = 10;
    const int ID = 42;
    {
        Obj::ResetCounters();
        Vector<int> v(SIZE);
        const auto& cv(v);
        v.PushBack(1);
        assert(&*v.begin() == &v[0]);
        *v.begin() = 2;
        assert(v[0] == 2);
        assert(v.end() - v.begin() == static_cast<std::ptrdiff_t>(v.Size()));
        assert(v.begin() == cv.begin());
        assert(v.end() == cv.end());
        assert(v.cbegin() == cv.begin());
        assert(v.cend() == cv.end());
    }
    {
        Obj::ResetCounters();
        Vector<Obj> v{SIZE};
        Obj obj{ 1 };
        Vector<Obj>::iterator pos = v.Insert(v.cbegin() + 1, obj);
        assert(v.Size() == SIZE + 1);
        assert(v.Capacity() == SIZE * 2);
        assert(&*pos == &v[1]);
        assert(v[1].id == obj.id);
        assert(Obj::num_copied == 1);
        assert(Obj::num_default_constructed == SIZE);
        assert(Obj::GetAliveObjectCount() == SIZE + 2);
    }
    {
        Obj::ResetCounters();
        Vector<Obj> v;
        auto* pos = v.Emplace(v.end(), Obj{ 1 });
        assert(v.Size() == 1);
        assert(v.Capacity() >= v.Size());
        assert(&*pos == &v[0]);
        assert(Obj::num_moved == 1);
        assert(Obj::num_constructed_with_id == 1);
        assert(Obj::num_copied == 0);
        assert(Obj::num_assigned == 0);
        assert(Obj::num_move_assigned == 0);
        assert(Obj::GetAliveObjectCount() == 1);
    }
    {
        Obj::ResetCounters();
        Vector<Obj> v;
        v.Reserve(SIZE);
        auto* pos = v.Emplace(v.end(), Obj{ 1 });
        assert(v.Size() == 1);
        assert(v.Capacity() >= v.Size());
        assert(&*pos == &v[0]);
        assert(Obj::num_moved == 1);
        assert(Obj::num_constructed_with_id == 1);
        assert(Obj::num_copied == 0);
        assert(Obj::num_assigned == 0);
        assert(Obj::num_move_assigned == 0);
        assert(Obj::GetAliveObjectCount() == 1);
    }
    {
        Obj::ResetCounters();
        Vector<Obj> v{SIZE};
        Vector<Obj>::iterator pos = v.Insert(v.cbegin() + 1, Obj{ 1 });
        assert(v.Size() == SIZE + 1);
        assert(v.Capacity() == SIZE * 2);
        assert(&*pos == &v[1]);
        assert(v[1].id == 1);
        assert(Obj::num_copied == 0);
        assert(Obj::num_default_constructed == SIZE);
        assert(Obj::GetAliveObjectCount() == SIZE + 1);
    }
    {
        Vector<TestObj> v{SIZE};
        v.Insert(v.cbegin() + 2, v[0]);
        assert(std::all_of(v.begin(), v.end(), [](const TestObj& obj) {
            return obj.IsAlive();
            }));
    }
    {
        Vector<TestObj> v{SIZE};
        v.Insert(v.cbegin() + 2, std::move(v[0]));
        assert(std::all_of(v.begin(), v.end(), [](const TestObj& obj) {
            return obj.IsAlive();
            }));
    }
    {
        Vector<TestObj> v{SIZE};
        v.Emplace(v.cbegin() + 2, std::move(v[0]));
        assert(std::all_of(v.begin(), v.end(), [](const TestObj& obj) {
            return obj.IsAlive();
            }));
    }
    {
        Obj::ResetCounters();
        Vector<Obj> v{SIZE};
        auto* pos = v.Emplace(v.cbegin() + 1, ID, "Ivan"s);
        assert(v.Size() == SIZE + 1);
        assert(v.Capacity() == SIZE * 2);
        assert(&*pos == &v[1]);
        assert(v[1].id == ID);
        assert(v[1].name == "Ivan"s);
        assert(Obj::num_copied == 0);
        assert(Obj::num_default_constructed == SIZE);
        assert(Obj::num_moved == SIZE);
        assert(Obj::num_move_assigned == 0);
        assert(Obj::num_assigned == 0);
        assert(Obj::GetAliveObjectCount() == SIZE + 1);
    }
    {
        Obj::ResetCounters();
        Vector<Obj> v{SIZE};
        auto* pos = v.Emplace(v.cbegin() + v.Size(), ID, "Ivan"s);
        assert(v.Size() == SIZE + 1);
        assert(v.Capacity() == SIZE * 2);
        assert(&*pos == &v[SIZE]);
        assert(v[SIZE].id == ID);
        assert(v[SIZE].name == "Ivan"s);
        assert(Obj::num_copied == 0);
        assert(Obj::num_default_constructed == SIZE);
        assert(Obj::num_moved == SIZE);
        assert(Obj::num_move_assigned == 0);
        assert(Obj::num_assigned == 0);
        assert(Obj::GetAliveObjectCount() == SIZE + 1);
    }
    {
        Obj::ResetCounters();
        Vector<Obj> v{SIZE};
        v.Reserve(SIZE * 2);
        const int old_num_moved = Obj::num_moved;
        assert(v.Capacity() == SIZE * 2);
        auto* pos = v.Emplace(v.cbegin() + 3, ID, "Ivan"s);
        assert(v.Size() == SIZE + 1);
        assert(&*pos == &v[3]);
        assert(v[3].id == ID);
        assert(v[3].name == "Ivan");
        assert(Obj::num_copied == 0);
        assert(Obj::num_default_constructed == SIZE);
        assert(Obj::num_constructed_with_id_and_name == 1);
        assert(Obj::num_moved == old_num_moved + 1);
        assert(Obj::num_move_assigned == SIZE - 3);
        assert(Obj::num_assigned == 0);
    }
    {
        Obj::ResetCounters();
        Vector<Obj> v{SIZE};
        v[2].id = ID;
        auto* pos = v.Erase(v.cbegin() + 1);
        assert((pos - v.begin()) == 1);
        assert(v.Size() == SIZE - 1);
        assert(v.Capacity() == SIZE);
        assert(pos->id == ID);
        assert(Obj::num_copied == 0);
        assert(Obj::num_assigned == 0);
        assert(Obj::num_move_assigned == SIZE - 2);
        assert(Obj::num_moved == 0);
        assert(Obj::GetAliveObjectCount() == SIZE - 1);
    }
}

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

    static void Reset() {
        def_ctor = 0;
        copy_ctor = 0;
        move_ctor = 0;
        copy_assign = 0;
        move_assign = 0;
        dtor = 0;
    }

    inline static size_t def_ctor = 0;
    inline static size_t copy_ctor = 0;
    inline static size_t move_ctor = 0;
    inline static size_t copy_assign = 0;
    inline static size_t move_assign = 0;
    inline static size_t dtor = 0;
};

void Dump() {
    using namespace std;
    cerr << "Def ctors: "sv << C::def_ctor              //
        << ", Copy ctors: "sv << C::copy_ctor          //
        << ", Move ctors: "sv << C::move_ctor          //
        << ", Copy assignments: "sv << C::copy_assign  //
        << ", Move assignments: "sv << C::move_assign  //
        << ", Dtors: "sv << C::dtor << endl;
}

void Benchmark() {
    using namespace std;
    try {
        const size_t NUM = 10;
        C c;
        {
            cerr << "std::vector:"sv << endl;
            C::Reset();
            vector<C> v(NUM);
            Dump();
            v.push_back(c);
        }
        Dump();
    }
    catch (...) {
    }
    try {
        const size_t NUM = 10;
        C c;
        {
            cerr << "Vector:"sv << endl;
            C::Reset();
            Vector<C> v(NUM);
            Dump();
            v.PushBack(c);
        }
        Dump();
    }
    catch (...) {
    }
}

int main() {
    try {
        Test1();
        Test2();
        Test3();
        Test4();
        Test5();
        Test6();
        Benchmark();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}