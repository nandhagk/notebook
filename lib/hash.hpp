#ifndef LIB_HASH_HPP
#define LIB_HASH_HPP 1

#include <type_traits>

#include <lib/prelude.hpp>
#include <lib/random.hpp>

template <typename T, typename D = void> struct hash {};

template <typename T> inline void hash_combine(u64 &seed, const T &v) {
    hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b97f4a7c15 + (seed << 12) + (seed >> 4);
};

template <typename T> struct hash<T, typename std::enable_if<std::is_integral<T>::value>::type> {
    u64 operator()(T _x) const {
        u64 x = _x;
        x += 0x9e3779b97f4a7c15 + FIXED_RANDOM;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }
};

template <typename T> struct hash<T, std::void_t<decltype(std::begin(std::declval<T>()))>> {
    u64 operator()(const T &a) const {
        u64 value = FIXED_RANDOM;
        for (const auto &x : a) hash_combine(value, x);
        return value;
    }
};

template <typename... T> struct hash<std::tuple<T...>> {
    u64 operator()(const std::tuple<T...> &a) const {
        u64 value = FIXED_RANDOM;
        std::apply([&value](T const &...args) { (hash_combine(value, args), ...); }, a);
        return value;
    }
};

template <typename T, typename U> struct hash<std::pair<T, U>> {
    u64 operator()(const std::pair<T, U> &a) const {
        u64 value = FIXED_RANDOM;
        hash_combine(value, a.first);
        hash_combine(value, a.second);
        return value;
    }
};

#endif // LIB_HASH_HPP
