#include "vector.h"

#include <stdexcept>

namespace {

struct Obj {
    Obj() {
        if (default_construction_throw_countdown > 0) {
            if (--default_construction_throw_countdown == 0) {
                throw std::runtime_error("Oops");
            }
        }
        ++num_default_constructed;
    }

    Obj(const Obj& other) {
        if (other.throw_on_copy) {
            throw std::runtime_error("Oops");
        }
        ++num_copied;
    }

    Obj(Obj&& /*other*/) noexcept {
        ++num_moved;
    }

    Obj& operator=(const Obj& other) = default;
    Obj& operator=(Obj&& other) = default;

    ~Obj() {
        ++num_destroyed;
    }

    static int GetAliveObjectCount() {
        return num_default_constructed + num_copied + num_moved - num_destroyed;
    }

    static void ResetCounters() {
        default_construction_throw_countdown = 0;
        num_default_constructed = 0;
        num_copied = 0;
        num_moved = 0;
        num_destroyed = 0;
    }

    bool throw_on_copy = false;

    static inline int default_construction_throw_countdown = 0;
    static inline int num_default_constructed = 0;
    static inline int num_copied = 0;
    static inline int num_moved = 0;
    static inline int num_destroyed = 0;
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
        v.Reserve(SIZE * 2);
        assert(Obj::GetAliveObjectCount() == SIZE);
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
        } catch (const std::runtime_error&) {
        } catch (...) {
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
        } catch (const std::runtime_error&) {
            assert(Obj::num_copied == SIZE / 2);
        } catch (...) {
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
            assert(false && "Exception is expected");
        } catch (const std::runtime_error&) {
            assert(Obj::num_copied == SIZE - 1);
        } catch (...) {
            // Unexpected error
            assert(false && "Unexpected exception");
        }
        assert(v.Capacity() == SIZE);
        assert(v.Size() == SIZE);
        assert(Obj::GetAliveObjectCount() == SIZE);
    }
}

int main() {
    Test1();
    Test2();
}
