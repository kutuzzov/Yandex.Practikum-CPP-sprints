#pragma once

#include "array_ptr.h"

#include <cassert>
#include <initializer_list>
#include <stdexcept>

// класс-обёртка для работы функции Reserve
class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity)
        : capacity_(capacity)
    {
    }

    size_t GetCapacity() {
        return capacity_;
    }

private:
    size_t capacity_;
};

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size)
        : SimpleVector(size, Type())
    {
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value)
        : items_(size), size_(size), capacity_(size)
    {
        std::fill(items_.Get(), items_.Get() + size, value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init)
        : items_(init.size()), size_(init.size()), capacity_(init.size())
    {
        std::copy(init.begin(), init.end(), items_.Get());
    }

    // конструктор копирования
    SimpleVector(const SimpleVector& other)
        : items_(other.capacity_), size_(other.size_)
    {
        std::copy(other.begin(), other.end(), items_.Get());
    }

    // конструктор перемещения
    SimpleVector(SimpleVector&& other)
        : items_(other.capacity_)
    {
        swap(other);
    }

    // Конструктор с вызовом функции Reserve
    explicit SimpleVector(ReserveProxyObj obj)
    {
        Reserve(obj.GetCapacity());
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (&items_ != &rhs.items_) {
            ArrayPtr<Type> temp(rhs.GetCapacity());
            std::copy(rhs.begin(), rhs.end(), temp.Get());
            items_.swap(temp);
            size_ = rhs.GetSize();
            capacity_ = rhs.GetCapacity();
        }
        return *this;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) throw std::out_of_range("Out of range");
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) throw std::out_of_range("Out of range");
        return items_[index];
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return begin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return end();
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // на замену std::fill для поддержки семантики перемещения метода Resize
    void Fill(Iterator first, Iterator last) {
        assert(first < last);

        for (; first != last; ++first) {
            *first = std::move(Type());
        }
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size <= size_) {
            size_ = new_size;
        }
        if (new_size <= capacity_) {
            Fill(items_.Get() + size_, items_.Get() + size_ + new_size);
        }
        if (new_size > capacity_) {
            size_t new_capacity = std::max(new_size, capacity_ * 2);
            ArrayPtr<Type> temp(new_capacity);
            Fill(temp.Get(), temp.Get() + new_capacity);
            std::move(items_.Get(), items_.Get() + capacity_, temp.Get());
            items_.swap(temp);

            size_ = new_size;
            capacity_ = new_capacity;
        }
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        if (size_ + 1 > capacity_) {
            size_t new_capacity = std::max(size_ + 1, capacity_ * 2);
            ArrayPtr<Type> temp(new_capacity);
            std::fill(temp.Get(), temp.Get() + new_capacity, Type());
            std::copy(items_.Get(), items_.Get() + size_, temp.Get());
            items_.swap(temp);
            capacity_ = new_capacity;
        }
        items_[size_] = item;
        ++size_;
    }

    // метод PushBack перемещением
    void PushBack(Type&& item) {
        if (size_ + 1 > capacity_) {
            size_t new_capacity = std::max(size_ + 1, capacity_ * 2);
            ArrayPtr<Type> temp(new_capacity);
            std::move(items_.Get(), items_.Get() + size_, temp.Get());
            items_.swap(temp);
            capacity_ = new_capacity;
        }
        items_[size_] = std::move(item);
        ++size_;
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos <= end());

        size_t count = pos - items_.Get();
        if (capacity_ == 0) {
            ArrayPtr<Type> temp(1);
            temp[count] = value;
            items_.swap(temp);
            ++capacity_;
        }
        else if (size_ < capacity_) {
            std::copy_backward(items_.Get() + count, items_.Get() + size_,
                items_.Get() + size_ + 1);
            items_[count] = value;
        }
        else {
            size_t new_capacity = std::max(size_ + 1, capacity_ * 2);
            ArrayPtr<Type> temp(capacity_);
            std::copy(items_.Get(), items_.Get() + size_,
                temp.Get());
            std::copy_backward(items_.Get() + count, items_.Get() + size_,
                temp.Get() + size_ + 1);
            temp[count] = value;
            items_.swap(temp);
            capacity_ = new_capacity;
        }
        ++size_;

        return &items_[count];
    }

    // метод Insert перемещением
    Iterator Insert(Iterator pos, Type&& value) {
        assert(pos >= begin() && pos <= end());

        size_t count = pos - items_.Get();
        if (capacity_ == 0) {
            ArrayPtr<Type> temp(1);
            temp[count] = std::move(value);
            items_.swap(temp);
            ++capacity_;
        }
        else if (size_ < capacity_) {
            std::move_backward(items_.Get() + count, items_.Get() + size_,
                items_.Get() + size_ + 1);
            items_[count] = std::move(value);
        }
        else {
            size_t new_capacity = std::max(size_ + 1, capacity_ * 2);
            ArrayPtr<Type> temp(capacity_);
            std::move(items_.Get(), items_.Get() + size_,
                temp.Get());
            std::move_backward(items_.Get() + count, items_.Get() + size_,
                temp.Get() + size_ + 1);
            temp[count] = std::move(value);
            items_.swap(temp);
            capacity_ = new_capacity;
        }
        ++size_;

        return &items_[count];
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (items_) --size_;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        assert(pos != this->end());

        size_t count = pos - items_.Get();
        std::move(items_.Get() + count + 1, items_.Get() + size_, items_.Get() + count);
        --size_;

        return &items_[count];
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        std::swap(capacity_, other.capacity_);
        std::swap(size_, other.size_);
        items_.swap(other.items_);
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> temp(new_capacity);
            std::fill(temp.Get(), temp.Get() + new_capacity, Type());
            std::copy(items_.Get(), items_.Get() + size_, temp.Get());
            items_.swap(temp);
            capacity_ = new_capacity;
        }
    }

private:
    ArrayPtr<Type> items_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(),
                      rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !std::equal(lhs.begin(), lhs.end(),
                       rhs.begin());
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                        rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs < rhs || lhs == rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs <= rhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}
