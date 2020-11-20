#pragma once

#include <iterator>
#include <tuple>

const double EPSILON = 1e-6;

template<typename C, typename T>
auto insert_in_container(C& c, T&& t) -> decltype(c.push_back(std::forward<T>(t)), void()) {
    c.push_back(std::forward<T>(t));
}

template<typename C, typename T>
auto insert_in_container(C& c, T&& t) -> decltype(c.insert(std::forward<T>(t)), void()) {
    c.insert(std::forward<T>(t));
}

template <typename Container>
auto its_and_idx(Container&& container) {
    return std::tuple{ std::begin(container), std::end(container), 0 };
}
