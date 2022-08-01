#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

int main() {
    // считайте входные данные и сформируйте вывод программы
    // ...
    int document_count;
    cin >> document_count;
    
    vector<string> words;
    words.reserve(document_count);
    string word;
    
    for (int document_id = 0; document_id < document_count; ++document_id) {
        cin >> word;
        words.push_back(word);
    }
    
    sort(words.begin(), words.end(), [](string& lhs, string& rhs) {
        return lexicographical_compare(
            lhs.begin(), lhs.end(),
            rhs.begin(), rhs.end(),
            [](char cl, char cr) { return tolower(cl) < tolower(cr); }
        );
    });
    
    for (auto word : words) {
        cout << word << " "s;
    }
    cout << endl;
}
