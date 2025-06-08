#ifndef LIB_ALGEBRAIC_TRAITS_HPP
#define LIB_ALGEBRAIC_TRAITS_HPP 1

#include <type_traits>

#include <lib/prelude.hpp>

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
struct has_pow : std::false_type {};

template <typename T>
struct has_pow<
    T, std::enable_if_t<std::is_same_v<decltype(T::pow(std::declval<typename T::ValueT>(), 0)), typename T::ValueT>>>
    : std::true_type {};

template <typename T>
constexpr bool has_pow_v = has_pow<T>::value;

template <typename, typename = std::void_t<>>
struct has_rev : std::false_type {};

template <typename T>
struct has_rev<
    T, std::enable_if_t<std::is_same_v<decltype(T::rev(std::declval<typename T::ValueT>())), typename T::ValueT>>>
    : std::true_type {};

template <typename T>
constexpr bool has_rev_v = has_rev<T>::value;

template <typename, typename = std::void_t<>>
struct has_fail : std::false_type {};

template <typename T>
struct has_fail<T, std::enable_if_t<std::is_same_v<decltype(T::failed(std::declval<typename T::ValueT>())), bool>>>
    : std::true_type {};

template <typename T>
constexpr bool has_fail_v = has_fail<T>::value;

template <typename Monoid, is_monoid_t<Monoid> * = nullptr>
constexpr typename Monoid::ValueT monoid_pow(const typename Monoid::ValueT &a, i64 n) {
    using MX = Monoid;
    using X = typename MX::ValueT;

    if constexpr (has_pow_v<MX>) return MX::pow(a, n);

    X b = MX::unit();
    for (X c = a; n != 0; n >>= 1, c = MX::op(c, c))
        if (n & 1) b = MX::op(b, c);

    return b;
}

#endif // LIB_ALGEBRAIC_TRAITS_HPP
