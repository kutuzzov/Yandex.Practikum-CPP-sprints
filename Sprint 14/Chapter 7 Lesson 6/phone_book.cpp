#include "phone_book.h"

using namespace std;

// Напишите тут реализации методов и функций, 
// декларированных в phone_book.h

PhoneBook::PhoneBook(vector<Contact> contacts)
    : contacts_(contacts)
{    
}

PhoneBook::ContactRange PhoneBook::FindByNamePrefix(string_view name_prefix) const {
    std::sort(contacts_.begin(), contacts_.end(),
              [](const Contact& lhs, const Contact& rhs) {
                    return lhs.name < rhs.name;
    });
    
    if (name_prefix.size() == 0) return { contacts_.begin(), contacts_.end() };
    
    auto lower_bound_comp = [](const Contact& contact, std::string_view prefix) {
        return contact.name < prefix;
    };

    auto upper_bound_comp = [](std::string_view prefix, const Contact& contact) {
        int x = contact.name.compare(0, prefix.size(), prefix);
        if (x == 0) return false;
        return prefix < contact.name;
    };
    
    Iterator from = std::lower_bound(contacts_.begin(), contacts_.end(), name_prefix, lower_bound_comp);
    Iterator to = std::upper_bound(from, contacts_.end(), name_prefix, upper_bound_comp);
        
    auto x = std::distance(from, to);
    return { from, to };
}

void PhoneBook::SaveTo(ostream& output) const {
    phone_book_serialize::ContactList pb_phone_book;
    for (auto contact : contacts_) {
        phone_book_serialize::Contact pb_contact;
        pb_contact.set_name(contact.name);
        if (contact.birthday.has_value()) {
            pb_contact.mutable_birthday()->set_day(contact.birthday->day);
            pb_contact.mutable_birthday()->set_month(contact.birthday->month);
            pb_contact.mutable_birthday()->set_year(contact.birthday->year);
        }
        for (auto phone : contact.phones) {
            pb_contact.add_phone_number(phone);
        }
        pb_phone_book.mutable_contact()->Add(std::move(pb_contact));
    }
    pb_phone_book.SerializeToOstream(&output);
}

PhoneBook DeserializePhoneBook(istream& input) {
    phone_book_serialize::ContactList pb_phone_book;
    if (pb_phone_book.ParseFromIstream(&input)) {
        vector<Contact> phone_book(pb_phone_book.contact_size());
        for (size_t i = 0; i < pb_phone_book.contact_size(); ++i) {
            auto pb_contact = pb_phone_book.contact(i);
            phone_book[i].name = pb_contact.name();
            if (pb_contact.has_birthday()) {
                Date birthday = { static_cast<int>(pb_contact.birthday().year()),
                                  static_cast<int>(pb_contact.birthday().month()),
                                  static_cast<int>(pb_contact.birthday().day()) };
                phone_book[i].birthday.emplace(birthday);
            }
            for (auto pb_phone : *pb_contact.mutable_phone_number()) {
                phone_book[i].phones.push_back(pb_phone);
            }
        }
        return PhoneBook(phone_book);
    }
}
