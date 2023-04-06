#include "people.h"

#include <stdexcept>
#include <algorithm>

using namespace std;

Person::Person(const std::string& name, int age, Gender gender)
    : name_(name), age_(age), gender_(gender)
{
}

const string& Person::GetName() const {
    return name_;
}

int Person::GetAge() const {
    return age_;
}

Gender Person::GetGender() const {
    return gender_;
}

Programmer::Programmer(const string& name, int age, Gender gender)
    : Person::Person(name, age, gender)
{
}

void Programmer::AddProgrammingLanguage(ProgrammingLanguage language) {
    skills_.push_back(language);
}

bool Programmer::CanProgram(ProgrammingLanguage language) const {
    return std::count(skills_.begin(), skills_.end(), language);
}

Worker::Worker(const string& name, int age, Gender gender)
    : Person::Person(name, age, gender)
{
}

void Worker::AddSpeciality(WorkerSpeciality speciality) {
    skills_.push_back(speciality);
}

bool Worker::HasSpeciality(WorkerSpeciality speciality) const {
    return std::count(skills_.begin(), skills_.end(), speciality);
}
