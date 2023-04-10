#pragma once

#include "people.h"

/*
    Надзиратель за уровнем удовлетворённости.
    Способен наблюдать за состоянием человека
    Если уровень удовлетворённости человека опустится ниже минимального уровня, Надзиратель
    побуждает человека танцевать до тех пор, пока уровень уровень удовлетворённости
    не станет больше или равен максимальному значению
*/
class SatisfactionSupervisor : public PersonObserver {
public:
    // Конструктор принимает значение нижнего и верхнего уровня удовлетворённости
    SatisfactionSupervisor(int min_satisfaction, int max_satisfaction) 
        : min_satisfaction_(min_satisfaction), max_satisfaction_(max_satisfaction)
    {
    }

    void OnSatisfactionChanged(Person& person, int old_value, int new_value) override {
        if (new_value < old_value) {
            if (new_value < min_satisfaction_) {
                while (new_value < max_satisfaction_) {
                    person.Dance();
                    new_value = person.GetSatisfaction();
                }
            }
        }
    }

private:
    int min_satisfaction_;
    int max_satisfaction_;
};
