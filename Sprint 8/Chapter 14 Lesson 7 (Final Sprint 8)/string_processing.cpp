#include "string_processing.h"

std::vector<std::string> SplitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

std::vector<std::string_view> SplitIntoWordsView(std::string_view str) {
    std::vector<std::string_view> result;
    str.remove_prefix(std::min(str.find_first_not_of(' '), str.size()));
    const std::string_view::size_type pos_end = std::string_view::npos;

    while (!str.empty()) {
        std::string_view::size_type space = str.find(' ');
        result.push_back(space == pos_end ? str.substr(0, str.size()) : str.substr(0, space));
        str.remove_prefix(std::min(str.find_first_not_of(' ', space), str.size()));
    }

    return result;
}