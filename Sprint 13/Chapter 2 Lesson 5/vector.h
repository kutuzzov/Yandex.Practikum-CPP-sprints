#pragma once
#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>

template <typename T>
class Vector {
public:
    Vector() = default;

    explicit Vector(size_t size)
        : data_(Allocate(size))
        , capacity_(size)
        , size_(size)  //
    {
        size_t i = 0;
        try {
            for (; i != size; ++i) {
                new (data_ + i) T();
            }
        } catch (...) {
            // В переменной i содержится количество созданных элементов.
            // Теперь их надо разрушить
            DestroyN(data_, i);
            // Освобождаем память, выделенную через Allocate
            Deallocate(data_);
            // Перевыбрасываем пойманное исключение, чтобы сообщить об ошибке создания объекта
            throw;
        }
    }
    
    Vector(const Vector& other)
        : data_(Allocate(other.size_))
        , capacity_(other.size_)
        , size_(other.size_)  //
    {
        size_t i = 0;
        try {
            for (; i != other.size_; ++i) {
                CopyConstruct(data_ + i, other.data_[i]);
            }
        } catch (...) {
            DestroyN(data_, i);
            Deallocate(data_);
            throw;
        }
    }
    
    ~Vector() {
        DestroyN(data_, size_);
        Deallocate(data_);
    }
    
    size_t Size() const noexcept {
        return size_;
    }

    size_t Capacity() const noexcept {
        return capacity_;
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<Vector&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < size_);
        return data_[index];
    }
    
    void Reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }
        T* new_data = Allocate(new_capacity);
        size_t i = 0;
        try {
            for (; i != size_; ++i) {
                CopyConstruct(new_data + i, data_[i]);
            }
        } catch (...) {
            DestroyN(new_data, i);
            Deallocate(new_data);
            throw;
        }
        DestroyN(data_, size_);
        Deallocate(data_);

        data_ = new_data;
        capacity_ = new_capacity;
    }

private:
    T* data_ = nullptr;
    size_t capacity_ = 0;
    size_t size_ = 0;
    
    // Выделяет сырую память под n элементов и возвращает указатель на неё
    static T* Allocate(size_t n) {
        return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
    }

    // Освобождает сырую память, выделенную ранее по адресу buf при помощи Allocate
    static void Deallocate(T* buf) noexcept {
        operator delete(buf);
    }
    
    // Вызывает деструкторы n объектов массива по адресу buf
    static void DestroyN(T* buf, size_t n) noexcept {
        for (size_t i = 0; i != n; ++i) {
            Destroy(buf + i);
        }
    }

    // Создаёт копию объекта elem в сырой памяти по адресу buf
    static void CopyConstruct(T* buf, const T& elem) {
        new (buf) T(elem);
    }

    // Вызывает деструктор объекта по адресу buf
    static void Destroy(T* buf) noexcept {
        buf->~T();
    }
};
