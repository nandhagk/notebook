#ifndef LIB_NUMERIC_TRAITS_HPP
#define LIB_NUMERIC_TRAITS_HPP 1

#include <type_traits>

#include <lib/prelude.hpp>

template <typename T>
constexpr bool is_integral_v = std::is_integral_v<T> || std::is_same_v<T, i128> || std::is_same_v<T, u128>;

template <typename T>
using is_integral_t = std::enable_if_t<is_integral_v<T>>;

template <typename T>
constexpr bool is_signed_integral_v = is_integral_v<T> && std::is_signed_v<T>;

template <typename T>
constexpr bool is_unsigned_integral_v = is_integral_v<T> && std::is_unsigned_v<T>;

template <typename T>
using is_signed_integral_t = std::enable_if_t<is_signed_integral_v<T>>;

template <typename T>
using is_unsigned_integral_t = std::enable_if_t<is_unsigned_integral_v<T>>;

template <typename T>
using make_signed =
    std::conditional_t<std::is_same_v<T, u128>, i128, std::conditional_t<std::is_same_v<T, u64>, i64, i32>>;

template <typename T>
using make_double = std::conditional_t<std::is_same_v<T, u64>, u128, u64>;

#endif // LIB_NUMERIC_TRAITS_HPP
