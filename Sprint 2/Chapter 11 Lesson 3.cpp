#include <iostream>
#include <string>

using namespace std;

// Определяет, будет ли слово палиндромом
// text может быть строкой, содержащей строчные символы английского алфавита и пробелы
// Пустые строки и строки, состоящие только из пробелов, — это не палиндромы
bool IsPalindrome(const string& text) {
    // Напишите недостающий код
    if (text.empty()) return false;

    string text_without_spaces;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] != ' ') text_without_spaces += text[i];
    }

    if (text_without_spaces.empty()) return false;
    bool result = true;
    for (size_t i = 0; i < (text_without_spaces.size() / 2); ++i) {
        if (text_without_spaces[i] == text_without_spaces[text_without_spaces.size() - 1 - i]) {
            result = true;
        }
        else {
            result = false;
        }
    }
    return result;
}

/*
void Print(const string& text) {
    if (IsPalindrome(text)) {
        cout << "palindrome"s << endl;
    }
    else {
        cout << "not a palindrome"s << endl;
    }
}
*/

int main() {
    string text;
    getline(cin, text);

    if (IsPalindrome(text)) {
        cout << "palindrome"s << endl;
    }
    else {
        cout << "not a palindrome"s << endl;
    }
    /*
    Print("radar"s);
    Print("never odd or even"s);
    Print("apple"s);
    Print("kick"s);
    Print(""s);
    Print(" "s);
    */
}
