#pragma once

#include <iostream>
#include <string>

using namespace std::string_view_literals;

class IdentityDocument {
public:
    IdentityDocument()
        : unique_id_(++unique_id_count_) 
    {
        SetVtable(this);
        std::cout << "IdentityDocument::Ctor() : "sv << unique_id_ << std::endl;
    }

    ~IdentityDocument() {
        --unique_id_count_;
        std::cout << "IdentityDocument::Dtor() : "sv << unique_id_ << std::endl;
    }

    IdentityDocument(const IdentityDocument& other)
        : unique_id_(++unique_id_count_) 
    {
        SetVtable(this);
        std::cout << "IdentityDocument::CCtor() : "sv << unique_id_ << std::endl;
    }

    IdentityDocument& operator=(const IdentityDocument&) = delete;


    static void PrintUniqueIDCount() {
        std::cout << "unique_id_count_ : "sv << unique_id_count_ << std::endl;
    }

    struct Vtable {
        void (*VirtualPrintId)(const IdentityDocument*);
        void (*VirtualDelete)(IdentityDocument*);
    };

    static void SetVtable(IdentityDocument* obj) {
        *(Vtable**)obj = &vtable_;
    }

    Vtable* GetVtable() const {
        return vptr_;
    }

    void PrintID() const {
        GetVtable()->VirtualPrintId(this);
    }

    void Delete() {
        GetVtable()->VirtualDelete(this);
    }

    int GetID() const {
        return unique_id_;
    }

private:
    Vtable* vptr_;
    static Vtable vtable_;
    static int unique_id_count_;
    int unique_id_;

    static void PrintID(const IdentityDocument* obj) {
        std::cout << "IdentityDocument::PrintID() : "sv << obj->unique_id_ << std::endl;
    }

    static void Delete(IdentityDocument* obj) {
        delete(obj);
    }
};

int IdentityDocument::unique_id_count_ = 0;
IdentityDocument::Vtable IdentityDocument::vtable_ = { IdentityDocument::PrintID, IdentityDocument::Delete };