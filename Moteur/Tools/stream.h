#pragma once

#include <type_traits>
#include <iostream>
#include "void_t.h"
#include "glm.h"

template<typename A, typename B>
inline std::ostream &operator<<(std::ostream &out, std::pair<A, B> const &p) {
	out << p.first << " " << p.second << std::endl;
	return out;
}

template<typename T>
constexpr bool is_already_streamable_out = std::is_same_v<T, std::string>;

template<typename T>
inline std::enable_if_t<is_iterable_v<T> && !is_already_streamable_out<T>, std::ostream> &operator<<(std::ostream &out, T const &t) {
	for (auto const &e : t)
		out << e << ", ";
	out << std::endl;
	return out;
}

template<typename T, typename V>
inline std::enable_if_t<has_push_back_v<T> && !is_iterable_v<V>, T> &operator<<(T &c, V const &e) {
	c.push_back(e);
	return c;
}

template<typename T, typename V>
using is_streamable = decltype(std::declval<T&>() << std::declval<V>());

template<typename T, typename V>
inline std::enable_if_t<is_iterable_v<T> && is_iterable_v<V> && is_valid_v< is_streamable, T, typename V::value_type> , T> &operator<<(T &c, V &v) {
	for (auto &e : v)
		c << e;
	return c;
}

