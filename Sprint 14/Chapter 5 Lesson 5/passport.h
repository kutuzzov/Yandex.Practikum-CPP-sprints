//#pragma warning(disable : 4996)
#pragma once

#include "identity_document.h"
#include <iostream>
#include <string>
#include <ctime>

using namespace std::string_view_literals;

class Passport {
public:
    Passport()
        : id_()
        , expiration_date_(GetExpirationDate()) 
    {
        SetVtable(this);
        std::cout << "Passport::Ctor()"sv << std::endl;
    }

    Passport(const Passport& other)
        : id_(other.id_)
        , expiration_date_(other.expiration_date_) 
    {
        SetVtable(this);
        std::cout << "Passport::CCtor()"sv << std::endl;
    }

    ~Passport() {
        IdentityDocument::SetVtable((IdentityDocument*)this);
        std::cout << "Passport::Dtor()"sv << std::endl;
    }

    void PrintVisa(const std::string& country) const {
        std::cout << "Passport::PrintVisa("sv << country << ") : "sv << GetID() << std::endl;
    }

    void PrintUniqueIDCount() {
        IdentityDocument::PrintUniqueIDCount();
    }

    struct Vtable {
        void (*VirtualPrintId)(const Passport*);
        void (*VirtualDelete)(Passport*);
    };

    static void SetVtable(Passport* obj) {
        *(Passport::Vtable**)obj = &vtable_;
    }

    Vtable* GetVtable() const {
        return reinterpret_cast<Vtable*>(id_.GetVtable());
    }

    void PrintID() const {
        GetVtable()->VirtualPrintId(this);
    }

    void Delete() {
        GetVtable()->VirtualDelete(this);
    }

    int GetID() const {
        return id_.GetID();
    }

    operator IdentityDocument() {
        return { id_ };
    }

private:
    IdentityDocument id_;
    static Vtable vtable_;

    const struct tm expiration_date_;

    tm GetExpirationDate() {
	    time_t t = time(nullptr);
	    tm exp_date = *localtime(&t);
	    exp_date.tm_year += 10;
	    mktime(&exp_date);
	    return exp_date;
	}

    static void PrintID(const Passport* obj) {
        std::cout << "Passport::PrintID() : "sv << obj->GetID();
        std::cout << " expiration date : "sv << obj->expiration_date_.tm_mday << "/"sv << obj->expiration_date_.tm_mon << "/"sv
                  << obj->expiration_date_.tm_year + 1900 << std::endl;
    }

    static void Delete(Passport* obj) {
        delete obj;
    }
};

Passport::Vtable Passport::vtable_ = { Passport::PrintID, Passport::Delete };