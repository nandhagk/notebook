#ifndef LIB_BITSET_HPP
#define LIB_BITSET_HPP 1

#include <array>
#include <functional>
#include <iostream>
#include <limits>

#include <lib/bits.hpp>
#include <lib/prelude.hpp>

template <usize N, typename T = u128>
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

    [[gnu::always_inline]] constexpr void reset() {
        std::fill(buf_.begin(), buf_.end(), T(0));
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

    [[gnu::always_inline]] constexpr bitset &operator<<=(usize shift) & {
        if (shift >= N) [[unlikely]]
            reset();
        else
            left_shift(shift);

        return *this;
    }

    [[gnu::always_inline]] constexpr bitset &operator>>=(usize shift) & {
        if (shift >= N) [[unlikely]]
            reset();
        else
            right_shift(shift);

        return *this;
    }

    friend std::istream &operator>>(std::istream &is, bitset &b) {
        std::string s;
        s.reserve(N);

        is >> s;

        const usize n = std::min(N, s.size());
        for (usize i = 0, j = n - 1; i < n; ++i, --j) b.set(i, s[j] == '1');

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

    [[gnu::always_inline, nodiscard]] friend constexpr bitset operator<<(bitset lhs, usize shift) {
        return lhs <<= shift;
    }

    [[gnu::always_inline, nodiscard]] friend constexpr bitset operator>>(bitset lhs, usize shift) {
        return lhs >>= shift;
    }

    [[gnu::always_inline, nodiscard]] friend constexpr bitset operator==(const bitset &lhs, const bitset &rhs) {
        return lhs.buf_ == rhs.buf_;
    }

private:
    std::array<T, B> buf_{};

    [[gnu::always_inline]] constexpr void set(usize pos, bool val) {
        buf_[pos >> S] |= (T(val) << (pos & M));
    }

    [[gnu::always_inline]] constexpr void left_shift(usize shift) {
        if (shift == 0) [[unlikely]]
            return;

        const usize wshift = shift >> S;
        const usize offset = shift & M;

        if (offset == 0) {
            for (usize i = B - 1; i >= wshift; --i) buf_[i] = buf_[i - wshift];
        } else {
            const usize sub_offset = W - offset;
            for (usize i = B - 1; i > wshift; --i)
                buf_[i] = (buf_[i - wshift] << offset) | (buf_[i - wshift - 1] >> sub_offset);
            buf_[wshift] = buf_[0] << offset;
        }

        std::fill(buf_.begin(), buf_.begin() + wshift, T(0));
    }

    [[gnu::always_inline]] constexpr void right_shift(usize shift) {
        if (shift == 0) [[unlikely]]
            return;

        const usize wshift = shift >> S;
        const usize offset = shift & M;
        const usize limit = B - shift - 1;

        if (offset == 0) {
            for (usize i = 0; i <= limit; ++i) buf_[i] = buf_[i + wshift];
        } else {
            const usize sub_offset = W - offset;
            for (usize i = 0; i < limit; ++i)
                buf_[i] = (buf_[i + wshift] >> offset) | (buf_[i + wshift + 1] << sub_offset);
            buf_[limit] = buf_[B - 1] >> offset;
        }

        std::fill(buf_.begin() + limit + 1, buf_.end(), T(0));
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
