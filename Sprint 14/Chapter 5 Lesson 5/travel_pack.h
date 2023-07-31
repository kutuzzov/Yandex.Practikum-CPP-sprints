#pragma once

#include "identity_document.h"
#include <iostream>
#include <string>

using namespace std::string_view_literals;

class TravelPack {
public:
    TravelPack()
        : id_()
        , identity_doc1_((IdentityDocument*)new Passport())
        , identity_doc2_((IdentityDocument*)new DrivingLicence())
    {
        SetVtable(this);
        std::cout << "TravelPack::Ctor()"sv << std::endl;
    }

    TravelPack(const TravelPack& other)
        : id_(other.id_)
        , identity_doc1_((IdentityDocument*)new Passport(*reinterpret_cast<Passport*>(other.identity_doc1_)))
        , identity_doc2_((IdentityDocument*)new DrivingLicence(*reinterpret_cast<DrivingLicence*>(other.identity_doc2_)))
        , additional_pass_(other.additional_pass_)
        , additional_dr_licence_(other.additional_dr_licence_) 
    {
        SetVtable(this);
        std::cout << "TravelPack::CCtor()"sv << std::endl;
    }

    ~TravelPack() {
        IdentityDocument::SetVtable((IdentityDocument*)this);
        identity_doc1_->Delete();
        identity_doc2_->Delete();

        std::cout << "TravelPack::Dtor()"sv << std::endl;
    }

    struct Vtable {
        void (*VirtualPrintId)(const TravelPack*);
        void (*VirtualDelete)(TravelPack*);
    };

    void SetVtable(TravelPack* obj) {
        *(TravelPack::Vtable**)obj = &vtable_;
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

private:
    IdentityDocument id_;
    static Vtable vtable_;
    
    IdentityDocument* identity_doc1_;
    IdentityDocument* identity_doc2_;
    Passport additional_pass_;
    DrivingLicence additional_dr_licence_;

    static void PrintID(const TravelPack* obj) {
        obj->identity_doc1_->PrintID();
        obj->identity_doc2_->PrintID();
        obj->additional_pass_.PrintID();
        obj->additional_dr_licence_.PrintID();
    }

    static void Delete(TravelPack* obj) {
        delete obj;
    }
};

TravelPack::Vtable TravelPack::vtable_ = { TravelPack::PrintID, TravelPack::Delete };