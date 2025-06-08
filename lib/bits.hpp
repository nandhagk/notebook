#ifndef LIB_BITS_HPP
#define LIB_BITS_HPP 1

#include <lib/prelude.hpp>

inline constexpr i32 popcnt(u32 x) {
    return __builtin_popcount(x);
}

inline constexpr i32 popcnt(i32 x) {
    return popcnt(static_cast<u32>(x));
}

inline constexpr i32 popcnt(u64 x) {
    return __builtin_popcountll(x);
}

inline constexpr i32 popcnt(i64 x) {
    return popcnt(static_cast<u64>(x));
}

inline constexpr i32 topbit(u32 x) {
    return 31 - __builtin_clz(x);
}

inline constexpr i32 topbit(i32 x) {
    return topbit(static_cast<u32>(x));
}

inline constexpr i32 topbit(u64 x) {
    return 63 - __builtin_clzll(x);
}

inline constexpr i32 topbit(i64 x) {
    return topbit(static_cast<u64>(x));
}

inline constexpr i32 lowbit(u32 x) {
    return __builtin_ctz(x);
}

inline constexpr i32 lowbit(i32 x) {
    return lowbit(static_cast<u32>(x));
}

inline constexpr i32 lowbit(u64 x) {
    return __builtin_ctzll(x);
}

inline constexpr i32 lowbit(i64 x) {
    return lowbit(static_cast<u64>(x));
}

#endif // LIB_BITS_HPP
