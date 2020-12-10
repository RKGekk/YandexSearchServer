#pragma once

#include <string>
#include <tuple>
#include <set>
#include <algorithm>
#include <stdexcept>
#include <map>
#include <cmath>

#include "document.h"
#include "string_processing.h"
#include "utility.h"

const int MAX_RESULT_DOCUMENT_COUNT = 5;

class SearchServer {
public:
    struct DocumentData {
        int rating;
        DocumentStatus status;
        std::vector<std::string> words;
    };

    typedef std::map<int, DocumentData> DocumentDataMap;
    typedef DocumentDataMap::iterator DocumentDataMapIterator;
    typedef DocumentDataMap::const_iterator DocumentDataMapConstIterator;

    class key_iterator : public DocumentDataMapIterator {
    public:
        key_iterator() : DocumentDataMapIterator() {};
        key_iterator(DocumentDataMapIterator s) : DocumentDataMapIterator(s) {};
        const int* operator->() { return &(DocumentDataMapIterator::operator->()->first); }
        int operator*() { return DocumentDataMapIterator::operator*().first; }
    };

    class key_const_iterator : public DocumentDataMapConstIterator {
    public:
        key_const_iterator() : DocumentDataMapConstIterator() {};
        key_const_iterator(DocumentDataMapConstIterator s) : DocumentDataMapConstIterator(s) {};
        const int* operator->() { return &(DocumentDataMapConstIterator::operator->()->first); }
        int operator*() { return DocumentDataMapConstIterator::operator*().first; }
    };

    key_iterator begin();
    key_iterator end();
    key_const_iterator cbegin() const;
    key_const_iterator cend() const;

    SearchServer() = default;

    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stopWords) {
        SetStopWords(stopWords);
    }

    explicit SearchServer(const std::string& stopWordsText);

    template <typename StringContainer>
    void SetStopWords(const StringContainer& stopWords) {
        for (const auto& stopWord : stopWords) {
            if (!isValidWord(stopWord)) {
                throw std::invalid_argument("Bad stop word");
            }
            m_stopWords.emplace(stopWord);
        }
    }

    void SetStopWords(const std::string& stopWordsText);

    void AddDocument(int documentId, const std::string& document, DocumentStatus status, const std::vector<int>& ratings);
    void RemoveDocument(int document_id);

    template <typename DocumentPredicate>
    std::vector<Document> FindTopDocuments(const std::string& rawQuery, const DocumentPredicate& document_predicate) const {
        Query query;
        if (!parseQuery(rawQuery, query)) {
            throw std::invalid_argument("Bad query");
        }
        std::vector<Document> matched_documents = findAllDocuments(query, document_predicate);

        std::sort(
            matched_documents.begin(),
            matched_documents.end(),
            [](const Document& lhs, const Document& rhs) {
            return lhs.relevance > rhs.relevance || (abs(lhs.relevance - rhs.relevance) < EPSILON && lhs.rating > rhs.rating);
        }
        );
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

    std::vector<Document> FindTopDocuments(const std::string& rawQuery, DocumentStatus status) const;
    std::vector<Document> FindTopDocuments(const std::string& rawQuery) const;

    const std::map<std::string, double>& GetWordFrequencies(int document_id) const;

    int GetDocumentCount() const;
    int GetDocumentId(int index) const;

    std::tuple<std::vector<std::string>, DocumentStatus> MatchDocument(const std::string& rawQuery, int documentId) const;

private:

    struct Query {
        std::set<std::string> plus_words;
        std::set<std::string> minus_words;
    };

    struct QueryWord {
        std::string data;
        bool is_minus;
        bool is_stop;
    };

    std::set<std::string> m_stopWords;
    std::map<std::string, std::map<int, double>> word_to_document_freqs_;
    std::map<int, std::map<std::string, double>> document_to_word_freqs_;
    std::map<int, DocumentData> documents_;
    std::set<int> document_ids_;
    std::map<std::string, double> emptyMap;

    [[nodiscard]]
    bool parseQuery(const std::string& text, Query& query) const;

    [[nodiscard]]
    bool parseQueryWord(std::string text, QueryWord& qw) const;

    bool addDocumentData(int document_id, const std::vector<std::string>& words, DocumentStatus status, const std::vector<int>& ratings);

    static int computeAverageRating(const std::vector<int>& ratings);

    void calculateTermFrequency(int documentId);

    bool splitIntoWordsNoStop(const std::string& text, std::vector<std::string>& words) const;

    static bool isValidChar(char character, bool isFirst);

    static bool isValidWord(const std::string& word);

    bool isStopWord(const std::string& word) const;

    template<typename Container>
    static void split(Container& out, const std::string& s, const std::string& delims = " \r\n\t\v") {
        auto begIdx = s.find_first_not_of(delims);
        while (begIdx != std::string::npos) {
            auto endIdx = s.find_first_of(delims, begIdx);
            if (endIdx == std::string::npos) {
                endIdx = s.length();
            }
            insert_in_container(out, s.substr(begIdx, endIdx - begIdx));
            begIdx = s.find_first_not_of(delims, endIdx);
        }
    }

    template <typename DocumentPredicate>
    std::vector<Document> findAllDocuments(const Query& query, DocumentPredicate document_predicate) const {
        std::map<int, double> document_to_relevance;
        for (const std::string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = computeWordInverseDocumentFreq(word);
            for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                const auto& document_data = documents_.at(document_id);
                if (document_predicate(document_id, document_data.status, document_data.rating)) {
                    document_to_relevance[document_id] += term_freq * inverse_document_freq;
                }
            }
        }

        for (const std::string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        std::vector<Document> matched_documents;
        for (const auto [document_id, relevance] : document_to_relevance) {
            matched_documents.push_back({
                document_id,
                relevance,
                documents_.at(document_id).rating
            });
        }
        return matched_documents;
    }

    double computeWordInverseDocumentFreq(const std::string& word) const;
};

void PrintDocument(const Document& document);
void PrintMatchDocumentResult(int document_id, const std::vector<std::string>& words, DocumentStatus status);
void AddDocument(SearchServer& search_server, int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings);
void FindTopDocuments(const SearchServer& search_server, const std::string& raw_query);
void MatchDocuments(const SearchServer& search_server, const std::string& query);