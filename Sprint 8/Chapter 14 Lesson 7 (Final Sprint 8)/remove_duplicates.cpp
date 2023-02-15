#include "remove_duplicates.h"

void RemoveDuplicates(SearchServer& search_server) {
	std::map<std::vector<std::string_view>, int> doc_words;
	std::vector<int> duplicate_ids;

	for (const int document_id : search_server) {
		auto word_freqs = search_server.GetWordFrequencies(document_id);
		std::vector<std::string_view> words(word_freqs.size());
		std::transform(word_freqs.cbegin(), word_freqs.cend(),
			std::inserter(words, words.begin()), [](const std::pair<std::string_view, double>& elements) {
				return elements.first;
			});

        if (!words_of_docs_processed.count(doc_words_vector)) {
            words_of_docs_processed.insert(doc_words_vector);
        }
        else {
            doc_duplicated_ids.insert(doc_found);
        }
    }

	for (const int document_id : duplicate_ids) {
		std::cout << "Found duplicate document id "s << document_id << std::endl;
		search_server.RemoveDocument(document_id);
	}
}
