#include "tests.h"

#include <string>
#include <vector>
#include <numeric>
#include <functional>

#include "search_server.h"

// ���� ���������, ��� ��������� ������� ��������� ����-����� ��� ���������� ����������
void TestExcludeStopWordsFromAddedDocumentContent() {
    using namespace std;

    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = { 1, 2, 3 };

    // ������� ����������, ��� ����� �����, �� ��������� � ������ ����-����,
    // ������� ������ ��������
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    // ����� ����������, ��� ����� ����� �� �����, ��������� � ������ ����-����,
    // ���������� ������ ���������
    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT_HINT(server.FindTopDocuments("in"s).empty(), "Stop words must be excluded from documents"s);
    }
}

// ���� ���������, ���������� ����������. ����������� �������� ������ ���������� �� ���������� �������, ������� �������� ����� �� ���������.
void TestAddDocument() {
    using namespace std;

    const int doc_id1 = 2;
    const string content1 = "cat in the city"s;
    const vector<int> ratings1 = { 3, 1, -1 };

    const int doc_id2 = 7;
    const string content2 = "porco rosso the crimson pig on a plane"s;
    const vector<int> ratings2 = { 2, 5, 6 };

    const int doc_id3 = 9;
    const string content3 = "black cat kyle"s;
    const vector<int> ratings3 = { -3, 2, 8 };


    {
        SearchServer server;
        server.SetStopWords("and in on the"s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        const auto found_docs = server.FindTopDocuments("pig"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id2);
    }

    {
        SearchServer server;
        server.SetStopWords("and in on the"s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        const auto found_docs = server.FindTopDocuments("cat -black"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_EQUAL(found_docs[0].id, doc_id1);
    }

    {
        SearchServer server;
        server.SetStopWords("and in on the"s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        const auto found_docs = server.FindTopDocuments("cat -city"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_EQUAL(found_docs[0].id, doc_id3);
    }

    {
        SearchServer server;
        server.SetStopWords("and in on the"s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        const auto found_docs = server.FindTopDocuments("starling"s);
        ASSERT(found_docs.empty());
    }
}

// ���� ���������, ��������� ����-����. ����-����� ����������� �� ������ ����������.
void TestExcludeStopWords() {
    using namespace std;

    const int doc_id1 = 2;
    const string content1 = "cat in the city"s;
    const int doc_id2 = 7;
    const string content2 = "porco rosso the crimson pig on a plane"s;
    const vector<int> ratings1 = { 3, 1, -1 };
    const vector<int> ratings2 = { 2, 5, 6 };

    {
        SearchServer server;
        server.SetStopWords("and in on the"s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        const auto found_docs = server.FindTopDocuments("the"s);
        ASSERT(found_docs.empty());
    }
}

// ���� ���������, ��������� �����-����. ���������, ���������� �����-����� ���������� �������, �� ������ ���������� � ���������� ������.
void TestMinusWords() {
    using namespace std;

    const int doc_id1 = 2;
    const string content1 = "cat in the city"s;
    const vector<int> ratings1 = { 3, 1, -1 };

    const int doc_id2 = 7;
    const string content2 = "porco rosso the crimson pig on a plane"s;
    const vector<int> ratings2 = { 2, 5, 6 };

    const int doc_id3 = 9;
    const string content3 = "big city bright lights"s;
    const vector<int> ratings3 = { 4, -2, 5 };

    {
        SearchServer server;
        server.SetStopWords("and in on the"s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        const auto found_docs = server.FindTopDocuments("city -cat"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id3);
    }

    {
        SearchServer server;
        server.SetStopWords("and in on the"s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        const auto found_docs = server.FindTopDocuments("pig -plane"s);
        ASSERT(found_docs.empty());
    }
}

// ���� ���������, ������� ����������. ��� �������� ��������� �� ���������� ������� ������ ����
// ���������� ��� ����� �� ���������� �������, �������������� � ���������. ���� ���� ������������
// ���� �� �� ������ �����-�����, ������ ������������ ������ ������ ����.
void TestMatch() {
    using namespace std;

    const int doc_id1 = 2;
    const string content1 = "big cat in the city"s;
    const vector<int> ratings1 = { 3, 1, -1 };

    const int doc_id2 = 7;
    const string content2 = "porco rosso the crimson pig on a plane"s;
    const vector<int> ratings2 = { 2, 5, 6 };

    const int doc_id3 = 9;
    const string content3 = "big city bright lights"s;
    const vector<int> ratings3 = { 4, -2, 5 };

    {
        SearchServer server;
        server.SetStopWords("and in on the"s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);

        const auto [words, status] = server.MatchDocument("big cat"s, doc_id1);
        ASSERT_EQUAL(words.size(), 2u);
        ASSERT((words[0] == "cat"s && words[1] == "big"s) || (words[1] == "cat"s && words[0] == "big"s));
    }

    {
        SearchServer server;
        server.SetStopWords("and in on the"s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);

        const auto [words, status] = server.MatchDocument("city cat"s, doc_id1);
        ASSERT_EQUAL(words.size(), 2u);
        ASSERT((words[0] == "cat"s && words[1] == "city"s) || (words[0] == "city"s && words[1] == "cat"s));
    }

    {
        SearchServer server;
        server.SetStopWords("and in on the"s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        const auto [words, status] = server.MatchDocument("the big -cat"s, doc_id1);
        ASSERT(words.empty());
    }
}

// ���� ���������, ���������� ��������� ���������� �� �������������. ������������ ��� ������
// ���������� ���������� ������ ���� ������������� � ������� �������� �������������.
void TestRelevanceSort() {
    using namespace std;

    const int doc_id1 = 2;
    const string content1 = "white cat and fashion collar"s;
    const vector<int> ratings1 = { 8, -3 };

    const int doc_id2 = 7;
    const string content2 = "fluffy cat fluffy tail"s;
    const vector<int> ratings2 = { 7, 2, 7 };

    const int doc_id3 = 9;
    const string content3 = "groomed dog expressive eyes"s;
    const vector<int> ratings3 = { 5, -12, 2, 1 };

    const int doc_id4 = 10;
    const string content4 = "groomed starling evgen"s;
    const vector<int> ratings4 = { 9 };

    {
        SearchServer server;
        server.SetStopWords("and in on the with"s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        server.AddDocument(doc_id4, content4, DocumentStatus::BANNED, ratings4);

        const auto found_docs = server.FindTopDocuments("fluffy groomed cat with collar"s);
        ASSERT_EQUAL(found_docs.size(), 3u);
        ASSERT_EQUAL(found_docs[0].id, doc_id2);
        ASSERT_EQUAL(found_docs[1].id, doc_id1);
        ASSERT_EQUAL(found_docs[2].id, doc_id3);

        for (size_t i = 1; i < found_docs.size(); ++i) {
            ASSERT(found_docs[i - 1].relevance >= found_docs[i].relevance);
        }
    }
}

// ���� ���������, ���������� �������� ����������. ������� ������������
// ��������� ����� �������� ��������������� ������ ���������.
void TestRating() {
    using namespace std;

    const int doc_id1 = 2;
    const string content1 = "white cat and fashion collar"s;
    const vector<int> ratings1 = { 8, -3 };

    const int doc_id2 = 7;
    const string content2 = "fluffy cat fluffy tail"s;
    const vector<int> ratings2 = { 257, 26, 769 };

    const int doc_id3 = 9;
    const string content3 = "groomed dog expressive eyes"s;
    const vector<int> ratings3 = { 75698, -12359, 28964, 13654 };

    const int doc_id4 = 10;
    const string content4 = "groomed starling evgen"s;
    const vector<int> ratings4 = { 9 };

    const int doc_id5 = 11;
    const string content5 = "red spider peter with black abdomen"s;
    const vector<int> ratings5;

    {
        SearchServer server;
        server.SetStopWords("and in on the with"s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        server.AddDocument(doc_id4, content4, DocumentStatus::BANNED, ratings4);
        server.AddDocument(doc_id5, content5, DocumentStatus::ACTUAL, ratings5);

        const auto found_docs = server.FindTopDocuments("white cat -fluffy"s);
        int rating_sum = accumulate(ratings1.cbegin(), ratings1.cend(), 0);
        int r = rating_sum / static_cast<int>(ratings1.size());
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_EQUAL(found_docs[0].rating, r);
    }

    {
        SearchServer server;
        server.SetStopWords("and in on the with"s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        server.AddDocument(doc_id4, content4, DocumentStatus::BANNED, ratings4);
        server.AddDocument(doc_id5, content5, DocumentStatus::ACTUAL, ratings5);

        const auto found_docs = server.FindTopDocuments("spider"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_EQUAL(found_docs[0].rating, 0);
    }
}

// ���� ���������, ���������� ����������� ������ � �������������� ���������, ����������� �������������.
void TestLambdaFiltering() {
    using namespace std;

    const int doc_id1 = 2;
    const string content1 = "white cat and fashion collar"s;
    const vector<int> ratings1 = { 8, -3 };

    const int doc_id2 = 7;
    const string content2 = "fluffy cat fluffy tail"s;
    const vector<int> ratings2 = { 7, 2, 7 };

    const int doc_id3 = 9;
    const string content3 = "groomed dog expressive eyes"s;
    const vector<int> ratings3 = { 5, -12, 2, 1 };

    const int doc_id4 = 10;
    const string content4 = "groomed starling evgen"s;
    const vector<int> ratings4 = { 9 };

    {
        SearchServer server;
        server.SetStopWords("and in on the with"s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        server.AddDocument(doc_id4, content4, DocumentStatus::BANNED, ratings4);

        const auto found_docs = server.FindTopDocuments("fluffy groomed cat with collar"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::ACTUAL && rating < 0 && document_id == 9; });
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT_EQUAL(found_docs[0].id, doc_id3);
    }
}

// ���� ���������, ����� ����������, ������� �������� ������.
void TestFilteringStatus() {
    using namespace std;

    const int doc_id1 = 2;
    const string content1 = "white cat and fashion collar"s;
    const vector<int> ratings1 = { 8, -3 };

    const int doc_id2 = 7;
    const string content2 = "fluffy cat fluffy tail"s;
    const vector<int> ratings2 = { 7, 2, 7 };

    const int doc_id3 = 9;
    const string content3 = "groomed dog expressive eyes"s;
    const vector<int> ratings3 = { 5, -12, 2, 1 };

    const int doc_id4 = 10;
    const string content4 = "groomed starling evgen"s;
    const vector<int> ratings4 = { 9 };

    const int doc_id6 = 15;
    const string content6 = "black bat wayne with black ears"s;
    const vector<int> ratings6 = { -3, 8, 4 };

    const int doc_id7 = 16;
    const string content7 = "red spider peter with black abdomen"s;
    const vector<int> ratings7 = { 2, 1, 6 };

    {
        SearchServer server;
        server.SetStopWords("and in on the with"s);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        server.AddDocument(doc_id4, content4, DocumentStatus::BANNED, ratings4);
        server.AddDocument(doc_id6, content6, DocumentStatus::REMOVED, ratings6);
        server.AddDocument(doc_id7, content7, DocumentStatus::IRRELEVANT, ratings7);

        const auto found_docs1 = server.FindTopDocuments("evgen"s, DocumentStatus::BANNED);
        ASSERT_EQUAL(found_docs1.size(), 1u);
        ASSERT_EQUAL(found_docs1[0].id, doc_id4);

        const auto found_docs2 = server.FindTopDocuments("wayne"s, DocumentStatus::REMOVED);
        ASSERT_EQUAL(found_docs2.size(), 1u);
        ASSERT_EQUAL(found_docs2[0].id, doc_id6);

        const auto found_docs3 = server.FindTopDocuments("peter"s, DocumentStatus::IRRELEVANT);
        ASSERT_EQUAL(found_docs3.size(), 1u);
        ASSERT_EQUAL(found_docs3[0].id, doc_id7);
    }
}

// ���� ���������, ���������� ���������� ������������� ��������� ����������.
void TestRelevance() {
    using namespace std;

    const int doc_id1 = 2;
    const string content1 = "white cat and fashion collar"s;
    const vector<int> ratings1 = { 8, -3 };

    const int doc_id2 = 7;
    const string content2 = "fluffy cat fluffy tail"s;
    const vector<int> ratings2 = { 7, 2, 7 };

    const int doc_id3 = 9;
    const string content3 = "groomed dog expressive eyes"s;
    const vector<int> ratings3 = { 5, -12, 2, 1 };

    const int doc_id4 = 10;
    const string content4 = "groomed starling evgen"s;
    const vector<int> ratings4 = { 9 };

    const int doc_id5 = 13;
    const string content5 = "black penguin oswald with black collar"s;
    const vector<int> ratings5 = { 7, 3, 8 };

    const int doc_id6 = 15;
    const string content6 = "black bat wayne with black ears"s;
    const vector<int> ratings6 = { -3, 8, 4 };

    const int doc_id7 = 16;
    const string content7 = "red spider peter with black abdomen"s;
    const vector<int> ratings7 = { 2, 1, 6 };

    {
        SearchServer server;
        string stopWords = "and in on the with"s;
        server.SetStopWords(stopWords);
        server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        server.AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        server.AddDocument(doc_id4, content4, DocumentStatus::BANNED, ratings4);
        server.AddDocument(doc_id5, content5, DocumentStatus::REMOVED, ratings5);
        server.AddDocument(doc_id6, content6, DocumentStatus::REMOVED, ratings6);
        server.AddDocument(doc_id7, content7, DocumentStatus::IRRELEVANT, ratings7);

        auto double_equals = [](double a, double b, double epsilon = 1e-6) {
            return std::abs(a - b) < epsilon;
        };

        //2     white cat and fashion collar
        //7     fluffy cat fluffy tail
        //9     groomed dog expressive eyes
        //10    groomed starling evgen
        //15    black bat wayne with black ears
        //16    red spider peter with black abdomen

        //N     Name        id/TF
        //==================================
        //1     abdomen	    {16|0,20}
        //2     bat	        {15|0,20}
        //3     black 	    {15|0,40}   {16/0,20}
        //4     cat	        { 2|0,25} 	{ 7/0,25}
        //5     collar	    { 2|0,25}
        //6     dog	        { 9|0,25}
        //7     ears	    {15|0,20}
        //8     evgen	    {10|0,33}
        //9     expressive  { 9|0,25}
        //10    eyes	    { 9|0,25}
        //11    fashion	    { 2|0,25}
        //12    fluffy	    { 7|0,50}
        //13    groomed	    { 9|0,25} 	{10/0,33}
        //14    peter	    {16|0,20}
        //15    red	        {16|0,20}
        //16    spider	    {16|0,20}
        //17    starling  	{10|0,33}
        //18    tail	    { 7|0,25}
        //19    wayne	    {15|0,20}
        //20    white	    { 2|0,25}

        //fluffy groomed cat with collar

        double totalDocCount = 7.0;

        //12 fluffy 7
        double fluffyDocCount = 1.0f;
        double fluffyIDF = log(totalDocCount * 1.0 / fluffyDocCount);

        //13 groomed 9 10
        double groomedDocCount = 2.0f;
        double groomedIDF = log(totalDocCount * 1.0 / groomedDocCount);

        //4 cat 2 7
        double catDocCount = 2.0f;
        double catIDF = log(totalDocCount * 1.0 / catDocCount);

        //5 collar 2
        double collarDocCount = 1.0f;
        double collarIDF = log(totalDocCount * 1.0 / collarDocCount);

        double relevance = groomedIDF * 0.25;
        const auto found_docs = server.FindTopDocuments("fluffy groomed cat with collar"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::ACTUAL && rating < 0 && document_id == 9; });
        ASSERT_EQUAL(found_docs.size(), 1u);
        ASSERT(double_equals(found_docs[0].relevance, relevance));


        set<string> stop_words_;
        map<string, map<int, double>> word_to_document_freqs_;
        map<int, pair<int, DocumentStatus>> documents_;

        auto SplitIntoWords = [](const string& text) {
            vector<string> words;
            string word;
            for (const char c : text) {
                if (c == ' ') {
                    words.push_back(word);
                    word = "";
                }
                else {
                    word += c;
                }
            }
            words.push_back(word);

            return words;
        };

        for (const string& word : SplitIntoWords(stopWords)) {
            stop_words_.insert(word);
        }

        auto IsStopWord = [&](const string& word) {
            return stop_words_.count(word) > 0;
        };

        auto SplitIntoWordsNoStop = [&](const string& text) {
            vector<string> words;
            for (const string& word : SplitIntoWords(text)) {
                if (!IsStopWord(word)) {
                    words.push_back(word);
                }
            }
            return words;
        };

        auto ComputeAverageRating = [](const vector<int>& ratings) {
            int rating_sum = accumulate(ratings.cbegin(), ratings.cend(), 0);
            return rating_sum / static_cast<int>(ratings.size());
        };

        auto AddDocument = [&](int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) {
            const vector<string> words = SplitIntoWordsNoStop(document);
            const double inv_word_count = 1.0 / words.size();
            for (const string& word : words) {
                if (word_to_document_freqs_[word].count(document_id) == 0) {
                    word_to_document_freqs_[word][document_id] = 0.0;
                }
                word_to_document_freqs_[word][document_id] += inv_word_count;
            }
            documents_.emplace(document_id, make_pair(ComputeAverageRating(ratings), status));
        };

        AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
        AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
        AddDocument(doc_id3, content3, DocumentStatus::ACTUAL, ratings3);
        AddDocument(doc_id4, content4, DocumentStatus::BANNED, ratings4);
        AddDocument(doc_id5, content5, DocumentStatus::REMOVED, ratings5);
        AddDocument(doc_id6, content6, DocumentStatus::REMOVED, ratings6);
        AddDocument(doc_id7, content7, DocumentStatus::IRRELEVANT, ratings7);

        auto ParseQueryWord = [&](string text) {
            bool is_minus = false;
            // Word shouldn't be empty
            if (text[0] == '-') {
                is_minus = true;
                text = text.substr(1);
            }
            return make_tuple(text, is_minus, IsStopWord(text));
        };

        auto ParseQuery = [&](const string& text) {
            pair<set<string>, set<string>> query;
            for (const string& word : SplitIntoWords(text)) {
                string data;
                bool is_minus;
                bool is_stop;
                tie(data, is_minus, is_stop) = ParseQueryWord(word);
                if (!is_stop) {
                    if (is_minus) {
                        query.second.insert(data);
                    }
                    else {
                        query.first.insert(data);
                    }
                }
            }
            return query;
        };

        auto GetDocumentCount = [&]() {
            return documents_.size();
        };

        auto ComputeWordInverseDocumentFreq = [&](const string& word) {
            return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
        };

        auto FindAllDocuments = [&](const pair<set<string>, set<string>>& query, function<bool(int document_id, DocumentStatus status, int rating)> fn) {
            map<int, double> document_to_relevance;
            for (const string& word : query.first) {
                if (word_to_document_freqs_.count(word) == 0) {
                    continue;
                }
                const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
                for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                    if (fn(document_id, documents_.at(document_id).second, documents_.at(document_id).first)) {
                        document_to_relevance[document_id] += term_freq * inverse_document_freq;
                    }
                }
            }

            for (const string& word : query.second) {
                if (word_to_document_freqs_.count(word) == 0) {
                    continue;
                }
                for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
                    document_to_relevance.erase(document_id);
                }
            }

            vector<Document> matched_documents;
            for (const auto [document_id, relevance] : document_to_relevance) {
                matched_documents.push_back({
                    document_id,
                    relevance,
                    documents_.at(document_id).first
                });
            }
            return matched_documents;
        };

        auto FindTopDocuments = [&](const string& raw_query, function<bool(int document_id, DocumentStatus status, int rating)> fn) {
            const pair<set<string>, set<string>> query = ParseQuery(raw_query);
            vector<Document> matched_documents = FindAllDocuments(query, fn);
            sort(
                matched_documents.begin(),
                matched_documents.end(),
                [](const Document& lhs, const Document& rhs) {
                return lhs.relevance > rhs.relevance || (abs(lhs.relevance - rhs.relevance) < 1e-6 && lhs.rating > rhs.rating);
            }
            );
            if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
                matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
            }
            return matched_documents;
        };

        const auto found_docs1 = server.FindTopDocuments("fluffy groomed cat with collar"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::ACTUAL && rating < 0 && document_id == 9; });
        const auto found_docs2 = FindTopDocuments("fluffy groomed cat with collar"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::ACTUAL && rating < 0 && document_id == 9; });

        ASSERT_EQUAL(found_docs1.size(), found_docs2.size());
        ASSERT(double_equals(found_docs1[0].relevance, found_docs2[0].relevance));

        const auto found_docs3 = server.FindTopDocuments("fluffy groomed cat with collar"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::ACTUAL; });
        const auto found_docs4 = FindTopDocuments("fluffy groomed cat with collar"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::ACTUAL; });

        ASSERT_EQUAL(found_docs3.size(), found_docs3.size());
        ASSERT(double_equals(found_docs3[0].relevance, found_docs4[0].relevance));
        ASSERT(double_equals(found_docs3[1].relevance, found_docs4[1].relevance));
        ASSERT(double_equals(found_docs3[2].relevance, found_docs4[2].relevance));

        const auto found_docs5 = server.FindTopDocuments("fluffy groomed cat with collar"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::BANNED; });
        const auto found_docs6 = FindTopDocuments("fluffy groomed cat with collar"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::BANNED; });

        ASSERT_EQUAL(found_docs5.size(), found_docs6.size());
        ASSERT(double_equals(found_docs5[0].relevance, found_docs6[0].relevance));

        const auto found_docs7 = server.FindTopDocuments("penguin"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::REMOVED; });
        const auto found_docs8 = FindTopDocuments("penguin"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::REMOVED; });

        ASSERT_EQUAL(found_docs7.size(), found_docs8.size());
        ASSERT(found_docs7[0].id == found_docs8[0].id);
        ASSERT(double_equals(found_docs7[0].relevance, found_docs8[0].relevance));

        const auto found_docs9 = server.FindTopDocuments("spider"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::IRRELEVANT; });
        const auto found_docs10 = FindTopDocuments("spider"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::IRRELEVANT; });

        ASSERT_EQUAL(found_docs9.size(), found_docs10.size());
        ASSERT(found_docs9[0].id == found_docs10[0].id);
        ASSERT(double_equals(found_docs9[0].relevance, found_docs10[0].relevance));
    }
}

// ������� TestSearchServer �������� ������ ����� ��� ������� ������
void TestSearchServer() {
    RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent);
    RUN_TEST(TestAddDocument);
    RUN_TEST(TestExcludeStopWords);
    RUN_TEST(TestMinusWords);
    RUN_TEST(TestMatch);
    RUN_TEST(TestRelevanceSort);
    RUN_TEST(TestRating);
    RUN_TEST(TestLambdaFiltering);
    RUN_TEST(TestFilteringStatus);
    RUN_TEST(TestRelevance);
}