#pragma once

template <class...>
struct make_void { using type = void; };

template <typename... T>
using void_t = typename make_void<T...>::type; // force SFINAE

namespace detail {
    template<typename AlwaysVoid, template<typename...> class Operator, typename ...Args>
    struct _is_valid : std::false_type {};


    template<template<typename...> class Operator, typename ...Args>
    struct _is_valid<void_t<Operator<Args...>>, Operator, Args...> : std::true_type { using type = Operator<Args...>; };
}

template<template<typename ...> class Operator, typename ...Args>
using is_valid = detail::_is_valid<void, Operator, Args...>;

template<template<typename ...> class Operator, typename ...Args>
constexpr bool is_valid_v = is_valid<Operator, Args...>::value;

#define HAS_MEMBER(name, ...)\
template<typename T>\
using _has_##name = decltype(std::declval<T>().name(__VA_ARGS__));\
\
template<typename T>\
using has_##name = is_valid<_has_##name, T>;\
\
template<typename T>\
constexpr bool has_##name##_v = has_##name<T>::value

HAS_MEMBER(push_back, std::declval<typename T::value_type>());
HAS_MEMBER(begin);
HAS_MEMBER(end);

template<typename T>
using is_iterable = std::conditional_t<has_begin_v<T> && has_end_v<T>, std::true_type, std::false_type>;

template<typename T>
constexpr bool is_iterable_v = is_iterable<T>::value;