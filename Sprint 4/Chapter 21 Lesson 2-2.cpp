#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename Type>
class Stack {
public:
    void Push(const Type& element) {
        elements_.push_back(element);
    }
    void Pop() {
        elements_.pop_back();
    }
    const Type& Peek() const {
        return elements_.back();
    }
    Type& Peek() {
        return elements_.back();
    }
    void Print() const {
        PrintRange(elements_.begin(), elements_.end());
    }
    uint64_t Size() const {
        return elements_.size();
    }
    bool IsEmpty() const {
        if (elements_.size() == 0) return true;
        return false;
    }

private:
    vector<Type> elements_;
};

template <typename Type>
struct Minimum
{
    Type element;
    Type minimum;
};

template <typename Type>
ostream& operator<<(ostream& out, const Minimum<Type>& pair_elements) {
    out << pair_elements.element;
    return out;
}

template <typename Type>
class StackMin {
public:
    void Push(const Type& element) {
        Minimum<Type> pair_elements = { element, element };
        if (!elements_.IsEmpty() && pair_elements.minimum > elements_.Peek().minimum) {
            pair_elements.minimum = elements_.Peek().minimum;
        }
        elements_.Push(pair_elements);
    }
    void Pop() {
        elements_.Pop();
    }
    const Type& Peek() const {
        return elements_.Peek().element;
    }
    Type& Peek() {
        return elements_.Peek().element;
    }
    void Print() const {
        elements_.Print();
    }
    uint64_t Size() const {
        return elements_.size();
    }
    bool IsEmpty() const {
        return elements_.IsEmpty();
    }
    const Type& PeekMin() const {
        return elements_.Peek().minimum;
    }
    Type& PeekMin() {
        return elements_.Peek().minimum;
    }
private:
    Stack<Minimum<Type>> elements_;
};

int main() {
    setlocale(LC_ALL, "Russian");

    StackMin<int> stack;
    vector<int> values(5);

    // заполняем вектор для тестирования нашего стека
    iota(values.begin(), values.end(), 1);
    // перемешиваем значения
    random_shuffle(values.begin(), values.end()); //random_shuffle C++14 !!!

    // заполняем стек
    for (int i = 0; i < 5; ++i) {
        stack.Push(values[i]);
    }

    // печатаем стек и его минимум, постепенно убирая из стека элементы
    while (!stack.IsEmpty()) {
        stack.Print();
        cout << "Минимум = "s << stack.PeekMin() << endl;
        stack.Pop();
    }
}
