#include "vector.h"

namespace {

struct Obj {
    Obj() {
        ++num_default_constructed;
    }

    Obj(const Obj& /*other*/) {
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
        num_default_constructed = 0;
        num_copied = 0;
        num_moved = 0;
        num_destroyed = 0;
    }

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

int main() {
    Test1();
}
