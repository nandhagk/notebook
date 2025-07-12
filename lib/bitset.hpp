#ifndef LIB_BITSET_HPP
#define LIB_BITSET_HPP 1

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <string>

#include <lib/bits.hpp>
#include <lib/prelude.hpp>

template <usize N, typename W, bool D, typename E>
class expr {
protected:
    static constexpr usize S = std::numeric_limits<W>::digits;
    static constexpr usize M = (N + S - 1) / S;

public:
    class const_word_iterator {
    public:
        using self_type = const_word_iterator;

        using difference_type = isize;
        using value_type = W;

        [[gnu::always_inline, nodiscard]] constexpr self_type &operator++() & {
            ++pos;
            return *this;
        }

        [[gnu::always_inline, nodiscard]] constexpr self_type &operator--() & {
            --pos;
            return *this;
        }

        [[gnu::always_inline, nodiscard]] constexpr value_type operator*() const {
            return e.word(pos);
        }

        // WARNING: Comparison of underlying expression is not performed
        [[gnu::always_inline, nodiscard]] constexpr bool operator==(const self_type &other) const {
            return pos == other.pos;
        }

        [[gnu::always_inline, nodiscard]] constexpr bool operator<(const self_type &other) const {
            return pos < other.pos;
        }

    private:
        const_word_iterator(const expr &f, usize p)
            : e(f), pos(p) {}

        const expr &e;
        usize pos;

        friend class expr;
    };

    static constexpr bool dir = D;

    [[gnu::always_inline, nodiscard]] constexpr const E &self() const {
        return *static_cast<const E *>(this);
    }

    [[gnu::always_inline, nodiscard]] constexpr W word(usize pos) const {
        return self().word(pos);
    }

    [[gnu::always_inline, nodiscard]] constexpr const_word_iterator cwbegin() const {
        return const_word_iterator(*this, 0);
    }

    [[gnu::always_inline, nodiscard]] constexpr const_word_iterator cwend() const {
        return const_word_iterator(*this, M);
    }

    [[gnu::always_inline, nodiscard]] constexpr usize count() const {
        return std::accumulate(cwbegin(), cwend(), 0, [](usize cnt, W w) { return cnt + popcnt(w); });
    }

    [[gnu::always_inline, nodiscard]] constexpr bool operator[](usize pos) const {
        return (word(pos / S) >> (pos % S)) & 1;
    }
};

template <usize N, typename W, typename Op, typename L, typename R>
class binary_expr : public expr<N, W, R::dir, binary_expr<N, W, Op, L, R>> {
    const L &lhs;
    const R &rhs;
    [[no_unique_address]] Op op;

public:
    constexpr binary_expr(const L &l, const R &r)
        : lhs(l), rhs(r) {}

    [[gnu::always_inline, nodiscard]] constexpr W word(usize pos) const {
        return op(lhs.word(pos), rhs.word(pos));
    }
};

template <usize N, typename W, typename E>
class shl_expr : public expr<N, W, false, shl_expr<N, W, E>> {
    using base_t = expr<N, W, false, shl_expr<N, W, E>>;

    using base_t::M;
    using base_t::S;

    const E &lhs;
    const usize wshift, offset, sub_offset;

public:
    constexpr shl_expr(const E &e, usize shift)
        : lhs(e), wshift(shift / S), offset(shift % S), sub_offset(S - offset) {}

    [[gnu::always_inline, nodiscard]] constexpr W word(usize pos) const {
        if (pos < wshift) return 0;
        if (offset == 0) return lhs.word(pos - wshift);

        if (pos == wshift) return lhs.word(0) << offset;
        return (lhs.word(pos - wshift) << offset) | (lhs.word(pos - wshift - 1) >> sub_offset);
    }
};

template <usize N, typename W, typename E>
struct shr_expr : public expr<N, W, true, shl_expr<N, W, E>> {
    using base_t = expr<N, W, true, shl_expr<N, W, E>>;

    using base_t::M;
    using base_t::S;

