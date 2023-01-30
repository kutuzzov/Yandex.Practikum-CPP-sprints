#pragma once

#include <iostream>
#include <string>
#include <vector>

struct Document {
    Document() = default;

    Document(int id_, double relevance_, int rating_)
        : id(id_), relevance(relevance_), rating(rating_)
    {}

    int id = 0;
    double relevance = 0.0;
    int rating = 0;
};

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED
};

std::ostream& operator<<(std::ostream& out, const Document& document);