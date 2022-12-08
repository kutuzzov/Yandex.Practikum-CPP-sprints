#include <cassert>

// Умный указатель, удаляющий связанный объект при своём разрушении.
// Параметр шаблона T задаёт тип объекта, на который ссылается указатель
template <typename T>
class ScopedPtr {
public:
    // Конструктор по умолчанию создаёт нулевой указатель,
    // так как поле ptr_ имеет значение по умолчанию nullptr
    ScopedPtr() = default;

    // Создаёт указатель, ссылающийся на переданный raw_ptr.
    // raw_ptr ссылается либо на объект, созданный в куче при помощи new,
    // либо является нулевым указателем
    // Спецификатор noexcept обозначает, что метод не бросает исключений
    explicit ScopedPtr(T* raw_ptr) noexcept
    : ptr_(raw_ptr)
    {
    }

    // Удаляем у класса конструктор копирования
    ScopedPtr(const ScopedPtr&) = delete;

    // Деструктор. Удаляет объект, на который ссылается умный указатель.
    ~ScopedPtr() {
        // Реализуйте тело деструктора самостоятельно
        delete ptr_;
    }

    // Возвращает указатель, хранящийся внутри ScopedPtr
    T* GetRawPtr() const noexcept {
        // Напишите код метода самостоятельно
        return ptr_;
    }

    // Прекращает владение объектом, на который ссылается умный указатель.
    // Возвращает прежнее значение "сырого" указателя и устанавливает поле ptr_ в null
    T* Release() noexcept {
        // Реализуйте самостоятельно
        T* other_ptr = ptr_;
        ptr_ = nullptr;
        return other_ptr;
    }

private:
    T* ptr_ = nullptr;
};

// Этот main тестирует класс ScopedPtr
int main() {
    // Вспомогательный "шпион", позволяющий узнать о своём удалении
    struct DeletionSpy {
        explicit DeletionSpy(bool& is_deleted)
            : is_deleted_(is_deleted) {
        }
        ~DeletionSpy() {
            is_deleted_ = true;
        }
        bool& is_deleted_;
    };

    // Проверяем автоматическое удаление
    {
        bool is_deleted = false;
        {
            // настраиваем "шпион", чтобы при своём удалении он выставил is_deleted в true
            DeletionSpy* raw_ptr = new DeletionSpy(is_deleted);
            ScopedPtr<DeletionSpy> p(raw_ptr);
            assert(p.GetRawPtr() == raw_ptr);
            assert(!is_deleted);
            // При выходе из блока деструктор p должен удалить "шпиона"
        }
        // Если деструктор умного указателя работает правильно, шпион перед своей "смертью"
        // должен выставить is_deleted в true
        assert(is_deleted);
    }

    // Проверяем работу метода Release
    {
        bool is_deleted = false;
        DeletionSpy* raw_ptr = new DeletionSpy(is_deleted);
        {
            ScopedPtr<DeletionSpy> scoped_ptr(raw_ptr);
            assert(scoped_ptr.Release() == raw_ptr);
            assert(scoped_ptr.GetRawPtr() == nullptr);
            // После Release умный указатель не ссылается на объект и не удаляет его при своём удалении
        }
        assert(!is_deleted);
        delete raw_ptr;
        assert(is_deleted);
    }
}
