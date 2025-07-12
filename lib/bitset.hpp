#ifndef LIB_BITSET_HPP
#define LIB_BITSET_HPP 1

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>

#include <lib/bits.hpp>
#include <lib/prelude.hpp>

template <usize N, typename W, bool D, typename E>
class expr {
public:
    static constexpr usize size = N;

    using word_type = W;
    static constexpr usize word_size = std::numeric_limits<word_type>::digits;
    static constexpr usize block_count = (size + word_size - 1) / word_size;
    static constexpr W mask = (static_cast<W>(1) << (size % word_size)) - 1;

    static constexpr bool dir = D;

    class const_word_iterator {
    public:
        using self_type = const_word_iterator;

        using difference_type = isize;
        using value_type = W;
        using reference = void;
        using iterator_category = std::random_access_iterator_tag;

        [[gnu::always_inline, nodiscard]] constexpr self_type &operator+=(difference_type offset) & {
            pos += offset;
            return *this;
        }

        [[gnu::always_inline, nodiscard]] constexpr self_type &operator-=(difference_type offset) & {
            return *this += -offset;
        }

        [[gnu::always_inline, nodiscard]] constexpr self_type &operator++() & {
            return *this += 1;
        }

        [[gnu::always_inline, nodiscard]] constexpr self_type &operator--() & {
            return *this -= 1;
        }

        [[gnu::always_inline, nodiscard]] constexpr self_type operator++(int) & {
            self_type tmp = *this;
            operator++();
            return tmp;
        }

        [[gnu::always_inline, nodiscard]] constexpr self_type operator--(int) & {
            self_type tmp = *this;
            operator--();
            return tmp;
        }

        [[gnu::always_inline, nodiscard]] constexpr value_type operator*() const {
            return e.word(pos);
        }

        [[gnu::always_inline, nodiscard]] friend constexpr self_type operator+(self_type lhs, difference_type offset) {
            return lhs += offset;
        }

        [[gnu::always_inline, nodiscard]] friend constexpr self_type operator-(self_type lhs, difference_type offset) {
            return lhs -= offset;
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
        return const_word_iterator(*this, block_count);
    }

    [[gnu::always_inline, nodiscard]] constexpr usize count() const {
        if constexpr (size % word_size == 0)
            return std::accumulate(cwbegin(), cwend(), 0, [](usize cnt, W w) { return cnt + popcnt(w); });

        const auto last = std::prev(cwend());
        return std::accumulate(cwbegin(), last, 0, [](usize cnt, W w) { return cnt + popcnt(w); }) +
               popcnt(*last & mask);
    }

