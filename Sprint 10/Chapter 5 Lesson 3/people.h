#pragma once
#include <string>
#include <vector>

enum class ProgrammingLanguage { CPP, JAVA, PYTHON, PHP };

enum class Gender { MALE, FEMALE };

class Person {
public:
    Person(const std::string& name, int age, Gender gender);
    
    const std::string& GetName() const;
    int GetAge() const;
    Gender GetGender() const;
    
private:
    std::string name_;
    int age_;
    Gender gender_;
};

// Программист. Знает несколько языков программирования
class Programmer : public Person {
public:
    Programmer(const std::string& name, int age, Gender gender);

    // Добавляет программисту знание языка программирования language
    // После вызова этого метода программист может программировать на этом языке
    // и на ранее добавленных
    void AddProgrammingLanguage(ProgrammingLanguage language);

    // Сообщает, может ли программист программировать на языке прогарммирования language
    // Изначально программист не владеет ни одним языком программирования
    bool CanProgram(ProgrammingLanguage language) const;
    
private:
    std::vector<ProgrammingLanguage> skills_;
};

enum class WorkerSpeciality { BLACKSMITH, CARPENTER, WOOD_CHOPPER, ENGINEER, PLUMBER };

// Рабочий. Владеет несколькими специальностями
class Worker : public Person {
public:
    Worker(const std::string& name, int age, Gender gender);

    // Добавляет рабочему способность работать по специальности speciality
    void AddSpeciality(WorkerSpeciality speciality);

    // Сообщает, может ли рабочий работать по указанной специальности.
    // Сразу после своего создания рабочий не владеет никакими специальностями
    bool HasSpeciality(WorkerSpeciality speciality) const;
    
private:
    std::vector<WorkerSpeciality> skills_;
};
