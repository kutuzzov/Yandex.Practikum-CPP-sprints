#pragma once

#include "identity_document.h"
#include <iostream>
#include <string>

using namespace std::string_view_literals;

class DrivingLicence {
public:
    DrivingLicence()
        : id_() {
        SetVtable(this);
        std::cout << "DrivingLicence::Ctor()"sv << std::endl;
    }

    DrivingLicence(const DrivingLicence& other)
        : id_(other.id_) 
    {
        SetVtable(this);
        std::cout << "DrivingLicence::CCtor()"sv << std::endl;
    }

    ~DrivingLicence() {
        IdentityDocument::SetVtable((IdentityDocument*)this);
        std::cout << "DrivingLicence::Dtor()"sv << std::endl;
    }

    struct Vtable {
        void (*VirtualPrintId)(const DrivingLicence*);
        void (*VirtualDelete)(DrivingLicence*);
        int (*VirtualGetId)(const DrivingLicence*);
    };

    static void SetVtable(DrivingLicence* obj) {
        *(DrivingLicence::Vtable**)obj = &vtable_;
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
        return GetVtable()->VirtualGetId(this);
    }

    operator IdentityDocument() {
        return { id_ };
    }

private:
    IdentityDocument id_;
    static Vtable vtable_;

    static void PrintID(const DrivingLicence* obj) {
        std::cout << "DrivingLicence::PrintID() : "sv << obj->id_.GetID() << std::endl;
    }

    static void Delete(DrivingLicence* obj) {
        delete obj;
    }

    static int GetID(const DrivingLicence* obj) {
        return obj->id_.GetID();
    }
};

DrivingLicence::Vtable DrivingLicence::vtable_ = { DrivingLicence::PrintID, DrivingLicence::Delete, DrivingLicence::GetID };