    const E &lhs;
    const usize wshift, offset, sub_offset, limit;

public:
    constexpr shr_expr(const E &e, usize shift)
        : lhs(e), wshift(shift / S), offset(shift % S), sub_offset(S - offset), limit(M - wshift - 1) {}

    [[gnu::always_inline, nodiscard]] constexpr W word(usize pos) const {
        if (pos > limit) return 0;
        if (offset == 0) return lhs.word(pos + wshift);

        if (pos == limit) return lhs.word(M - 1) >> offset;
        return (lhs.word(pos + wshift) >> offset) | (lhs.word(pos + wshift + 1) << (sub_offset));
    }
};

template <usize N, typename W, typename L, typename R, bool D1, bool D2>
[[gnu::always_inline, nodiscard]] constexpr auto operator|(const expr<N, W, D1, L> &lhs, const expr<N, W, D2, R> &rhs) {
    return binary_expr<N, W, std::bit_or<W>, L, R>(lhs.self(), rhs.self());
}

template <usize N, typename W, typename L, typename R, bool D1, bool D2>
[[gnu::always_inline, nodiscard]] constexpr auto operator&(const expr<N, W, D1, L> &lhs, const expr<N, W, D2, R> &rhs) {
    return binary_expr<N, W, std::bit_and<W>, L, R>(lhs.self(), rhs.self());
}

template <usize N, typename W, typename L, typename R, bool D1, bool D2>
[[gnu::always_inline, nodiscard]] constexpr auto operator^(const expr<N, W, D1, L> &lhs, const expr<N, W, D2, R> &rhs) {
    return binary_expr<N, W, std::bit_xor<W>, L, R>(lhs.self(), rhs.self());
}

template <usize N, typename W, typename E, bool D>
[[gnu::always_inline, nodiscard]] constexpr auto operator<<(const expr<N, W, D, E> &lhs, usize shift) {
    return shl_expr<N, W, E>(lhs.self(), shift);
}

template <usize N, typename W, typename E, bool D>
[[gnu::always_inline, nodiscard]] constexpr auto operator>>(const expr<N, W, D, E> &lhs, usize shift) {
    return shr_expr<N, W, E>(lhs.self(), shift);
}

template <usize N, typename W = u128>
class bitset : public expr<N, W, true, bitset<N, W>> {
    using base_t = expr<N, W, true, bitset<N, W>>;

    using base_t::M;
    using base_t::S;

    std::array<W, M> d{};

    template <typename E, bool D>
    [[gnu::always_inline]] constexpr void materialize(const expr<N, W, D, E> &expr) {
        if constexpr (D)
            std::copy(expr.cwbegin(), expr.cwend(), wbegin());
        else
            std::copy_backward(expr.cwbegin(), expr.cwend(), wbegin());
    }


public:
    constexpr bitset() = default;
    constexpr bitset(const bitset &other) = default;
    constexpr bitset(bitset &&other) = default;

    class reference {
    public:
        [[gnu::always_inline]] constexpr reference &operator=(bool val) {
            if (val)
                set();
            else
                unset();

            return *this;
        }

        [[gnu::always_inline, nodiscard]] constexpr operator bool() const {
            return (*word_ptr >> pos) & 1;
        }

        [[gnu::always_inline, nodiscard]] constexpr bool operator~() const {
            return !operator bool();
        }

        [[gnu::always_inline]] constexpr void set() {
            *word_ptr |= static_cast<W>(1) << pos;
        }

        [[gnu::always_inline]] constexpr void unset() {
            *word_ptr &= ~(static_cast<W>(1) << pos);
        }

        [[gnu::always_inline]] constexpr void flip() {
            *word_ptr ^= static_cast<W>(1) << pos;
        }

    private:
        constexpr reference(W *w_ptr, usize p)
            : word_ptr(w_ptr), pos(p) {}

        W *const word_ptr;
        const usize pos;

        friend class bitset;
    };

    [[gnu::always_inline, nodiscard]] constexpr reference operator[](usize pos) {
        return reference(&d[pos / S], pos % S);
    }

    class word_iterator {
    public:
        using self_type = word_iterator;

