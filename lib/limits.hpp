#ifndef LIB_LIMITS_HPP
#define LIB_LIMITS_HPP 1

#include <lib/prelude.hpp>

template <typename T>
inline constexpr T inf = 0;

template <>
inline constexpr i32 inf<i32> = 1'010'000'000;
template <>
inline constexpr u32 inf<u32> = inf<i32>;
template <>
inline constexpr i64 inf<i64> = 2'020'000'000'000'000'000;
template <>
inline constexpr u64 inf<u64> = inf<i64>;
template <>
inline constexpr i128 inf<i128> = i128(inf<i64>) * 2'000'000'000'000'000'000;
template <>
inline constexpr u128 inf<u128> = inf<i128>;

template <>
inline constexpr f32 inf<f32> = inf<i32>;
template <>
inline constexpr f64 inf<f64> = inf<i64>;
template <>
inline constexpr f80 inf<f80> = inf<i64>;

#endif // LIB_LIMITS_HPP
