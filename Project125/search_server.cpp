#include "search_server.h"
#include "string_processing.h"

#include <numeric>

SearchServer::SearchServer(const std::string& stopWordsText) {
    SetStopWords(stopWordsText);
}

void SearchServer::SetStopWords(const std::string& stopWordsText) {
    std::set<std::string> temp;
    split(temp, stopWordsText);
    SetStopWords(temp);
}

void SearchServer::AddDocument(int documentId, const std::string& document, DocumentStatus status, const std::vector<int>& ratings) {
    if (documentId < 0 || documents_.find(documentId) != documents_.end()) {
        throw std::invalid_argument("Bad document id");
    }
    std::vector<std::string> words;
    if (!splitIntoWordsNoStop(document, words)) {
        throw std::invalid_argument("Bad document data");
    };
    documents_.emplace(documentId, DocumentData{ computeAverageRating(ratings), status, words });
    document_ids_.push_back(documentId);
    calculateTermFrequency(documentId);
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string& rawQuery, DocumentStatus status) const {
    return FindTopDocuments(rawQuery, [status](int document_id, DocumentStatus document_status, int rating) {
        return document_status == status;
    });
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string& rawQuery) const {
    return FindTopDocuments(rawQuery, DocumentStatus::ACTUAL);
}

int SearchServer::GetDocumentCount() const {
    return documents_.size();
}

 int SearchServer::GetDocumentId(int index) const {
     if (index >= 0 && index < GetDocumentCount()) {
         return document_ids_[index];
     }
     throw std::out_of_range("document index not present");
 }

 std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::string& rawQuery, int documentId) const {
     Query query;
     if (!parseQuery(rawQuery, query)) {
         throw std::invalid_argument("Bad query");
     }
     std::vector<std::string> matched_words;
     for (const std::string& word : query.plus_words) {
         if (word_to_document_freqs_.count(word) == 0) {
             continue;
         }
         if (word_to_document_freqs_.at(word).count(documentId)) {
             matched_words.push_back(word);
         }
     }
     for (const std::string& word : query.minus_words) {
         if (word_to_document_freqs_.count(word) == 0) {
             continue;
         }
         if (word_to_document_freqs_.at(word).count(documentId)) {
             matched_words.clear();
             break;
         }
     }
     return { matched_words, documents_.at(documentId).status };
 }


 [[nodiscard]]
 bool SearchServer::parseQuery(const std::string& text, Query& query) const {
     std::vector<std::string> words;
     split(words, text);
     for (const std::string& word : words) {
         QueryWord query_word;
         if (!parseQueryWord(word, query_word)) {
             return false;
         }
         if (!query_word.is_stop) {
             if (query_word.is_minus) {
                 query.minus_words.insert(query_word.data);
             }
             else {
                 query.plus_words.insert(query_word.data);
             }
         }
     }
     return true;
 }

 [[nodiscard]]
 bool SearchServer::parseQueryWord(std::string text, QueryWord& qw) const {
     if (text.empty()) {
         return false;
     }
     bool is_minus = false;
     if (text[0] == '-') {
         is_minus = true;
         text = text.substr(1);
     }
     if (!isValidWord(text)) {
         return false;
     }
     qw = { text, is_minus, isStopWord(text) };
     return true;
 }

 bool SearchServer::addDocumentData(int document_id, const std::vector<std::string>& words, DocumentStatus status, const std::vector<int>& ratings) {
     if (documents_.count(document_id) == 0) {
         documents_.emplace(document_id, DocumentData{ computeAverageRating(ratings), status, words });
         return true;
     }
     return false;
 }

int SearchServer::computeAverageRating(const std::vector<int>& ratings) {
    int ratingsCount = static_cast<int>(ratings.size());
    if (ratingsCount == 0) {
        return 0;
    }
    else {
        int rating_sum = std::accumulate(ratings.cbegin(), ratings.cend(), 0);
        return rating_sum / static_cast<int>(ratings.size());
    }
}

 void SearchServer::calculateTermFrequency(int documentId) {
     const std::vector<std::string>& words = documents_[documentId].words;
     const double inv_word_count = 1.0 / words.size();
     for (const std::string& word : words) {
         if (word_to_document_freqs_[word].count(documentId) == 0) {
             word_to_document_freqs_[word][documentId] = 0.0;
         }
         word_to_document_freqs_[word][documentId] += inv_word_count;
     }
 }

bool SearchServer::splitIntoWordsNoStop(const std::string& text, std::vector<std::string>& words) const {
    std::vector<std::string> tempWords;
    split(tempWords, text);
    for (const std::string& word : tempWords) {
        if (!isValidWord(word)) {
            return false;
        }
        if (!isStopWord(word)) {
            words.push_back(word);
        }
    }
    return true;
}

bool SearchServer::isValidChar(char character, bool isFirst) {
    if (character < 0) {
        return true;
    }
    if (iscntrl(character)) {
        return false;
    }
    if (isFirst && isalpha(character)) {
        return true;
    }
    if (!isFirst && isgraph(character)) {
        return true;
    }
    return false;
}

bool SearchServer::isValidWord(const std::string& word) {
    if (word.empty()) {
        return false;
    }
    for (auto [it, end, i] = its_and_idx(word); it != end; ++it, ++i) {
        if (!isValidChar(*it, i == 0)) {
            return false;
        }
    }
    return true;
}

bool SearchServer::isStopWord(const std::string& word) const {
    return m_stopWords.count(word) > 0;
}

double SearchServer::computeWordInverseDocumentFreq(const std::string& word) const {
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}

void PrintDocument(const Document& document) {
    using namespace std;
    cout << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating << " }"s << endl;
}

void PrintMatchDocumentResult(int document_id, const std::vector<std::string>& words, DocumentStatus status) {
    using namespace std;
    cout << "{ "s
        << "document_id = "s << document_id << ", "s
        << "status = "s << static_cast<int>(status) << ", "s
        << "words ="s;
    for (const string& word : words) {
        cout << ' ' << word;
    }
    cout << "}"s << endl;
}

void AddDocument(SearchServer& search_server, int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings) {
    using namespace std;
    try {
        search_server.AddDocument(document_id, document, status, ratings);
    }
    catch (const exception& e) {
        cout << "Ошибка добавления документа "s << document_id << ": "s << e.what() << endl;
    }
}

void FindTopDocuments(const SearchServer& search_server, const std::string& raw_query) {
    using namespace std;
    cout << "Результаты поиска по запросу: "s << raw_query << endl;
    try {
        for (const Document& document : search_server.FindTopDocuments(raw_query)) {
            PrintDocument(document);
        }
    }
    catch (const exception& e) {
        cout << "Ошибка поиска: "s << e.what() << endl;
    }
}

void MatchDocuments(const SearchServer& search_server, const std::string& query) {
    using namespace std;
    try {
        cout << "Матчинг документов по запросу: "s << query << endl;
        const int document_count = search_server.GetDocumentCount();
        for (int index = 0; index < document_count; ++index) {
            const int document_id = search_server.GetDocumentId(index);
            const auto [words, status] = search_server.MatchDocument(query, document_id);
            PrintMatchDocumentResult(document_id, words, status);
        }
    }
    catch (const exception& e) {
        cout << "Ошибка матчинга документов на запрос "s << query << ": "s << e.what() << endl;
    }
}