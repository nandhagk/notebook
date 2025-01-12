#ifndef LIB_TYPE_TRAITS_HPP
#define LIB_TYPE_TRAITS_HPP 1

#include <type_traits>

#include <lib/prelude.hpp>

template <typename T>
using is_integral_t = std::enable_if_t<std::is_integral_v<T>>;

template <typename T>
using is_signed_integral =
    std::conditional_t<std::is_integral_v<T> && std::is_signed_v<T>, std::true_type, std::false_type>;

template <typename T>
using is_unsigned_integral =
    std::conditional_t<std::is_integral_v<T> && std::is_unsigned_v<T>, std::true_type, std::false_type>;

template <typename T>
constexpr bool is_signed_integral_v = is_signed_integral<T>::value;

template <typename T>
constexpr bool is_unsigned_integral_v = is_unsigned_integral<T>::value;

template <typename T>
using is_signed_integral_t = std::enable_if_t<is_signed_integral_v<T>>;

template <typename T>
using is_unsigned_integral_t = std::enable_if_t<is_unsigned_integral_v<T>>;

template <typename T>
using make_signed =
    std::conditional_t<std::is_same_v<T, u128>, i128, std::conditional_t<std::is_same_v<T, u64>, i64, i32>>;

template <typename T>
using make_double = std::conditional_t<std::is_same_v<T, u64>, u128, u64>;

template <typename, typename = std::void_t<>>
struct has_op : std::false_type {};

template <typename T>
struct has_op<
    T,
    std::enable_if_t<std::is_same_v<
        decltype(T::op(std::declval<typename T::ValueT>(), std::declval<typename T::ValueT>())), typename T::ValueT>>>
    : std::true_type {};

template <typename T>
constexpr bool has_op_v = has_op<T>::value;

template <typename, typename = std::void_t<>>
struct has_unit : std::false_type {};

template <typename T>
struct has_unit<T, std::enable_if_t<std::is_same_v<decltype(T::unit()), typename T::ValueT>>> : std::true_type {};

template <typename T>
constexpr bool has_unit_v = has_unit<T>::value;

template <typename T>
using is_monoid = std::conditional_t<has_op_v<T> && has_unit_v<T>, std::true_type, std::false_type>;

template <typename T>
constexpr bool is_monoid_v = is_monoid<T>::value;

template <typename T>
using is_monoid_t = std::enable_if_t<is_monoid_v<T>>;

template <typename T>
using is_commutative_monoid = std::conditional_t<is_monoid_v<T> && T::commutative, std::true_type, std::false_type>;

template <typename T>
constexpr bool is_commutative_monoid_v = is_commutative_monoid<T>::value;

template <typename T>
using is_commutative_monoid_t = std::enable_if_t<is_commutative_monoid_v<T>>;

template <typename, typename = std::void_t<>>
struct has_inv : std::false_type {};

template <typename T>
struct has_inv<
    T, std::enable_if_t<std::is_same_v<decltype(T::inv(std::declval<typename T::ValueT>())), typename T::ValueT>>>
    : std::true_type {};

template <typename T>
constexpr bool has_inv_v = has_inv<T>::value;

template <typename T>
using is_group = std::conditional_t<is_monoid_v<T> && has_inv_v<T>, std::true_type, std::false_type>;

template <typename T>
constexpr bool is_group_v = is_group<T>::value;

template <typename T>
using is_group_t = std::enable_if_t<is_group_v<T>>;

template <typename T>
using is_abelian_group = std::conditional_t<is_group_v<T> && T::commutative, std::true_type, std::false_type>;

template <typename T>
constexpr bool is_abelian_group_v = is_abelian_group<T>::value;

template <typename T>
using is_abelian_group_t = std::enable_if_t<is_abelian_group_v<T>>;

template <typename, typename = std::void_t<>>
struct has_rev : std::false_type {};

template <typename T>
struct has_rev<
    T, std::enable_if_t<std::is_same_v<decltype(T::rev(std::declval<typename T::ValueT>())), typename T::ValueT>>>
    : std::true_type {};

template <typename T>
constexpr bool has_rev_v = has_rev<T>::value;

#endif // LIB_TYPE_TRAITS_HPP
