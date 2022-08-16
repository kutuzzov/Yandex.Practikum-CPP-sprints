#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

template <typename Documents, typename Term>
vector<double> ComputeTfIdfs(const Documents& documents, const Term& term) {
    vector<double> result;
    int document_freq = 0;
    for (const auto& document : documents) {
        int freq = static_cast<int>(count(document.begin(), document.end(), term));
        if (freq > 0) {
            ++document_freq;
        }
        result.push_back(static_cast<double>(freq) / document.size());
    }
    double idf = log(static_cast<double>(documents.size()) / document_freq);
    for (auto& element : result) {
        element *= idf;
    }
    return result;
}

int main() {
    const vector<vector<string>> documents = {
        {"белый"s, "кот"s, "и"s, "модный"s, "ошейник"s},
        {"пушистый"s, "кот"s, "пушистый"s, "хвост"s},
        {"ухоженный"s, "пёс"s, "выразительные"s, "глаза"s},
    };
    const auto& tf_idfs = ComputeTfIdfs(documents, "кот"s);
    for (const double tf_idf : tf_idfs) {
        cout << tf_idf << " "s;
    }
    cout << endl;
}
