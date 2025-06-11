#ifndef LIB_BARRETT_HPP
#define LIB_BARRETT_HPP 1

#include <iostream>

#include <lib/math.hpp>
#include <lib/numeric_traits.hpp>
#include <lib/prelude.hpp>

struct barrett_32 {
    constexpr barrett_32(u32 m_)
        : m(m_), im((u64)(-1) / m + 1) {}

    constexpr u32 mod() const {
        return m;
    }

    constexpr u32 modulo(u64 z) const {
        if (m == 1) return 0;

        u64 x = u64((u128(z) * im) >> 64);
        u64 y = x * m;

        return u32(z - y + (z < y ? m : 0));
    }

    constexpr u32 mul(u32 a, u32 b) const {
        return modulo(u64(a) * b);
    }

private:
    u32 m;
    u64 im;
};

struct barrett_64 {
    constexpr barrett_64(u64 m_)
        : m(m_) {
        u128 im = u128(-1) / m;
        if (im * m + m == u128(0)) ++im;

        mh = im >> 64;
        ml = im & u64(-1);
    }

    constexpr u64 mod() const {
        return m;
    }

    constexpr u64 modulo(u128 x) const {
        u128 z = (x & u64(-1)) * ml;

        z = (x & u64(-1)) * mh + (x >> 64) * ml + (z >> 64);
        z = (x >> 64) * mh + (z >> 64);

        x -= z * m;
        return u64(x < m ? x : x - m);
    }

    constexpr u64 mul(u64 a, u64 b) const {
        return modulo(u128(a) * b);
    }

private:
    u64 m;
    u128 mh{}, ml{};
};

template <typename U, is_unsigned_integral_t<U> * = nullptr>
using barrett = std::conditional_t<std::is_same_v<U, u32>, barrett_32, barrett_64>;

#endif // LIB_BARRETT_HPP
