#include <array>
#include <iostream>
#include <string>
#include "identity_document.h"
#include "passport.h"
#include "driving_licence.h"
#include "international_driving_licence.h"
#include "travel_pack.h"
using namespace std;

void PrintInfo(const IdentityDocument& doc) {
    doc.PrintID();
}

void PrintInfo() {
    IdentityDocument::PrintUniqueIDCount();
}

void PrintInfo(const Passport& pass) {
    pass.PrintVisa("France"s);
}

IdentityDocument PrintInfo(int i) {
    Passport pass;
    cout << "PrintInfo("sv << i << ")"sv << endl;
    pass.PrintID();
    return pass;
}

void PrintDrivingLicence(DrivingLicence dr_lic) {
    dr_lic.PrintID();
}

int main() {
    cout << "Test1"sv << endl;
    Passport pass;
    PrintInfo(pass);
    PrintInfo(3).PrintID();

    cout << "Test2"sv << endl;
    array<IdentityDocument*, 3> docs = { (IdentityDocument*)(new Passport()), (IdentityDocument*)(new DrivingLicence()), (IdentityDocument*)(new Passport()) };
    for (const auto* doc : docs) {
        doc->PrintID();
    }

    for (size_t i = 0; i < docs.size(); ++i) {
        docs[i]->Delete();
    }

    cout << "Test3"sv << endl;
    array<IdentityDocument, 3> docs2 = {Passport(), DrivingLicence(), IdentityDocument()};

    for (size_t i = 0; i < docs2.size(); ++i) {
        docs2[i].PrintID();
    }

    PrintInfo(docs2[0]);

    cout << "Test4"sv << endl;
    IdentityDocument::PrintUniqueIDCount();

    cout << "Test5"sv << endl;
    Passport pass2;
    pass2.PrintUniqueIDCount();
}
