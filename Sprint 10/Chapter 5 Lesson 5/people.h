#pragma once

#include <string>

class Person;

// ����������� �� ���������� ��������.
class PersonObserver {
public:
    // ���� ����� ����������, ����� �������� ��������� ���������������� ��������
    virtual void OnSatisfactionChanged(Person& /*person*/, int /*old_value*/, int /*new_value*/) {
        // ���������� ������ �������� ������ ������ �� ������
    }

protected:
    // ����� PersonObserver �� ������������ ��� �������� ��������
    ~PersonObserver() = default;
};

/*
    �������.
    ��� ��������� ������ ���������������� ����������
    ���������� � ��� �����������
*/
class Person {
public:
    Person(const std::string& name, int age) 
        : name_(name), age_(age)
    {
    }

    virtual ~Person() = default;

    int GetSatisfaction() const { return satisfaction_; }
    const std::string& GetName() const { return name_; }

    // ������������ ����������� � ��������. ����������� �����������
    // ������������ �� ��������� ������ ���������������� ��������
    // ����� ����������� �������� ����� ����� ������������
    // ���� �������� nullptr � �������� �����������, ��� ������������ ���������� �����������
    void SetObserver(PersonObserver* observer) { observer_ = observer; }
    int GetAge() const { return age_; }

    // ����������� �� 1 ���������� ������� �� �����
    // ����������� ���������������� �� 1
    virtual void Dance() {
        SetSatisfaction(GetSatisfaction() + 1);
        ++dance_count_;
    }

    int GetDanceCount() const { return dance_count_; }

    // ������� ����. ���������� � ������� ������ ������ �� ������
    virtual void LiveADay() {
        // ��������� ����� �������������� ���� �����
    }

protected:
    void SetSatisfaction(int value) {
        if (satisfaction_ != value) {
            int old_satisfaction = satisfaction_;
            satisfaction_ = value;
            // ���� � �������� ���� �����������, �������� ��� �� ��������� ����������������
            if (observer_) {
                observer_->OnSatisfactionChanged(*this, old_satisfaction, satisfaction_);
            }
        }
    }

private:
    std::string name_;
    PersonObserver* observer_ = nullptr;
    int satisfaction_ = 100;
    int age_ = 0;
    int dance_count_ = 0;
};

// �������.
// ���� �������� �������� �� �������
class Worker : public Person {
public:
    Worker(const std::string& name, int age) 
        : Person(name, age)
    {
    }

    // ������� ������ 30 ��� � ������ 40 �� ����� �������� 2 ������� ���������������� ������ 1
    void Dance() override {
        if (GetAge() > 30 && GetAge() < 40) SetSatisfaction(GetSatisfaction() + 1);
        Person::Dance();
    }
    // ���� �������� �������� �� �������
    void LiveADay() override { Work(); }

    // ����������� ������� ��������� ������ �� 1, ��������� ���������������� �� 5
    void Work() {
        ++work_;
        SetSatisfaction(GetSatisfaction() - 5);
    }

    // ���������� �������� �������� ��������� ������
    int GetWorkDone() const { return work_; }

private:
    int work_ = 0;
};

// �������.
// ���� �������� �������� �� ������
class Student : public Person {
public:
    Student(const std::string& name, int age) 
        : Person(name, age)
    {
    }

    // ���� �������� �������� �� ������
    void LiveADay() override { Study(); }

    // ����� ����������� ������� ������ �� 1, ��������� ������� ���������������� �� 3
    void Study() {
        ++knowledge_;
        SetSatisfaction(GetSatisfaction() - 3);
    }

    // ���������� ������� ������
    int GetKnowledgeLevel() const { return knowledge_; }

private:
    int knowledge_ = 0;
};