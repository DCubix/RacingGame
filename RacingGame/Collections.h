#ifndef COLLECTIONS_H
#define COLLECTIONS_H

#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <string>
#include <iterator>

#include "Int.h"

template <typename T> using Vec = std::vector<T>;
template <typename T, u32 N> using Array = std::array<T, N>;
template <typename K, typename V> using Map = std::map<K, V>;
template <typename K, typename V> using UMap = std::unordered_map<K, V>;
using String = std::string;

#define ToVector(type, arr) Vec<type>(std::begin(arr), std::end(arr))

#endif