#pragma once

#include <iostream>
#include <utility>
#include <vector>
#include <set>
#include <map>

template<typename Key, typename Value>
std::ostream& operator<<(std::ostream& out, const std::pair<Key, Value>& container) {
    out << container.first;
    out << ": ";
    out << container.second;
    return out;
}

template<typename T>
void Print(std::ostream& out, T container) {

    bool first = true;
    for (const auto& element : container) {
        if (first) {
            out << element;
            first = false;
        }
        else {
            out << ", " << element;
        }
    }
}

template<typename Element>
std::ostream& operator<<(std::ostream& out, const std::vector<Element>& container) {
    out << "[";
    Print(out, container);
    out << "]";
    return out;
}

template<typename Element>
std::ostream& operator<<(std::ostream& out, const std::set<Element>& container) {
    out << "{";
    Print(out, container);
    out << "}";
    return out;
}

template<typename Key, typename Value>
std::ostream& operator<<(std::ostream& out, const std::map<Key, Value>& container) {
    out << "{";
    Print(out, container);
    out << "}";
    return out;
}

template <typename T, typename U>
void AssertEqualImpl(const T& t, const U& u, const std::string& t_str, const std::string& u_str, const std::string& file, const std::string& func, unsigned line, const std::string& hint) {
    using namespace std;
    if (t != u) {
        cerr << boolalpha;
        cerr << file << "("s << line << "): "s << func << ": "s;
        cerr << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s;
        cerr << t << " != "s << u << "."s;
        if (!hint.empty()) {
            cerr << " Hint: "s << hint;
        }
        cerr << endl;
        abort();
    }
}

template <typename K, typename V>
void AssertEqualImpl(const std::map<K, V>& t, const std::map<K, V>& u, const std::string& t_str, const std::string& u_str, const std::string& file, const std::string& func, unsigned line, const std::string& hint) {
    using namespace std;
    if (!equal(t.cbegin(), t.cend(), u.cbegin())) {
        cerr << boolalpha;
        cerr << file << "("s << line << "): "s << func << ": "s;
        cerr << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s;
        cerr << t << " != "s << u << "."s;
        if (!hint.empty()) {
            cerr << " Hint: "s << hint;
        }
        cerr << endl;
        abort();
    }
}

template <typename T>
void AssertEqualImpl(const std::set<T>& t, const std::set<T>& u, const std::string& t_str, const std::string& u_str, const std::string& file, const std::string& func, unsigned line, const std::string& hint) {
    using namespace std;
    if (!equal(t.cbegin(), t.cend(), u.cbegin())) {
        cerr << boolalpha;
        cerr << file << "("s << line << "): "s << func << ": "s;
        cerr << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s;
        cerr << t << " != "s << u << "."s;
        if (!hint.empty()) {
            cerr << " Hint: "s << hint;
        }
        cerr << endl;
        abort();
    }
}

template <typename T>
void AssertEqualImpl(const std::vector<T>& t, const std::vector<T>& u, const std::string& t_str, const std::string& u_str, const std::string& file, const std::string& func, unsigned line, const std::string& hint) {
    using namespace std;
    if (!equal(t.cbegin(), t.cend(), u.cbegin())) {
        cerr << boolalpha;
        cerr << file << "("s << line << "): "s << func << ": "s;
        cerr << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s;
        cerr << t << " != "s << u << "."s;
        if (!hint.empty()) {
            cerr << " Hint: "s << hint;
        }
        cerr << endl;
        abort();
    }
}

#define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, ""s)

#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint))

void AssertImpl(bool value, const std::string& expr_str, const std::string& file, const std::string& func, unsigned line, const std::string& hint);

#define ASSERT(expr) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, ""s)

#define ASSERT_HINT(expr, hint) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, (hint))

template <typename FX>
void RunTestImpl(FX fn, const std::string& fnName) {
    fn();
    std::cerr << fnName << " OK" << std::endl;
}

#define RUN_TEST(func) RunTestImpl((func), #func)