    [[gnu::always_inline, nodiscard]] constexpr bool operator[](usize pos) const {
        return (word(pos / word_size) >> (pos % word_size)) & 1;
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
class not_expr : public expr<N, W, E::dir, not_expr<N, W, E>> {
    const E &lhs;

public:
    constexpr not_expr(const E &e)
        : lhs(e) {}

    [[gnu::always_inline, nodiscard]] constexpr W word(usize pos) const {
        return ~lhs.word(pos);
    }
};

template <usize N, typename W, typename E>
class shl_expr : public expr<N, W, false, shl_expr<N, W, E>> {
    using base_type = expr<N, W, false, shl_expr<N, W, E>>;

    using base_type::block_count;
    using base_type::word_size;

    const E &lhs;
    const usize wshift, offset, sub_offset;

public:
    constexpr shl_expr(const E &e, usize shift)
        : lhs(e), wshift(shift / word_size), offset(shift % word_size), sub_offset(word_size - offset) {}

    [[gnu::always_inline, nodiscard]] constexpr W word(usize pos) const {
        if (pos < wshift) return 0;
        if (offset == 0) return lhs.word(pos - wshift);

        if (pos == wshift) return lhs.word(0) << offset;
        return (lhs.word(pos - wshift) << offset) | (lhs.word(pos - wshift - 1) >> sub_offset);
    }
};

template <usize N, typename W, typename E>
struct shr_expr : public expr<N, W, true, shl_expr<N, W, E>> {
    using base_type = expr<N, W, true, shl_expr<N, W, E>>;

    using base_type::block_count;
    using base_type::word_size;

    const E &lhs;
    const usize wshift, offset, sub_offset, limit;

public:
    constexpr shr_expr(const E &e, usize shift)
        : lhs(e),
          wshift(shift / word_size),
          offset(shift % word_size),
          sub_offset(word_size - offset),
          limit(block_count - wshift - 1) {}

    [[gnu::always_inline, nodiscard]] constexpr W word(usize pos) const {
        if (pos > limit) return 0;
        if (offset == 0) return lhs.word(pos + wshift);

        if (pos == limit) return lhs.word(block_count - 1) >> offset;
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
[[gnu::always_inline, nodiscard]] constexpr auto operator~(const expr<N, W, D, E> &lhs) {
    return not_expr<N, W, E>(lhs.self());
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
    static_assert(N > 0, "Can't create empty bitset");

    using base_type = expr<N, W, true, bitset<N, W>>;

    using base_type::block_count;
    using base_type::mask;
    using base_type::size;
    using base_type::word_size;

    using storage_type = std::array<W, block_count>;

    storage_type d{};

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
        return reference(&d[pos / word_size], pos % word_size);
    }

    using word_iterator = typename storage_type::iterator;

    [[gnu::always_inline, nodiscard]] constexpr word_iterator wbegin() {
        return d.begin();
    }

    [[gnu::always_inline, nodiscard]] constexpr word_iterator wend() {
        return d.end();
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

    [[gnu::always_inline]] constexpr bitset &operator~() & {
        return *this = ~(*this);
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

    [[gnu::always_inline, nodiscard]] constexpr W &word(usize pos) {
        return d[pos];
    }

    [[gnu::always_inline]] constexpr void set(usize pos) {
        set(pos, true);
    }

    [[gnu::always_inline]] constexpr void unset(usize pos) {
        d[pos / word_size] &= ~(static_cast<W>(1) << (pos % word_size));
    }

    [[gnu::always_inline]] constexpr void flip(usize pos) {
        d[pos / word_size] ^= static_cast<W>(1) << (pos % word_size);
    }

    [[gnu::always_inline]] constexpr void set() {
        std::fill(d.begin(), d.end(), static_cast<W>(-1));
    }

    [[gnu::always_inline]] constexpr void unset() {
        std::fill(d.begin(), d.end(), static_cast<W>(0));
    }

    [[gnu::always_inline]] constexpr void flip() {
        operator~();
    }

    [[gnu::always_inline, nodiscard]] constexpr bool all() const {
        if constexpr (size % word_size == 0)
            return std::all_of(d.begin(), d.end(), [](W w) { return w == static_cast<W>(-1); });

        const auto last = std::prev(d.end());
        return std::all_of(d.begin(), last, [](W w) { return w == static_cast<W>(-1); }) && ((*last & mask) == mask);
    }

    [[gnu::always_inline, nodiscard]] constexpr bool any() const {
        if constexpr (size % word_size == 0) return std::any_of(d.begin(), d.end(), [](W w) { return w != 0; });

        const auto last = std::prev(d.end());
        return std::any_of(d.begin(), last, [](W w) { return w != 0; }) || ((*last & mask) != 0);
    }

    [[gnu::always_inline, nodiscard]] constexpr bool none() const {
        return !any();
    }

    [[gnu::always_inline]] friend std::istream &operator>>(std::istream &is, bitset &b) {
        std::string s;
        s.reserve(size);

        is >> s;

        const usize n = s.size();
        for (usize i = 0, j = n; i < n; ++i) b.set(i, s[--j] == '1');

        return is;
    }

private:
    [[gnu::always_inline]] constexpr void set(usize pos, bool val) {
        d[pos / word_size] |= static_cast<W>(val) << (pos % word_size);
    }
};

#endif // LIB_BITSET_HPP
