#include <set>
#include <string>
#include <numeric>
#include <iostream>

using namespace std;

string Tail(string left, string right) {
    return left + " -"s + right;
}

string AddStopWords(const string& query, const set<string>& stop_words) {
    return query + accumulate(stop_words.begin(), stop_words.end(), ""s, Tail);
}

int main() {
    string query{ "some tasty oranges"s };
    set<string> stop_words{ "of"s, "in"s };
    cout << AddStopWords(query, stop_words) << endl;
}
