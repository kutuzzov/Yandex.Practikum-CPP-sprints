#include <cassert>
#include <cstddef>  // нужно для nullptr_t

using namespace std;

// Реализуйте шаблон класса UniquePtr
template <typename T>
class UniquePtr {
private:
    T* ptr_;
public:
    UniquePtr()
        : ptr_(nullptr)
    {}

    explicit UniquePtr(T* ptr) 
        : ptr_(ptr)
    {}

    UniquePtr(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) {
        if (this != &other) {
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
     }

    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr& operator=(nullptr_t) {
        delete ptr_;
        ptr_ = nullptr;
        return *this;
    }

    UniquePtr& operator=(UniquePtr&& other) {
        if (this == &other) return *this;
        delete ptr_;
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
        return *this;
    }

    ~UniquePtr() {
        delete ptr_;
        ptr_ = nullptr;
    }

    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const {
        return ptr_;
    }

    T* Release() {
        T* temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }

    void Reset(T* ptr) {
        delete ptr_;
        ptr_ = ptr;
    }

    void Swap(UniquePtr& other) {
        T* temp = other.ptr_;
        other.ptr_ = ptr_;
        ptr_ = temp;
    }

    T* Get() const {
        return ptr_;
    }
};

struct Item {
    static int counter;
    int value;
    Item(int v = 0)
        : value(v)
    {
        ++counter;
    }
    Item(const Item& other)
        : value(other.value)
    {
        ++counter;
    }
    ~Item() {
        --counter;
    }
};

int Item::counter = 0;

void TestLifetime() {
    Item::counter = 0;
    {
        UniquePtr<Item> ptr(new Item);
        assert(Item::counter == 1);

        ptr.Reset(new Item);
        assert(Item::counter == 1);
    }
    assert(Item::counter == 0);

    {
        UniquePtr<Item> ptr(new Item);
        assert(Item::counter == 1);

        auto rawPtr = ptr.Release();
        assert(Item::counter == 1);

        delete rawPtr;
        assert(Item::counter == 0);
    }
    assert(Item::counter == 0);
}

void TestGetters() {
    UniquePtr<Item> ptr(new Item(42));
    assert(ptr.Get()->value == 42);
    assert((*ptr).value == 42);
    assert(ptr->value == 42);
}

int main() {
    TestLifetime();
    TestGetters();
}
