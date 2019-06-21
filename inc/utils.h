#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <initializer_list>
#include <iterator>

template <typename T, typename container = std::initializer_list<T>>
auto contains(container const& cnt, T const& t) -> bool
{
    return std::find(std::begin(cnt), std::end(cnt), t) != std::end(cnt);
}

#endif // UTILS_H
