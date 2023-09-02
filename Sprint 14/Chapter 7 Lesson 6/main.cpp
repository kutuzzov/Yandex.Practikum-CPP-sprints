#include "phone_book.h"

#include <cassert>
#include <sstream>
#include <string_view>

#include <contact.pb.h>

using namespace std;

void TestSerialization() {
    const PhoneBook ab({
        {"Ivan Ivanov"s, Date{1980, 1, 13}, {"+79850685521"s}},
        {"Margarita Petrova"s, Date{1989, 4, 23}, {"+79998887766"s, "+71112223344"s}},
        {"Just Birthday"s, Date{1989, 4, 23}, {}},
        {"No Birthday"s, std::nullopt, {"+7-4862-77-25-64"s}},
    });

    ostringstream output(std::ios::binary);
    ab.SaveTo(output);

    istringstream input(output.str(), std::ios::binary);
    phone_book_serialize::ContactList list;
    assert(list.ParseFromIstream(&input));
    assert(list.contact_size() == 4);

    unordered_map<string, const phone_book_serialize::Contact*> persons;
    for (const auto& p : list.contact()) {
        persons[p.name()] = &p;
    }

    {
        auto it = persons.find("Ivan Ivanov"s);
        assert(it != persons.end());
        const auto& p = *it->second;

        assert(p.has_birthday());
        assert(p.birthday().year() == 1980);
    }
    {
        auto it = persons.find("Margarita Petrova"s);
        assert(it != persons.end());
        const auto& p = *it->second;

        assert(p.has_birthday());
        assert(p.birthday().year() == 1989);
        assert(p.phone_number_size() == 2);
        assert(p.phone_number(1) == "+71112223344"sv);
    }
    {
        auto it = persons.find("Just Birthday"s);
        assert(it != persons.end());
        const auto& p = *it->second;

        assert(p.has_birthday());
        assert(p.birthday().day() == 23);
        assert(p.phone_number_size() == 0);
    }
    {
        auto it = persons.find("No Birthday"s);
        assert(it != persons.end());
        const auto& p = *it->second;

        assert(!p.has_birthday());
        assert(p.phone_number_size() == 1);
    }
}

void TestDeserialization() {
    phone_book_serialize::ContactList list;

    phone_book_serialize::Contact p;
    p.set_name("Vanya"s);
    p.add_phone_number("+7-953-181-46-12"s);

    *list.add_contact() = p;

    p.Clear();
    p.set_name("Veronika"s);
    p.mutable_birthday()->set_year(2000);
    p.mutable_birthday()->set_month(3);
    p.mutable_birthday()->set_day(31);
    p.add_phone_number("325-87-16"s);

    *list.add_contact() = p;

    p.Clear();
    p.set_name("Vitya"s);

    *list.add_contact() = p;

    ostringstream output(std::ios::binary);
    list.SerializeToOstream(&output);

    istringstream input(output.str(), std::ios::binary);
    const PhoneBook book = DeserializePhoneBook(input);

    auto range = book.FindByNamePrefix(""sv);
    vector<Contact> contacts(range.begin(), range.end());

    assert(contacts.size() == 3);

    assert(contacts[0].name == "Vanya"sv);
    assert(contacts[0].phones == (vector<string>{"+7-953-181-46-12"s}));
    assert(!contacts[0].birthday);

    assert(contacts[1].name == "Veronika"sv);
    assert(contacts[1].birthday);
    assert(contacts[1].birthday->year == 2000);
    assert(contacts[1].birthday->month == 3);
    assert(contacts[1].birthday->day == 31);
    assert(contacts[1].phones == (vector<string>{"325-87-16"s}));

    assert(contacts[2].name == "Vitya"sv);
    assert(!contacts[2].birthday);
    assert(contacts[2].phones.empty());
}

