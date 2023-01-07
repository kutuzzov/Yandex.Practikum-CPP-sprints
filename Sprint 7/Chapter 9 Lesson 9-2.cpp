#include <iostream>
#include <map>
#include <string_view>
#include <cassert>

using namespace std;

class Translator {
public:
    Translator() = default;
    
    void Add(string_view source, string_view target) {
        russian_to_english_.insert(pair{source, target});
        english_to_russian_.insert(pair{target, source});
    }
    
    string_view TranslateForward(string_view source) const {
        if (auto search = russian_to_english_.find(static_cast<string>(source)); search != russian_to_english_.end()) {
            return search->second;
        }
        return {};
    }
    
    string_view TranslateBackward(string_view target) const {
        if (auto search = english_to_russian_.find(static_cast<string>(target)); search != english_to_russian_.end()) {
            return search->second;
        }
        return {};
    }

private:
    map<string, string> russian_to_english_{};
    map<string, string> english_to_russian_{};
};

void TestSimple() {
    Translator translator;
    translator.Add(string("okno"s), string("window"s));
    translator.Add(string("stol"s), string("table"s));

    assert(translator.TranslateForward("okno"s) == "window"s);
    assert(translator.TranslateBackward("table"s) == "stol"s);
    assert(translator.TranslateForward("table"s) == ""s);
}

int main() {
    TestSimple();
    return 0;
}
