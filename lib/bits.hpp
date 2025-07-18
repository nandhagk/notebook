#ifndef LIB_BITS_HPP
#define LIB_BITS_HPP 1

#include <lib/prelude.hpp>

[[gnu::always_inline, nodiscard]] inline constexpr usize popcnt(u32 x) {
    return __builtin_popcount(x);
}

[[gnu::always_inline, nodiscard]] inline constexpr usize popcnt(i32 x) {
    return popcnt(static_cast<u32>(x));
}

[[gnu::always_inline, nodiscard]] inline constexpr usize popcnt(u64 x) {
    return __builtin_popcountll(x);
}

[[gnu::always_inline, nodiscard]] inline constexpr usize popcnt(i64 x) {
    return popcnt(static_cast<u64>(x));
}

[[gnu::always_inline, nodiscard]] inline constexpr usize popcnt(u128 x) {
    return popcnt(static_cast<u64>(x)) + popcnt(static_cast<u64>(x >> 64));
}

[[gnu::always_inline, nodiscard]] inline constexpr usize popcnt(i128 x) {
    return popcnt(static_cast<u128>(x));
}

[[gnu::always_inline, nodiscard]] inline constexpr usize topbit(u32 x) {
    return 31 - __builtin_clz(x);
}

[[gnu::always_inline, nodiscard]] inline constexpr usize topbit(i32 x) {
    return topbit(static_cast<u32>(x));
}

[[gnu::always_inline, nodiscard]] inline constexpr usize topbit(u64 x) {
    return 63 - __builtin_clzll(x);
}

[[gnu::always_inline, nodiscard]] inline constexpr usize topbit(i64 x) {
    return topbit(static_cast<u64>(x));
}

[[gnu::always_inline, nodiscard]] inline constexpr usize topbit(u128 x) {
    return static_cast<u64>(x >> 64) ? (topbit(static_cast<u64>(x >> 64)) + 64) : topbit(static_cast<u64>(x));
}

[[gnu::always_inline, nodiscard]] inline constexpr usize topbit(i128 x) {
    return topbit(static_cast<u128>(x));
}

[[gnu::always_inline, nodiscard]] inline constexpr usize lowbit(u32 x) {
    return __builtin_ctz(x);
}

[[gnu::always_inline, nodiscard]] inline constexpr usize lowbit(i32 x) {
    return lowbit(static_cast<u32>(x));
}

[[gnu::always_inline, nodiscard]] inline constexpr usize lowbit(u64 x) {
    return __builtin_ctzll(x);
}

[[gnu::always_inline, nodiscard]] inline constexpr usize lowbit(i64 x) {
    return lowbit(static_cast<u64>(x));
}

[[gnu::always_inline, nodiscard]] inline constexpr usize lowbit(u128 x) {
    return static_cast<u64>(x) ? lowbit(static_cast<u64>(x)) : (lowbit(static_cast<u64>(x >> 64)) + 64);
}

[[gnu::always_inline, nodiscard]] inline constexpr usize lowbit(i128 x) {
    return lowbit(static_cast<u128>(x));
}

#endif // LIB_BITS_HPP
