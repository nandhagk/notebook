#ifndef LIB_BITS_HPP
#define LIB_BITS_HPP 1

#include <lib/prelude.hpp>

inline constexpr i32 popcnt(i32 x) {
    return __builtin_popcount(static_cast<u32>(x));
}

inline constexpr i32 popcnt(u32 x) {
    return __builtin_popcount(x);
}

inline constexpr i32 popcnt(i64 x) {
    return __builtin_popcountll(static_cast<u64>(x));
}

inline constexpr i32 popcnt(u64 x) {
    return __builtin_popcountll(x);
}

inline constexpr i32 topbit(i32 x) {
    return 31 - __builtin_clz(static_cast<u32>(x));
}

inline constexpr i32 topbit(u32 x) {
    return 31 - __builtin_clz(x);
}

inline constexpr i32 topbit(i64 x) {
    return 63 - __builtin_clzll(static_cast<u64>(x));
}

inline constexpr i32 topbit(u64 x) {
    return 63 - __builtin_clzll(x);
}

inline constexpr i32 lowbit(i32 x) {
    return __builtin_ctz(static_cast<u32>(x));
}

inline constexpr i32 lowbit(u32 x) {
    return __builtin_ctz(x);
}

inline constexpr i32 lowbit(i64 x) {
    return __builtin_ctzll(static_cast<u64>(x));
}

inline constexpr i32 lowbit(u64 x) {
    return __builtin_ctzll(x);
}

#endif // LIB_BITS_HPP
