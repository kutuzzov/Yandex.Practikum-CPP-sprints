#pragma once

#include <stdexcept> // содержит std::logic_error

template <typename T>
class ScopedPtr {
public:
    ScopedPtr() = default;

    explicit ScopedPtr(T* raw_ptr) noexcept
        : ptr_(raw_ptr) {
    }

    // «апрещаем копирование указател€
    ScopedPtr(const ScopedPtr&) = delete;

    ~ScopedPtr() {
        delete ptr_;
    }

    T* GetRawPtr() const noexcept {
        return ptr_;
    }

    T* Release() noexcept {
        T* p = ptr_;
        ptr_ = nullptr;
        return p;
    }

    explicit operator bool() const {
        return ptr_ != nullptr;
    }

    T* operator->() const {
        using namespace std::literals;
        if (!ptr_) {
            throw std::logic_error("Scoped ptr is null"s);
        }
        return ptr_;
    }

    T& operator*() const {
        using namespace std::literals;
        if (!ptr_) {
            throw std::logic_error("Scoped ptr is null"s);
        }
        return *ptr_;
    }

private:
    T* ptr_ = nullptr;
};