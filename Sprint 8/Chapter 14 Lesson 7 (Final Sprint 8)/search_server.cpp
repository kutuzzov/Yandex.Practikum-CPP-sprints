#include "search_server.h"

void SearchServer::AddDocument(int document_id, std::string_view document, DocumentStatus status, const std::vector<int>& ratings) {
    if (document_id < 0) {
        throw std::invalid_argument("документ с отрицательным id"s);
    }
    if (documents_.count(document_id)) {
        throw std::invalid_argument("документ c id ранее добавленного документа"s);
    }
    if (!IsValidWord(document)) {
        throw std::invalid_argument("наличие недопустимых символов"s);
    }

    auto words = SplitIntoWordsNoStop(document);
    const std::string document_string{ document };
    documents_.emplace(document_id, DocumentData{ SearchServer::ComputeAverageRating(ratings), status, document_string });
    
    words = SplitIntoWordsNoStop(documents_.at(document_id).text_);
    
    for (auto word : words) {
        word_to_document_freqs_[word][document_id] += 1.0 / words.size();
        document_to_word_freqs_[document_id][word] += 1.0 / words.size();
    }
    
    document_ids_.emplace(document_id);
}

std::vector<Document> SearchServer::FindTopDocuments(std::string_view raw_query, DocumentStatus status) const {
    return FindTopDocuments(std::execution::seq, raw_query, status);
}

std::vector<Document> SearchServer::FindTopDocuments(std::string_view raw_query) const {
    return FindTopDocuments(std::execution::seq, raw_query, DocumentStatus::ACTUAL);
}

int SearchServer::GetDocumentCount() const {
    return static_cast<int>(documents_.size());
}

const std::set<int>::const_iterator SearchServer::begin() const noexcept {
    return document_ids_.begin();
}

const std::set<int>::const_iterator SearchServer::end() const noexcept {
    return document_ids_.end();
}

std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(std::string_view raw_query, int document_id) const {
    const Query query = ParseQuery(raw_query);

    std::vector<std::string_view> matched_words;
    for (const std::string_view word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.clear();
            return { {}, documents_.at(document_id).status };
        }
    }
    for (const std::string_view word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }

    return { matched_words, documents_.at(document_id).status };
}

std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(const std::execution::sequenced_policy& policy, std::string_view raw_query, int document_id) const {
    return MatchDocument(raw_query, document_id);
}

std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(const std::execution::parallel_policy& policy, std::string_view raw_query, int document_id) const {
    if ((document_id < 0) || (documents_.count(document_id) == 0)) {
            throw std::invalid_argument("document_id out of range"s);
        }

    const Query& query = ParseQueryParallel(raw_query);
    const auto& word_freqs = document_to_word_freqs_.at(document_id);
    
    if (std::any_of(query.minus_words.begin(),
                    query.minus_words.end(),
                    [&word_freqs](const std::string_view word) {
                        return word_freqs.count(word) > 0;
                    })) {
        return { {}, documents_.at(document_id).status };
    }

    std::vector<std::string_view> matched_words;
    matched_words.reserve(query.plus_words.size());
    std::copy_if(query.plus_words.begin(),
                 query.plus_words.end(),
                 std::back_inserter(matched_words),
                 [&word_freqs](const std::string_view word) {
                     return word_freqs.count(word) > 0;
                 });

    std::sort(policy, matched_words.begin(), matched_words.end());
    auto it = std::unique(matched_words.begin(), matched_words.end());
    matched_words.erase(it, matched_words.end());

    return { matched_words, documents_.at(document_id).status };
}

const std::map<std::string_view, double>& SearchServer::GetWordFrequencies(int document_id) const {
    static const std::map<std::string_view, double> dummy;
    if (document_to_word_freqs_.count(document_id) == 0) {
        return dummy;
    }
    else {
        return document_to_word_freqs_.at(document_id);
    }
}

void SearchServer::RemoveDocument(int document_id) {
    if (document_to_word_freqs_.count(document_id)) {
        std::map<std::string_view, double> search_element = document_to_word_freqs_[document_id];

        for (auto [key_string, value_double] : search_element) {
            word_to_document_freqs_[key_string].erase(document_id);
        }

        document_to_word_freqs_.erase(document_id);
        documents_.erase(document_id);
        document_ids_.erase(document_id);
    }
}

bool SearchServer::IsStopWord(std::string_view word) const {
    return stop_words_.count(word) > 0;
}

bool SearchServer::IsValidWord(std::string_view word) {
    // A valid word must not contain special characters
    return std::none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
        });
}

std::vector<std::string_view> SearchServer::SplitIntoWordsNoStop(std::string_view text) const {
    std::vector<std::string_view> words;
    for (const auto& word : SplitIntoWordsView(text)) {
        if (!IsValidWord(word)) {
            throw std::invalid_argument("наличие недопустимых символов"s);
        }
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}

int SearchServer::ComputeAverageRating(const std::vector<int>& ratings) {
    if (ratings.empty()) {
        return 0;
    }
    int rating_sum = 0;
    for (const auto rating : ratings) {
        rating_sum += rating;
    }
    return rating_sum / static_cast<int>(ratings.size());
}

SearchServer::QueryWord SearchServer::ParseQueryWord(std::string_view text) const {
    QueryWord result;

    if (text.empty()) {
        throw std::invalid_argument("отсутствие текста после символа «минус» в поисковом запросе"s);
    }
    bool is_minus = false;
    if (text[0] == '-') {
        is_minus = true;
        text = text.substr(1);
    }
    if (text.empty() || text[0] == '-' || !IsValidWord(text)) {
        throw std::invalid_argument("наличие более чем одного минуса перед словами"s);
    }

    return { text, is_minus, IsStopWord(text) };
}

SearchServer::Query SearchServer::ParseQuery(std::string_view text) const {
    Query result;

    for (auto word : SplitIntoWordsView(text)) {
        const QueryWord query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                result.minus_words.push_back(query_word.data);
            }
            else {
                result.plus_words.push_back(query_word.data);
            }
        }
    }

    sort(result.minus_words.begin(), result.minus_words.end());
    sort(result.plus_words.begin(), result.plus_words.end());

    auto last_minus = unique(result.minus_words.begin(), result.minus_words.end());
    auto last_plus = unique(result.plus_words.begin(), result.plus_words.end());

    size_t newSize = last_minus - result.minus_words.begin();
    result.minus_words.resize(newSize);

    newSize = last_plus - result.plus_words.begin();
    result.plus_words.resize(newSize);

    return result;
}

SearchServer::Query SearchServer::ParseQueryParallel(std::string_view text) const {
    Query result;

    for (auto word : SplitIntoWordsView(text)) {
        const QueryWord query_word(ParseQueryWord(word));
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                result.minus_words.push_back(query_word.data);
            }
            else {
                result.plus_words.push_back(query_word.data);
            }
        }
    }
    return result;
}

double SearchServer::ComputeWordInverseDocumentFreq(std::string_view word ) const {
    return std::log( GetDocumentCount() * 1.0 / word_to_document_freqs_.at( word ).size() );
}