void TestFindNameByPrefix() {
    PhoneBook book({
        {"Vasiliy Petrov"s, std::nullopt, {}},
        {"Ivan Ivanov"s, std::nullopt, {}},
        {"Vasiliy Ivanov"s, std::nullopt, {}},
        {"Vasilisa Kuznetsova"s, std::nullopt, {}},
        {"Ivan Petrov"s, std::nullopt, {}},
        {"Vassisualiy Lokhankin"s, std::nullopt, {}},
        {"Ivan Vasiliev"s, std::nullopt, {}},
        {""s, std::nullopt, {}},
    });

    auto get_names = [](PhoneBook::ContactRange range) {
        vector<string> result;
        for (const auto& record : range) {
            result.push_back(record.name);
        }
        return result;
    };

    assert(get_names(book.FindByNamePrefix("Ivan"sv)) == (vector<string>{"Ivan Ivanov"s, "Ivan Petrov"s, "Ivan Vasiliev"s}));
    assert(get_names(book.FindByNamePrefix("I"sv)) == (vector<string>{"Ivan Ivanov"s, "Ivan Petrov"s, "Ivan Vasiliev"s}));
    assert(get_names(book.FindByNamePrefix("Iv"sv)) == (vector<string>{"Ivan Ivanov"s, "Ivan Petrov"s, "Ivan Vasiliev"s}));
    assert(get_names(book.FindByNamePrefix("Ivan Ivan"sv)) == (vector<string>{"Ivan Ivanov"s}));
    assert(get_names(book.FindByNamePrefix("Vas"sv))
           == (vector<string>{"Vasilisa Kuznetsova"s, "Vasiliy Ivanov"s, "Vasiliy Petrov"s, "Vassisualiy Lokhankin"s}));
    assert(get_names(book.FindByNamePrefix("Vasili"sv))
           == (vector<string>{"Vasilisa Kuznetsova"s, "Vasiliy Ivanov"s, "Vasiliy Petrov"s}));
    assert(get_names(book.FindByNamePrefix("Vasiliy"sv)) == (vector<string>{"Vasiliy Ivanov"s, "Vasiliy Petrov"s}));
    assert(get_names(book.FindByNamePrefix("Vasilis"sv)) == (vector<string>{"Vasilisa Kuznetsova"s}));
    assert(get_names(book.FindByNamePrefix(""sv))
           == (vector<string>{""s, "Ivan Ivanov"s, "Ivan Petrov"s, "Ivan Vasiliev"s, "Vasilisa Kuznetsova"s,
                              "Vasiliy Ivanov"s, "Vasiliy Petrov"s, "Vassisualiy Lokhankin"s}));
}

void TestFindNameByPrefix2() {
    PhoneBook book({
        {"a"s, std::nullopt, {}},
        {"aaaa"s, std::nullopt, {}},
        {"aabc"s, std::nullopt, {}},
        {"aabccc"s, std::nullopt, {}},
        {"aabcbc"s, std::nullopt, {}},
        {"aeca"s, std::nullopt, {}},
        {"aeca"s, std::nullopt, {}},
        {"aefg"s, std::nullopt, {}},
        {"aq"s, std::nullopt, {}},
    });

    auto get_names = [](PhoneBook::ContactRange range) {
        vector<string> result;
        for (const auto& record : range) {
            result.push_back(record.name);
        }
        return result;
    };

    assert(book.FindByNamePrefix("a"sv).size() == 9u);
    assert(get_names(book.FindByNamePrefix("aa"sv)) == (vector<string>{"aaaa"s, "aabc"s, "aabcbc"s, "aabccc"s}));

    assert(book.FindByNamePrefix("ab"sv).size() == 0u);
    assert(book.FindByNamePrefix("b"sv).size() == 0u);
    assert(book.FindByNamePrefix("aaaaa"sv).size() == 0u);
    assert(book.FindByNamePrefix("aeb"sv).size() == 0u);
    assert(book.FindByNamePrefix("aed"sv).size() == 0u);
    assert(book.FindByNamePrefix("aeg"sv).size() == 0u);

    assert(get_names(book.FindByNamePrefix("aaa"sv)) == (vector<string>{"aaaa"s}));
    assert(get_names(book.FindByNamePrefix("ae"sv)) == (vector<string>{"aeca"s, "aeca"s, "aefg"s}));
}

int main() {
    TestFindNameByPrefix();
    TestFindNameByPrefix2();
    TestSerialization();
    TestDeserialization();
}
