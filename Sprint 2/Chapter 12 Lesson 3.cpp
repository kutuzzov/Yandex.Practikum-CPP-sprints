#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std;

#define ASSERT(expr) AssertImpl((expr), #expr, __FILE__, __FUNCTION__, __LINE__, ""s)
#define ASSERT_HINT(expr, hint) AssertImpl((expr), #expr, __FILE__, __FUNCTION__, __LINE__, (hint))

void AssertImpl(bool value, const string& expr, const string& file,
                     const string& func, unsigned line, const string& hint) {
    if (value == false) {
        cout << boolalpha;
        cout << file << "("s << line << "): "s << func << ": "s;
        cout << "ASSERT("s << expr << ") failed. "s;
        if (!hint.empty()) {
            cout << "Hint: "s << hint;
        }
        cout << endl;
        abort();
    }
}

int main() {
    string hello = "hello"s;
    ASSERT(!hello.empty());
    ASSERT_HINT(2 + 2 == 5, "This will fail"s);
}
