#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

template <typename Term>
map<Term, int> ComputeTermFreqs(const vector<Term>& terms) {
    map<Term, int> term_freqs;
    for (const Term& term : terms) {
        ++term_freqs[term];
    }
    return term_freqs;
}

pair<string, int> FindMaxFreqAnimal(const vector<pair<string, int>>& animals) {
    // верните животного с максимальной частотой
    int max_freq = 0;
    pair<string, int> max_freq_animal;
    for (auto& [animal, freq] : ComputeTermFreqs(animals)) {
        if (freq > max_freq) {
            max_freq = freq;
            max_freq_animal = animal;
        }
    }
    return max_freq_animal;
}

int main() {
    const vector<pair<string, int>> animals = {
        {"Murka"s, 5},
        {"Belka"s, 6},
        {"Murka"s, 7},
        {"Murka"s, 5},
        {"Belka"s, 6},
    };
    const pair<string, int> max_freq_animal = FindMaxFreqAnimal(animals);
    cout << max_freq_animal.first << " "s
        << max_freq_animal.second << endl;
}
