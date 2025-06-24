#ifndef LIB_BITSET_HPP
#define LIB_BITSET_HPP 1

#include <array>
#include <functional>
#include <iostream>
#include <limits>

#include <lib/bits.hpp>
#include <lib/numeric_traits.hpp>
#include <lib/prelude.hpp>

template <usize N, typename T = u128, is_unsigned_integral_t<T> * = nullptr>
class bitset {
public:
    static constexpr usize W = std::numeric_limits<T>::digits;
    static_assert((W & (W - 1)) == 0, "sizeof(T) must be a power of 2");

    static constexpr usize S = topbit(W);
    static constexpr usize M = W - 1;
    static constexpr usize B = (N + W - 1) >> S;

    class reference {
    public:
        [[gnu::always_inline]] constexpr reference &operator=(bool val) {
            if (val)
                *word_ |= T(1) << pos_;
            else
                *word_ &= ~(T(1) << pos_);
        }

        [[gnu::always_inline, nodiscard]] constexpr operator bool() const {
            return (*word_ >> pos_) != T(0);
        }

        [[gnu::always_inline, nodiscard]] constexpr bool operator~() const {
            return (*word_ >> pos_) == T(0);
        }

        [[gnu::always_inline]] constexpr void flip() {
            *word_ ^= T(1) << pos_;
        }

    private:
        [[gnu::always_inline]] constexpr reference(T *word, usize pos)
            : word_{word}, pos_{pos} {}

        T *word_{nullptr};
        usize pos_{0};

        friend class bitset;
    };

    [[gnu::always_inline]] constexpr bool operator[](usize pos) const {
        return (buf_[pos >> S] >> (pos & M)) != T(0);
    }

    [[gnu::always_inline]] constexpr reference operator[](usize pos) {
        return reference(&buf_[pos >> S], pos & M);
    }

    [[gnu::always_inline]] constexpr void set(usize pos) {
        buf_[pos >> S] |= T(1) << (pos & M);
    }

    [[gnu::always_inline]] constexpr void unset(usize pos) {
        buf_[pos >> S] &= ~(T(1) << (pos & M));
    }

    [[gnu::always_inline]] constexpr void flip(usize pos) {
        buf_[pos >> S] ^= T(1) << (pos & M);
    }

    [[gnu::always_inline, nodiscard]] constexpr bool any() const {
        for (usize i = 0; i < B; ++i)
            if (buf_[i]) return true;

        return false;
    }

    [[gnu::always_inline, nodiscard]] constexpr bool none() const {
        return !any();
    }

    [[gnu::always_inline, nodiscard]] constexpr usize count() const {
        usize cnt = 0;
        for (usize i = 0; i < B; ++i) cnt += popcnt(buf_[i]);

        return cnt;
    }

    [[gnu::always_inline, nodiscard]] constexpr usize count_intersection(const bitset &rhs) const {
        return count_op(rhs, std::bit_and{});
    }

    [[gnu::always_inline, nodiscard]] constexpr usize count_union(const bitset &rhs) const {
        return count_op(rhs, std::bit_or{});
    }

    [[gnu::always_inline, nodiscard]] constexpr usize count_symmetric_difference(const bitset &rhs) const {
        return count_op(rhs, std::bit_xor{});
    }

    [[gnu::always_inline]] constexpr bitset &operator&=(const bitset &rhs) & {
        do_op(rhs, std::bit_and{});
        return *this;
    }

    [[gnu::always_inline]] constexpr bitset &operator|=(const bitset &rhs) & {
        do_op(rhs, std::bit_or{});
        return *this;
    }

    [[gnu::always_inline]] constexpr bitset &operator^=(const bitset &rhs) & {
        do_op(rhs, std::bit_xor{});
        return *this;
    }

    friend std::istream &operator>>(std::istream &is, bitset &b) {
        std::string s;
        s.reserve(N);

        is >> s;

        const usize n = std::min(N, s.size());
        for (usize i = 0; i < n; ++i) b.set(i, s[i] == '1');

        return is;
    }

    [[gnu::always_inline, nodiscard]] friend constexpr bitset operator&(bitset lhs, const bitset &rhs) {
        return lhs &= rhs;
    }

    [[gnu::always_inline, nodiscard]] friend constexpr bitset operator|(bitset lhs, const bitset &rhs) {
        return lhs |= rhs;
    }

    [[gnu::always_inline, nodiscard]] friend constexpr bitset operator^(bitset lhs, const bitset &rhs) {
        return lhs ^= rhs;
    }

    [[gnu::always_inline, nodiscard]] friend constexpr bitset operator==(const bitset &lhs, const bitset &rhs) {
        return lhs.buf_ == rhs.buf_;
    }

private:
    std::array<T, B> buf_{};

    [[gnu::always_inline]] constexpr void set(usize pos, bool val) {
        buf_[pos >> S] |= (T(val) << (pos & M));
    }

    template <typename Op>
    [[gnu::always_inline]] constexpr void do_op(const bitset &rhs, Op op) & {
        for (usize i = 0; i < B; ++i) buf_[i] = op(buf_[i], rhs.buf_[i]);
    }

    template <typename Op>
    [[gnu::always_inline, nodiscard]] constexpr usize count_op(const bitset &rhs, Op op) const {
        usize cnt = 0;
        for (usize i = 0; i < B; ++i) cnt += popcnt(op(buf_[i], rhs.buf_[i]));

        return cnt;
    }
};

#endif // LIB_BITSET_HPP
