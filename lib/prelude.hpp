#ifndef LIB_PRELUDE_HPP
#define LIB_PRELUDE_HPP 1

#include <cstdint>

using i32 = std::int32_t;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;
using i128 = __int128;
using u128 = unsigned __int128;
using isize = std::ptrdiff_t;
using usize = std::size_t;

using f32 = float;
using f64 = double;
using f80 = long double;

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

inline i32 popcnt(i32 x) {
    return __builtin_popcount(static_cast<u32>(x));
}

inline i32 popcnt(u32 x) {
    return __builtin_popcount(x);
}

inline i32 popcnt(i64 x) {
    return __builtin_popcountll(static_cast<u64>(x));
}

inline i32 popcnt(u64 x) {
    return __builtin_popcountll(x);
}

inline i32 topbit(i32 x) {
    return 31 - __builtin_clz(static_cast<u32>(x));
}

inline i32 topbit(u32 x) {
    return 31 - __builtin_clz(x);
}

inline i32 topbit(i64 x) {
    return 63 - __builtin_clzll(static_cast<u64>(x));
}

inline i32 topbit(u64 x) {
    return 63 - __builtin_clzll(x);
}

inline i32 lowbit(i32 x) {
    return __builtin_ctz(static_cast<u32>(x));
}

inline i32 lowbit(u32 x) {
    return __builtin_ctz(x);
}

inline i32 lowbit(i64 x) {
    return __builtin_ctzll(static_cast<u64>(x));
}

inline i32 lowbit(u64 x) {
    return __builtin_ctzll(x);
}

#endif // LIB_PRELUDE_HPP
