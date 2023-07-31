#pragma once

#include "driving_licence.h"
#include <iostream>
#include <string>

using namespace std::string_view_literals;

class InternationalDrivingLicence {
public:
    InternationalDrivingLicence()
        : id_() {
        SetVtable(this);
        std::cout << "InternationalDrivingLicence::Ctor()"sv << std::endl;
    }

    InternationalDrivingLicence(const InternationalDrivingLicence& other)
        : id_(other.id_) 
    {
        SetVtable(this);
        std::cout << "InternationalDrivingLicence::CCtor()"sv << std::endl;
    }

    ~InternationalDrivingLicence() {
        DrivingLicence::SetVtable((DrivingLicence*)this);
        std::cout << "InternationalDrivingLicence::Dtor()"sv << std::endl;
    }

    struct Vtable {
        void (*VirtualPrintId)(const InternationalDrivingLicence*);
        void (*VirtualDelete)(InternationalDrivingLicence*);
        int (*VirtualGetId)(const InternationalDrivingLicence*);
    };

    void SetVtable(InternationalDrivingLicence* obj) {
        *(InternationalDrivingLicence::Vtable**)obj = &vtable_;
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

    operator DrivingLicence() {
        return id_;
    }

private:
    DrivingLicence id_;
    static Vtable vtable_;

    static void PrintID(const InternationalDrivingLicence* obj) {
        std::cout << "InternationalDrivingLicence::PrintID() : "sv << obj->id_.GetID() << std::endl;
    }

    static void Delete(InternationalDrivingLicence* obj) {
        delete obj;
    }

    static int GetID(const InternationalDrivingLicence* obj) {
        return obj->id_.GetID();
    }
};

InternationalDrivingLicence::Vtable InternationalDrivingLicence::vtable_ = { InternationalDrivingLicence::PrintID, InternationalDrivingLicence::Delete, InternationalDrivingLicence::GetID };