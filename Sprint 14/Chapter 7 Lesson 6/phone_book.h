#pragma once

#include <contact.pb.h>

#include "iterator_range.h"

#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

struct Date {
    int year, month, day;
};

struct Contact {
    std::string name;
    std::optional<Date> birthday;
    std::vector<std::string> phones;
};

class PhoneBook {
public:
    explicit PhoneBook(std::vector<Contact> contacts);

    using Iterator = std::vector<Contact>::iterator;

    using ContactRange = IteratorRange<Iterator>;

    ContactRange FindByNamePrefix(std::string_view name_prefix) const;

    void SaveTo(std::ostream& output) const;
    
private:
    mutable std::vector<Contact> contacts_;
};

PhoneBook DeserializePhoneBook(std::istream& input);