        using difference_type = isize;
        using value_type = W;
        using reference = value_type&;

        [[gnu::always_inline, nodiscard]] constexpr self_type &operator++() & {
            ++pos;
            return *this;
        }

        [[gnu::always_inline, nodiscard]] constexpr self_type &operator--() & {
            --pos;
            return *this;
        }

        [[gnu::always_inline, nodiscard]] constexpr reference operator*() const {
            return b.word(pos);
        }

        // WARNING: Comparison of underlying expression is not performed
        [[gnu::always_inline, nodiscard]] constexpr bool operator==(const self_type &other) const {
            return pos == other.pos;
        }

        [[gnu::always_inline, nodiscard]] constexpr bool operator<(const self_type &other) const {
            return pos < other.pos;
        }

    private:
        word_iterator(const bitset &c, usize p)
            : b(c), pos(p) {}

        const bitset &b;
        usize pos;

        friend class bitset;
    };

    [[gnu::always_inline, nodiscard]] constexpr word_iterator wbegin() const {
        return word_iterator(*this, 0);
    }

    [[gnu::always_inline, nodiscard]] constexpr word_iterator wend() const {
        return word_iterator(*this, M);
    }

    template <typename E, bool D>
    [[gnu::always_inline]] explicit constexpr bitset(const expr<N, W, D, E> &expr) {
        materialize(expr);
    }

    template <typename E, bool D>
    [[gnu::always_inline]] constexpr bitset &operator=(const expr<N, W, D, E> &expr) {
        materialize(expr);
        return *this;
    }

    [[gnu::always_inline]] constexpr void swap(bitset &other) noexcept {
        std::swap(d, other.d);
    }

    template <typename E, bool D>
    [[gnu::always_inline]] constexpr bitset &operator|=(const expr<N, W, D, E> &expr) & {
        return *this = *this | expr;
    }

    template <typename E, bool D>
    [[gnu::always_inline]] constexpr bitset &operator&=(const expr<N, W, D, E> &expr) & {
        return *this = *this & expr;
    }

    template <typename E, bool D>
    [[gnu::always_inline]] constexpr bitset &operator^=(const expr<N, W, D, E> &expr) & {
        return *this = *this ^ expr;
    }

    [[gnu::always_inline]] constexpr bitset &operator<<=(usize shift) & {
        return *this = *this << shift;
    }

    [[gnu::always_inline]] constexpr bitset &operator>>=(usize shift) & {
        return *this = *this >> shift;
    }

    [[gnu::always_inline, nodiscard]] constexpr W word(usize pos) const {
        return d[pos];
    }
    
    [[gnu::always_inline, nodiscard]] constexpr W& word(usize pos) {
        return d[pos];
    }

    [[gnu::always_inline]] constexpr void set(usize pos) {
        set(pos, true);
    }

    [[gnu::always_inline]] constexpr void unset(usize pos) {
        d[pos / S] &= ~(static_cast<W>(1) << (pos % S));
    }

    [[gnu::always_inline]] constexpr void flip(usize pos) {
        d[pos / S] ^= static_cast<W>(1) << (pos % S);
    }

    [[gnu::always_inline]] constexpr void reset() {
        std::fill(d.begin(), d.end(), static_cast<W>(0));
    }

    [[gnu::always_inline, nodiscard]] constexpr bool any() const {
        return std::any_of(d.begin(), d.end(), [](W w) { return w != 0; });
    }

    [[gnu::always_inline, nodiscard]] constexpr bool none() const {
        return !any();
    }

    [[gnu::always_inline]] friend std::istream &operator>>(std::istream &is, bitset &b) {
        std::string s;
        s.reserve(N);

        is >> s;

        const usize n = s.size();
        for (usize i = 0, j = n; i < n; ++i) b.set(i, s[--j] == '1');

        return is;
    }

private:
    [[gnu::always_inline]] constexpr void set(usize pos, bool val) {
        d[pos / S] |= static_cast<W>(val) << (pos % S);
    }
};

#endif // LIB_BITSET_HPP
