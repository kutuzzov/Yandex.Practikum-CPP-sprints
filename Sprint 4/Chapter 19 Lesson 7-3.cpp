#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

class Tower {
public:
    // конструктор и метод SetDisks нужны, чтобы правильно создать башни
    Tower(int disks_num) {
        FillTower(disks_num);
    }

    int GetDisksNum() const {
        return static_cast<int>(disks_.size());
    }

    vector<int> GetDisks() const {
        return disks_;
    }

    void SetDisks(int disks_num) {
        FillTower(disks_num);
    }

    // добавляем диск на верх собственной башни
    // обратите внимание на исключение, которое выбрасывается этим методом
    void AddToTop(int disk) {
        int top_disk_num = static_cast<int>(disks_.size()) - 1;
        if (0 != disks_.size() && disk >= disks_[top_disk_num]) {
            throw invalid_argument("Невозможно поместить большой диск на маленький");
        }
        else {
            // допишите этот метод и используйте его в вашем решении
            disks_.push_back(disk);
        }
    }

    // вы можете дописывать необходимые для вашего решения методы

    // disks_num - количество перемещаемых дисков
    // destination - конечная башня для перемещения
    // buffer - башня, которую нужно использовать в качестве буфера для дисков
    void MoveDisks(int disks_num, Tower& destination, Tower& buffer) {
        if (disks_num > 0) {
            // действия из шага рекурсии
            MoveDisks(disks_num - 1, buffer, destination);

            int top_disk_num = static_cast<int>(disks_.size()) - 1;
            try {
                destination.AddToTop(disks_[top_disk_num]);
            }
            catch (const invalid_argument& e) {
                cout << e.what() << '\n';
                throw;
            }
            disks_.pop_back();

            buffer.MoveDisks(disks_num - 1, destination, *this);
        }
    }

private:
    vector<int> disks_;

    // используем приватный метод FillTower, чтобы избежать дубликации кода
    void FillTower(int disks_num) {
        for (int i = disks_num; i > 0; i--) {
            disks_.push_back(i);
        }
    }
};

void SolveHanoi(vector<Tower>& towers) {
    int disks_num = towers[0].GetDisksNum();
    // запускаем рекурсию
    // просим переложить все диски на последнюю башню
    // с использованием средней башни как буфера
    towers[0].MoveDisks(disks_num, towers[2], towers[1]);
}

void Print(vector<Tower>& towers) {
    for (const auto& tower : towers) {
        cout << "Башня " << &tower - &(*begin(towers)) + 1 << ": "s;
        for (const auto& disk : tower.GetDisks()) {
            cout << disk << " "s;
        }
        cout << endl;
    }
    cout << endl;
}

int main() {
    setlocale(LC_ALL, "Russian");

    int towers_num = 3;
    int disks_num = 3;
    vector<Tower> towers;
    // добавим в вектор три пустые башни
    for (int i = 0; i < towers_num; ++i) {
        towers.push_back(0);
    }
    // добавим на первую башню три кольца
    towers[0].SetDisks(disks_num);

    cout << "Вектор башен до перемещения:"s << endl;
    Print(towers);

    SolveHanoi(towers);

    cout << "Вектор башен после перемещения:"s << endl;
    Print(towers);
}
