#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <future>
#include <vector>

using namespace std;
using KeyWords = set<string, less<>>;

struct Stats {
    map<string, int> word_frequences;

    void operator+=(const Stats& other) {
        for (const auto& [word, freq] : other.word_frequences) {
            word_frequences[word] += freq;
        }
    }
};

Stats ExploreLine(const KeyWords& key_words, const string& line) {
    Stats result;
    istringstream is(line);
    string word;

    while (is >> word) {
        if (key_words.count(word)) ++result.word_frequences[word];
    }

    return result;
}

Stats ExploreBatch(const KeyWords& key_words, vector<string> lines) {
    Stats result;

    for (const string& line : lines) {
        result += ExploreLine(key_words, line);
    }

    return result;
}

Stats ExploreKeyWords(const KeyWords& key_words, istream& input) {
    Stats result;
    vector<future<Stats>> futures;
    vector<string> batch;

    for (string line; getline(input, line);) {
        batch.push_back(move(line));
        futures.push_back(async(ExploreBatch, ref(key_words), move(batch)));
    }
    for (auto& f : futures) {
        result += f.get();
    }

    return result;
}

int main() {
    // rocks - 2, sucks - 1, yangle - 6
    const KeyWords key_words = { "yangle", "rocks", "sucks", "all" };

    stringstream ss;
    ss << "this new yangle service really rocks\n";
    ss << "It sucks when yangle isn't available\n";
    ss << "10 reasons why yangle is the best IT company\n";
    ss << "yangle rocks others suck\n";
    ss << "Goondex really sucks, but yangle rocks. Use yangle\n";

    for (const auto& [word, frequency] : ExploreKeyWords(key_words, ss).word_frequences) {
        cout << word << " " << frequency << endl;
    }

    return 0;
}
