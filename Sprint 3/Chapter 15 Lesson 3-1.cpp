#include <string>

using namespace std;

enum class AnimalType {
    Cat,
    Dog,
    Mouse,
};

class Animal {
public:
    Animal(AnimalType type, string name, string owner_name)
        : type_(type), name_(name), owner_name_(owner_name)
    {}
    
    AnimalType GetType() const {
        return type_;
    }

    const string& GetName() const {
        return name_;
    }

    const string& GetOwnerName() const {
        return owner_name_;
    }

private:
    AnimalType type_;
    string name_;
    string owner_name_;
};
