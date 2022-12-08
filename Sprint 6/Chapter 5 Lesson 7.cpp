#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Щупальце
class Tentacle {
public:
    explicit Tentacle(int id)
        : id_(id)
    {
    }

    int GetId() const {
        return id_;
    }

private:
    int id_ = 0;
};

// Осьминог
class Octopus {
public:
    Octopus() {
        Tentacle* t = nullptr;
        try {
            for (int i = 1; i <= 8; ++i) {
                t = new Tentacle(i);      // Может выбросить исключение bad_alloc
                tentacles_.push_back(t);  // Может выбросить исключение bad_alloc

                // Обнуляем указатель на щупальце, которое уже добавили в tentacles_,
                // чтобы не удалить его в обработчике catch повторно
                t = nullptr;
            }
        }
        catch (const bad_alloc&) {
            // Удаляем щупальца, которые успели попасть в контейнер tentacles_
            Cleanup();
            // Удаляем щупальце, которое создали, но не добавили в tentacles_
            delete t;
            // Конструктор не смог создать осьминога с восемью щупальцами,
            // поэтому выбрасываем исключение, чтобы сообщить вызывающему коду об ошибке
            // throw без параметров внутри catch выполняет ПЕРЕВЫБРОС пойманного исключения
            throw;
        }
    }

    const Tentacle& GetTentacle(int index) const {
        if (index < 0 || static_cast<size_t>(index) >= tentacles_.size()) {
            throw out_of_range("Invalid tentacle index"s);
        }
        // Чтобы превратить указатель в ссылку, разыменовываем его
        return *tentacles_[index];
    }

    ~Octopus() {
        // Осьминог владеет объектами в динамической памяти (щупальца),
        // которые должны быть удалены при его разрушении.
        // Деструктор - лучшее место, чтобы прибраться за собой.
        Cleanup();
    }

private:
    void Cleanup() {
        // Удаляем щупальца осьминога из динамической памяти
        for (Tentacle* t : tentacles_) {
            delete t;
        }
        // Очищаем массив указателей на щупальца
        tentacles_.clear();
    }

    // Вектор хранит указатели на щупальца. Сами объекты щупалец находятся в куче
    vector<Tentacle*> tentacles_;
};

// Напишите функцию main, в ней создайте осьминога и несколько его копий
int main() {
    Octopus octopus1;
    Octopus octopus2 = octopus1;
    Octopus octopus3 = octopus1;
    Octopus octopus4 = octopus1;
    
    // или циклом
    Octopus octopus[10];
    for (size_t i = 1; i < 10; ++i) {
        octopus[i] = octopus[0];
    }
}
