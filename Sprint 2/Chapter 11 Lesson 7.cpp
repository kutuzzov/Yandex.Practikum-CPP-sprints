// -------- Начало модульных тестов поисковой системы ----------

// Добавление документов
void TestFindAddedDocumentByDocumentWord() {
    {
        SearchServer server;
        assert(server.GetDocumentCount() == 0);
        server.AddDocument(42, "cat in the city"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
        assert(server.GetDocumentCount() == 1);
        server.AddDocument(43, "black dog fluffy tail", DocumentStatus::ACTUAL, { 1, 2, 3 });
        assert(server.GetDocumentCount() == 2);
        const auto found_docs = server.FindTopDocuments("cat"s);
        assert(found_docs.size() == 1);
        const Document& doc0 = found_docs[0];
        assert(doc0.id == 42);
    }
}

// Поддержка стоп-слов
void TestExcludeStopWordsFromAddedDocumentContent() {
    {
        SearchServer server;
        server.AddDocument(42, "cat in the city"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
        const auto found_docs = server.FindTopDocuments("in"s);
        assert(found_docs.size() == 1);
        const Document& doc0 = found_docs[0];
        assert(doc0.id == 42);
    }

    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(42, "cat in the city"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
        assert(server.FindTopDocuments("in"s).empty());
    }
}

// Поддержка минус-слов
void TestExcludeDocumentsWithMinusWordsFromResults() {
    SearchServer server;
    server.AddDocument(101, "cat in the city"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
    server.AddDocument(102, "black dog fluffy tail", DocumentStatus::ACTUAL, { 1, 2, 3 });
    {
        const auto found_docs = server.FindTopDocuments("city -dog"s);
        assert(found_docs.size() == 1);
        assert(found_docs[0].id == 101);
    }
    {
        const auto found_docs = server.FindTopDocuments("fluffy -cat"s);
        assert(found_docs.size() == 1);
        assert(found_docs[0].id == 102);
    }
}

// Матчинг документов
void TestMatchedDocuments() {
    SearchServer server;
    server.SetStopWords("and in on"s);
    server.AddDocument(100, "fluffy cat and black dog in a collar"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
    {
        const auto [matched_words, status] = server.MatchDocument("dog and cat"s, 100);
        const vector<string> expected_result = { "cat"s, "dog"s };
        assert(expected_result == matched_words);
    }
    {
        const auto [matched_words, status] = server.MatchDocument("dog and -cat"s, 100);
        const vector<string> expected_result = {}; // пустой результат поскольку есть минус-слово
        assert(expected_result == matched_words);
        assert(matched_words.empty());
    }
}

// Сортировка найденных документов по релевантности
void TestSortResultsByRelevance() {
    SearchServer server;
    server.AddDocument(100, "fluffy cat fluffy tail"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
    server.AddDocument(101, "fluffy dog"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
    server.AddDocument(102, "dog leather collar"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
    {
        const auto found_docs = server.FindTopDocuments("fluffy cat"s);
        assert(found_docs.size() == 2u);
        for (size_t i = 1; i < found_docs.size(); i++) {
            assert(found_docs[i - 1].relevance >= found_docs[i].relevance);
        }
    }
}

// Вычисление рейтинга документов
void TestCalculateDocumentRating() {
    SearchServer server;
    const vector<int> ratings = { 10, 11, 3 };
    const int average = (10 + 11 + 3) / 3;
    server.AddDocument(0, "fluffy cat fluffy tail"s, DocumentStatus::ACTUAL, ratings);
    {
        const auto found_docs = server.FindTopDocuments("fluffy cat"s);
        assert(found_docs.size() == 1u);
        assert(found_docs[0].rating == average);
    }
}

// Функция TestSearchServer является точкой входа для запуска тестов
void TestSearchServer() 
{
    TestFindAddedDocumentByDocumentWord();
    TestExcludeStopWordsFromAddedDocumentContent();
    TestExcludeDocumentsWithMinusWordsFromResults();
    TestMatchedDocuments();
    TestSortResultsByRelevance();
    TestCalculateDocumentRating();
}
