#include "search_server.h"

void SearchServer::AddDocument(int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings) {
    if (document_id < 0) {
        throw std::invalid_argument("документ с отрицательным id"s);
    }
    if (documents_.count(document_id)) {
        throw std::invalid_argument("документ c id ранее добавленного документа"s);
    }
    if (!IsValidWord(document)) {
        throw std::invalid_argument("наличие недопустимых символов"s);
    }

    const std::vector<std::string> words = SplitIntoWordsNoStop(document);
    for (auto& word : words) {
        word_to_document_freqs_[word][document_id] += 1.0 / words.size();
    }
    for (auto& word : words) {
        document_to_word_freqs_[document_id][word] += 1.0 / words.size();
    }
    documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status });
    document_ids_.emplace(document_id);
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, DocumentStatus status) const {
    return FindTopDocuments(raw_query,
        [&status](int document_id, DocumentStatus new_status, int rating) {
            return new_status == status;
        });
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
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

std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::string& raw_query, int document_id) const {
    return MatchDocument(std::execution::seq, raw_query, document_id);
}

std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::execution::sequenced_policy&, const std::string& raw_query, int document_id) const {
    const Query query = ParseQuery(raw_query);

    std::vector<std::string> matched_words;
    for (const std::string& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }
    for (const std::string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.clear();
            break;
        }
    }

    return { matched_words, documents_.at(document_id).status };
}

std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::execution::parallel_policy&, const std::string& raw_query, int document_id) const {
    const Query query = ParseQueryParallel(raw_query);
    std::vector<std::string> matched_words;
    
    if (any_of(query.minus_words.begin(), query.minus_words.end(),
        [this, document_id](const std::string& word) {
            return (word_to_document_freqs_.count(word) > 0 &&
            word_to_document_freqs_.at(word).count(document_id) > 0);
        })) {
        return { matched_words, documents_.at(document_id).status };
    }
    
    matched_words.resize(query.plus_words.size());
    const auto& it = std::copy_if(query.plus_words.begin(), query.plus_words.end(),
        matched_words.begin(),
        [this, document_id](const std::string& word) {
            return (word_to_document_freqs_.count(word) > 0 &&
            word_to_document_freqs_.at(word).count(document_id) > 0);
        }
    );
    matched_words.erase(it, matched_words.end());
    
    std::sort(std::execution::par, matched_words.begin(), matched_words.end());
    const auto& itr = std::unique(matched_words.begin(), matched_words.end());
    matched_words.erase(itr, matched_words.end());
    //
    return { matched_words, documents_.at(document_id).status };
}

const std::map<std::string, double>& SearchServer::GetWordFrequencies(int document_id) const {
    static const std::map<std::string, double> dummy;
    if (document_to_word_freqs_.count(document_id) == 0) {
        return dummy;
    }
    else {
        return document_to_word_freqs_.at(document_id);
    }
}

void SearchServer::RemoveDocument(int document_id) {
    return RemoveDocument(std::execution::seq, document_id);
}

void SearchServer::RemoveDocument(const std::execution::sequenced_policy&, int document_id) {
    if (!document_ids_.count(document_id)) {
        throw std::invalid_argument("Invalid document ID to remove"s);
    }

    document_ids_.erase(document_id);

    if (documents_.count(document_id)) {
        documents_.erase(document_id);
    }

    for (auto& element : SearchServer::word_to_document_freqs_) {
        if (element.second.count(document_id)) {
            element.second.erase(document_id);
        }
    }

    if (document_to_word_freqs_.count(document_id)) {
        document_to_word_freqs_.erase(document_id);
    }
}

void SearchServer::RemoveDocument(const std::execution::parallel_policy&, int document_id) {
    if (!document_ids_.count(document_id)) {
        throw std::invalid_argument("Invalid document ID to remove"s);
    }

    std::map<std::string, double> word_freqs = document_to_word_freqs_.at(document_id);
    std::vector<const std::string*> words_to_erase(word_freqs.size());

    std::transform(std::execution::par, word_freqs.begin(), word_freqs.end(),
        words_to_erase.begin(),
        [](const auto& words_freq) { return &words_freq.first; });

    std::for_each(std::execution::par, words_to_erase.begin(), words_to_erase.end(),
        [this, document_id](const auto& word) {word_to_document_freqs_.at(*word).erase(document_id); });

    documents_.erase(document_id);
    document_ids_.erase(document_id);
    document_to_word_freqs_.erase(document_id);
}

bool SearchServer::IsStopWord(const std::string& word) const {
    return stop_words_.count(word) > 0;
}

bool SearchServer::IsValidWord(const std::string& word) {
    // A valid word must not contain special characters
    return none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
        });
}

std::vector<std::string> SearchServer::SplitIntoWordsNoStop(const std::string& text) const {
    std::vector<std::string> words;
    for (const std::string& word : SplitIntoWords(text)) {
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

SearchServer::QueryWord SearchServer::ParseQueryWord(std::string text) const {
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

SearchServer::Query SearchServer::ParseQuery(const std::string& text) const {
    Query result;
    for (const std::string& word : SplitIntoWords(text)) {
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
    std::sort(result.minus_words.begin(), result.minus_words.end());
    auto itm = std::unique(result.minus_words.begin(), result.minus_words.end());
    result.minus_words.resize(std::distance(result.minus_words.begin(), itm));
    
    std::sort(result.plus_words.begin(), result.plus_words.end());
    auto itp = std::unique(result.plus_words.begin(), result.plus_words.end());
    result.plus_words.resize(std::distance(result.plus_words.begin(), itp));
    
    return result;
}

SearchServer::Query SearchServer::ParseQueryParallel(const std::string& text) const {
    Query result;
    for (const std::string& word : SplitIntoWords(text)) {
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
    return result;
}
