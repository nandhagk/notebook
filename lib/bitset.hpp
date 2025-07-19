#ifndef LIB_BITSET_HPP
#define LIB_BITSET_HPP 1

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>

#include <lib/bits.hpp>
#include <lib/numeric_traits.hpp>
#include <lib/prelude.hpp>

enum class order : std::int8_t { forward, backward };

// CONTRACT: Excess bits will always be zero
template <usize N, typename WordT, typename ExprT, order Ord>
class expr {
public:
    using word_type = WordT;
    using expr_type = ExprT;

    [[gnu::always_inline, nodiscard]] static constexpr usize size() {
        return N;
    }

    [[gnu::always_inline, nodiscard]] static constexpr usize word_size() {
        return std::numeric_limits<word_type>::digits;
    }

    [[gnu::always_inline, nodiscard]] static constexpr usize word_count() {
        return (size() + word_size() - 1) / word_size();
    }

    [[gnu::always_inline, nodiscard]] static constexpr bool has_excess_bits() {
        return size() % word_size();
    }

    [[gnu::always_inline, nodiscard]] static constexpr order ord() {
        return Ord;
    }

    static_assert(size() > 0, "size must be positive");
    static_assert(is_unsigned_integral_v<word_type>, "word_type must be unsigned integral");

    [[gnu::always_inline, nodiscard]] static constexpr usize whichword(usize pos) {
        return pos / word_size();
    }

    [[gnu::always_inline, nodiscard]] static constexpr usize whichbit(usize pos) {
        return pos % word_size();
    }

    [[gnu::always_inline, nodiscard]] static constexpr word_type maskbit(usize pos) {
        return static_cast<word_type>(1) << whichbit(pos);
    }

    [[gnu::always_inline, nodiscard]] static constexpr word_type mask() {
        static_assert(has_excess_bits());
        return maskbit(size()) - 1;
    }

    [[gnu::always_inline, nodiscard]] constexpr const expr_type &self() const {
        return *static_cast<const expr_type *>(this);
    }

    [[gnu::always_inline, nodiscard]] constexpr word_type word(usize wpos) const {
        return self().word(wpos);
    }

    class const_word_iterator {
    public:
        using self_type = const_word_iterator;

        using difference_type = isize;
        using value_type = word_type;
        using reference = void;
        using iterator_category = std::random_access_iterator_tag;

        [[gnu::always_inline]] constexpr self_type &operator+=(difference_type n) & {
            wpos += n;
            return *this;
        }

        [[gnu::always_inline]] constexpr self_type &operator-=(difference_type n) & {
            wpos -= n;
            return *this;
        }

        [[gnu::always_inline]] constexpr self_type &operator++() & {
            ++wpos;
            return *this;
        }

        [[gnu::always_inline]] constexpr self_type &operator--() & {
            --wpos;
            return *this;
        }

        [[gnu::always_inline, nodiscard]] constexpr self_type operator++(i32) & {
            self_type tmp = *this;
            operator++();
            return tmp;
        }

        [[gnu::always_inline, nodiscard]] constexpr self_type operator--(i32) & {
            self_type tmp = *this;
            operator--();
            return tmp;
        }

        [[gnu::always_inline, nodiscard]] constexpr value_type operator*() const {
            return operator[](0);
        }

        [[gnu::always_inline, nodiscard]] constexpr value_type operator[](difference_type n) const {
            return expr_ptr->word(wpos + n);
        }

        [[gnu::always_inline, nodiscard]] friend constexpr self_type operator+(self_type lhs, difference_type n) {
            return lhs += n;
        }

        [[gnu::always_inline, nodiscard]] friend constexpr self_type operator-(self_type lhs, difference_type n) {
            return lhs -= n;
        }

        [[gnu::always_inline, nodiscard]] friend constexpr difference_type operator-(const self_type &lhs,
                                                                                     const self_type &rhs) {
            return static_cast<difference_type>(lhs.wpos) - rhs.wpos;
        }

        // WARNING: Comparison of underlying expression is not performed
        [[gnu::always_inline, nodiscard]] constexpr bool operator==(const self_type &other) const {
            return wpos == other.wpos;
        }

    private:
        const_word_iterator(const expr *const e_ptr, usize p)
            : expr_ptr(e_ptr), wpos(p) {}

        const expr *const expr_ptr;
        usize wpos;

        friend class expr;
    };

    using const_reverse_word_iterator = std::reverse_iterator<const_word_iterator>;

    [[gnu::always_inline, nodiscard]] constexpr const_word_iterator cwbegin() const {
        return const_word_iterator(this, 0);
    }

    [[gnu::always_inline, nodiscard]] constexpr const_word_iterator cwend() const {
        return const_word_iterator(this, word_count());
    }

    [[gnu::always_inline, nodiscard]] constexpr const_reverse_word_iterator crwbegin() const {
        return const_reverse_word_iterator(cwend());
    }

    [[gnu::always_inline, nodiscard]] constexpr const_reverse_word_iterator crwend() const {
        return const_reverse_word_iterator(cwbegin());
    }

    [[gnu::always_inline, nodiscard]] constexpr usize count() const {
        return std::transform_reduce(cwbegin(), cwend(), 0, std::plus<usize>{}, [](word_type w) { return popcnt(w); });
    }

    [[gnu::always_inline, nodiscard]] constexpr bool all() const {
        if constexpr (!has_excess_bits()) return std::none_of(cwbegin(), cwend(), [](word_type w) { return ~w; });

        const auto last = std::prev(cwend());
        return ((*last & mask()) == mask()) && std::none_of(cwbegin(), last, [](word_type w) { return ~w; });
    }

    [[gnu::always_inline, nodiscard]] constexpr bool any() const {
        return std::any_of(cwbegin(), cwend(), [](word_type w) { return w; });
    }

    [[gnu::always_inline, nodiscard]] constexpr bool none() const {
        return !any();
    }

    template <typename RExprT, order ROrd>
    [[gnu::always_inline, nodiscard]] constexpr bool is_subset_of(const expr<N, WordT, RExprT, ROrd> &rhs) const {
        return (*this & rhs) == *this;
    }

    template <typename RExprT, order ROrd>
    [[gnu::always_inline, nodiscard]] constexpr bool is_superset_of(const expr<N, WordT, RExprT, ROrd> &rhs) const {
        return rhs.is_subset_of(*this);
    }

    [[gnu::always_inline, nodiscard]] constexpr bool operator[](usize pos) const {
        return (word(whichword(pos)) >> (whichbit(pos))) & 1;
    }

    class const_iterator {
    public:
        using self_type = const_iterator;

        using difference_type = isize;
        using value_type = bool;
        using reference = void;
        using iterator_category = std::random_access_iterator_tag;

        [[gnu::always_inline]] constexpr self_type &operator+=(difference_type n) & {
            pos += n;
            return *this;
        }

        [[gnu::always_inline]] constexpr self_type &operator-=(difference_type n) & {
            pos -= n;
            return *this;
        }

        [[gnu::always_inline]] constexpr self_type &operator++() & {
            ++pos;
            return *this;
        }

        [[gnu::always_inline]] constexpr self_type &operator--() & {
            --pos;
            return *this;
        }

        [[gnu::always_inline, nodiscard]] constexpr self_type operator++(i32) & {
            self_type tmp = *this;
            operator++();
            return tmp;
        }

        [[gnu::always_inline, nodiscard]] constexpr self_type operator--(i32) & {
            self_type tmp = *this;
            operator--();
            return tmp;
        }

        [[gnu::always_inline, nodiscard]] constexpr value_type operator*() const {
            return operator[](0);
        }

        [[gnu::always_inline, nodiscard]] constexpr value_type operator[](difference_type n) const {
            return expr_ptr->operator[](pos + n);
        }

        [[gnu::always_inline, nodiscard]] friend constexpr self_type operator+(self_type lhs, difference_type n) {
            return lhs += n;
        }

        [[gnu::always_inline, nodiscard]] friend constexpr self_type operator-(self_type lhs, difference_type n) {
            return lhs -= n;
        }

        [[gnu::always_inline, nodiscard]] friend constexpr difference_type operator-(const self_type &lhs,
                                                                                     const self_type &rhs) {
            return static_cast<difference_type>(lhs.pos) - rhs.pos;
        }

        // WARNING: Comparison of underlying expression is not performed
        [[gnu::always_inline, nodiscard]] constexpr bool operator==(const self_type &other) const {
            return pos == other.pos;
        }

    private:
        const_iterator(const expr *const e_ptr, usize p)
            : expr_ptr(e_ptr), pos(p) {}

        const expr *const expr_ptr;
        usize pos;

        friend class expr;
    };

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    [[gnu::always_inline, nodiscard]] constexpr const_iterator cbegin() const {
        return const_iterator(this, 0);
    }

    [[gnu::always_inline, nodiscard]] constexpr const_iterator cend() const {
        return const_iterator(this, size());
    }

    [[gnu::always_inline, nodiscard]] constexpr const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
    }

    [[gnu::always_inline, nodiscard]] constexpr const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
    }

    [[gnu::always_inline, nodiscard]] constexpr usize find_first() const {
        for (usize wpos = 0; wpos < word_count(); ++wpos)
            if (const word_type w = word(wpos); w != 0) return lowbit(w) + wpos * word_size();

        return size();
    }

    [[gnu::always_inline, nodiscard]] constexpr usize find_next(usize prev) const {
        const usize prev_word = whichword(prev);

        const word_type prev_mask = maskbit(whichbit(prev));
        if (const word_type w = word(prev_word) & ~prev_mask & ~(prev_mask - 1); w != 0)
            return lowbit(w) + prev_word * word_size();

        for (usize wpos = prev_word + 1; wpos < word_count(); ++wpos)
            if (const word_type w = word(wpos); w != 0) return lowbit(w) + wpos * word_size();

        return size();
    }

    [[gnu::always_inline, nodiscard]] constexpr usize find_last() const {
        for (usize wpos = word_count(); wpos-- > 0;)
            if (const word_type w = word(wpos); w != 0) return topbit(w) + wpos * word_size();

        return -1;
    }

    [[gnu::always_inline, nodiscard]] constexpr usize find_prev(usize prev) const {
        if (prev == size()) return find_last();

        const usize prev_word = whichword(prev);

        const word_type prev_mask = maskbit(whichbit(prev));
        if (const word_type w = word(prev_word) & (prev_mask - 1); w != 0) return topbit(w) + prev_word * word_size();

        for (usize wpos = prev_word; wpos-- > 0;)
            if (const word_type w = word(wpos); w != 0) return topbit(w) + wpos * word_size();

        return -1;
    }

    class const_ones_iterator {
    public:
        using self_type = const_ones_iterator;

        using difference_type = isize;
        using value_type = usize;
        using reference = void;
        using iterator_category = std::bidirectional_iterator_tag;

        [[gnu::always_inline]] constexpr self_type &operator++() & {
            pos = expr_ptr->find_next(pos);
            return *this;
        }

        [[gnu::always_inline]] constexpr self_type &operator--() & {
            pos = expr_ptr->find_prev(pos);
            return *this;
        }

        [[gnu::always_inline, nodiscard]] constexpr self_type operator++(i32) & {
            self_type tmp = *this;
            operator++();
            return tmp;
        }

        [[gnu::always_inline, nodiscard]] constexpr self_type operator--(i32) & {
            self_type tmp = *this;
            operator--();
            return tmp;
        }

        [[gnu::always_inline, nodiscard]] constexpr value_type operator*() const {
            return pos;
        }

        // WARNING: Comparison of underlying expression is not performed
        [[gnu::always_inline, nodiscard]] constexpr bool operator==(const self_type &other) const {
            return pos == other.pos;
        }

    private:
        const_ones_iterator(const expr *const e_ptr, usize p)
            : expr_ptr(e_ptr), pos(p) {}

        const expr *const expr_ptr;
        usize pos;

        friend class expr;
    };

    using const_reverse_ones_iterator = std::reverse_iterator<const_ones_iterator>;

    [[gnu::always_inline, nodiscard]] constexpr const_ones_iterator cobegin() const {
        return const_ones_iterator(this, find_first());
    }

    [[gnu::always_inline, nodiscard]] constexpr const_ones_iterator coend() const {
        return const_ones_iterator(this, size());
    }

    [[gnu::always_inline, nodiscard]] constexpr const_reverse_ones_iterator crobegin() const {
        return const_reverse_ones_iterator(coend());
    }

    [[gnu::always_inline, nodiscard]] constexpr const_reverse_ones_iterator croend() const {
        return const_reverse_ones_iterator(cobegin());
    }
};

template <usize N, typename WordT, typename LExprT, typename RExprT, order LOrd, order ROrd>
[[gnu::always_inline, nodiscard]] constexpr bool operator==(const expr<N, WordT, LExprT, LOrd> &lhs,
                                                            const expr<N, WordT, RExprT, ROrd> &rhs) {
    return std::equal(lhs.cwbegin(), lhs.cwend(), rhs.cwbegin());
}

template <usize N, typename WordT, typename LExprT, typename RExprT, order LOrd, order ROrd>
[[gnu::always_inline, nodiscard]] constexpr bool operator<(const expr<N, WordT, LExprT, LOrd> &lhs,
                                                           const expr<N, WordT, RExprT, ROrd> &rhs) {
    return std::lexicographical_compare(lhs.cwbegin(), lhs.cwend(), rhs.cwbegin(), rhs.cwend());
}

template <usize N, typename WordT, typename LExprT, typename RExprT, typename Op>
class binary_expr : public expr<N, WordT, binary_expr<N, WordT, LExprT, RExprT, Op>, RExprT::ord()> {
    using base_type = expr<N, WordT, binary_expr<N, WordT, LExprT, RExprT, Op>, RExprT::ord()>;
    using word_type = typename base_type::word_type;

    const LExprT &lhs;
    const RExprT &rhs;
    [[no_unique_address]] Op op;

public:
    constexpr binary_expr(const LExprT &l, const RExprT &r)
        : lhs(l), rhs(r) {}

    [[gnu::always_inline, nodiscard]] constexpr word_type word(usize wpos) const {
        return op(lhs.word(wpos), rhs.word(wpos));
    }
};

template <usize N, typename WordT, typename ExprT>
class not_expr : public expr<N, WordT, not_expr<N, WordT, ExprT>, ExprT::ord()> {
    using base_type = expr<N, WordT, not_expr<N, WordT, ExprT>, ExprT::ord()>;
    using word_type = typename base_type::word_type;

    using base_type::has_excess_bits;
    using base_type::mask;
    using base_type::size;
    using base_type::word_count;
    using base_type::word_size;

    const ExprT &lhs;

public:
    constexpr not_expr(const ExprT &e)
        : lhs(e) {}

    [[gnu::always_inline, nodiscard]] constexpr word_type word(usize wpos) const {
        const word_type w = ~lhs.word(wpos);

        if constexpr (!has_excess_bits()) return w;
        return wpos == word_count() - 1 ? w & mask() : w;
    }
};

template <usize N, typename WordT, typename ExprT>
class shl_expr : public expr<N, WordT, shl_expr<N, WordT, ExprT>, order::backward> {
    using base_type = expr<N, WordT, shl_expr<N, WordT, ExprT>, order::backward>;
    using word_type = typename base_type::word_type;

    using base_type::has_excess_bits;
    using base_type::mask;
    using base_type::size;
    using base_type::word_count;
    using base_type::word_size;

    const ExprT &lhs;
    const usize wshift, offset, sub_offset;

    [[gnu::always_inline, nodiscard]] constexpr word_type unmasked_word(usize wpos) const {
        if (offset == 0) return lhs.word(wpos - wshift);

        if (wpos == wshift) return lhs.word(0) << offset;
        return (lhs.word(wpos - wshift) << offset) | (lhs.word(wpos - wshift - 1) >> sub_offset);
    }

public:
    constexpr shl_expr(const ExprT &e, usize shift)
        : lhs(e), wshift(shift / word_size()), offset(shift % word_size()), sub_offset(word_size() - offset) {}

    [[gnu::always_inline, nodiscard]] constexpr word_type word(usize wpos) const {
        if (wpos < wshift) return 0;

        const word_type w = unmasked_word(wpos);

        if constexpr (!has_excess_bits()) return w;
        return wpos == word_count() - 1 ? w & mask() : w;
    }
};

template <usize N, typename WordT, typename ExprT>
struct shr_expr : public expr<N, WordT, shr_expr<N, WordT, ExprT>, order::forward> {
    using base_type = expr<N, WordT, shr_expr<N, WordT, ExprT>, order::forward>;
    using word_type = typename base_type::word_type;

    using base_type::word_count;
    using base_type::word_size;

    const ExprT &lhs;
    const usize wshift, offset, sub_offset, limit;

public:
    constexpr shr_expr(const ExprT &e, usize shift)
        : lhs(e),
          wshift(shift / word_size()),
          offset(shift % word_size()),
          sub_offset(word_size() - offset),
          limit(word_count() - wshift - 1) {}

    [[gnu::always_inline, nodiscard]] constexpr word_type word(usize wpos) const {
        if (wpos > limit) return 0;
        if (offset == 0) return lhs.word(wpos + wshift);

        if (wpos == limit) return lhs.word(word_count - 1) >> offset;
        return (lhs.word(wpos + wshift) >> offset) | (lhs.word(wpos + wshift + 1) << (sub_offset));
    }
};

template <usize N, typename WordT, typename LExprT, typename RExprT, order LOrd, order ROrd>
[[gnu::always_inline, nodiscard]] constexpr auto operator|(const expr<N, WordT, LExprT, LOrd> &lhs,
                                                           const expr<N, WordT, RExprT, ROrd> &rhs) {
    return binary_expr<N, WordT, LExprT, RExprT, std::bit_or<WordT>>(lhs.self(), rhs.self());
}

template <usize N, typename WordT, typename LExprT, typename RExprT, order LOrd, order ROrd>
[[gnu::always_inline, nodiscard]] constexpr auto operator&(const expr<N, WordT, LExprT, LOrd> &lhs,
                                                           const expr<N, WordT, RExprT, ROrd> &rhs) {
    return binary_expr<N, WordT, LExprT, RExprT, std::bit_and<WordT>>(lhs.self(), rhs.self());
}

template <usize N, typename WordT, typename LExprT, typename RExprT, order LOrd, order ROrd>
[[gnu::always_inline, nodiscard]] constexpr auto operator^(const expr<N, WordT, LExprT, LOrd> &lhs,
                                                           const expr<N, WordT, RExprT, ROrd> &rhs) {
    return binary_expr<N, WordT, LExprT, RExprT, std::bit_xor<WordT>>(lhs.self(), rhs.self());
}

template <usize N, typename WordT, typename ExprT, order Ord>
[[gnu::always_inline, nodiscard]] constexpr auto operator~(const expr<N, WordT, ExprT, Ord> &lhs) {
    return not_expr<N, WordT, ExprT>(lhs.self());
}

template <usize N, typename WordT, typename LExprT, typename RExprT, order LOrd, order ROrd>
[[gnu::always_inline, nodiscard]] constexpr auto operator-(const expr<N, WordT, LExprT, LOrd> &lhs,
                                                           const expr<N, WordT, RExprT, ROrd> &rhs) {
    return lhs & ~rhs;
}

template <usize N, typename WordT, typename ExprT, order Ord>
[[gnu::always_inline, nodiscard]] constexpr auto operator<<(const expr<N, WordT, ExprT, Ord> &lhs, usize shift) {
    return shl_expr<N, WordT, ExprT>(lhs.self(), shift);
}

template <usize N, typename WordT, typename ExprT, order Ord>
[[gnu::always_inline, nodiscard]] constexpr auto operator>>(const expr<N, WordT, ExprT, Ord> &lhs, usize shift) {
    return shr_expr<N, WordT, ExprT>(lhs.self(), shift);
}

template <usize N, typename WordT = u64>
class bitset : public expr<N, WordT, bitset<N, WordT>, order::forward> {
    using base_type = expr<N, WordT, bitset<N, WordT>, order::forward>;
    using word_type = typename base_type::word_type;

    using base_type::has_excess_bits;
    using base_type::mask;
    using base_type::size;
    using base_type::word_count;
    using base_type::word_size;

    using base_type::maskbit;
    using base_type::whichbit;
    using base_type::whichword;

    using storage_type = std::array<word_type, word_count()>;
    storage_type d{};

    template <typename ExprT, order Ord>
    [[gnu::always_inline]] constexpr void materialize(const expr<N, WordT, ExprT, Ord> &expr) {
        if constexpr (Ord == order::forward)
            std::copy(expr.cwbegin(), expr.cwend(), wbegin());
        else
            std::copy_backward(expr.cwbegin(), expr.cwend(), wend());
    }

    // WARNING: Assumes that bitset is zeroed beforehand
    template <typename CharT, typename Traits>
    [[gnu::always_inline]] constexpr void from_string(std::basic_string_view<CharT, Traits> str,
                                                      CharT zero = CharT('0'), CharT one = CharT('1')) {
        const usize n = std::min(size(), str.size());
        for (usize i = 0, j = n; i < n; ++i) {
            const CharT c = str[--j];

            // Really slow to check!!
            // assert(Traits::eq(c, zero) || Traits::eq(c, one));

            set(i, Traits::eq(c, one));
        }
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
            *word_ptr |= maskbit(pos);
        }

        [[gnu::always_inline]] constexpr void unset() {
            *word_ptr &= ~maskbit(pos);
        }

        [[gnu::always_inline]] constexpr void flip() {
            *word_ptr ^= maskbit(pos);
        }

    private:
        constexpr reference(word_type *w_ptr, usize p)
            : word_ptr(w_ptr), pos(p) {}

        [[gnu::always_inline, nodiscard]] static constexpr word_type maskbit(usize pos) {
            return static_cast<word_type>(1) << pos;
        }

        word_type *const word_ptr;
        const usize pos;

        friend class bitset;
    };

    using base_type::operator[];

    [[gnu::always_inline, nodiscard]] constexpr reference operator[](usize pos) {
        return reference(&d[whichword(pos)], whichbit(pos));
    }

    class iterator {
    public:
        using self_type = iterator;

        using difference_type = isize;
        using value_type = bitset<N, WordT>::reference;
        using reference = void;
        using iterator_category = std::random_access_iterator_tag;

        [[gnu::always_inline]] constexpr self_type &operator+=(difference_type n) & {
            pos += n;
            return *this;
        }

        [[gnu::always_inline]] constexpr self_type &operator-=(difference_type n) & {
            pos -= n;
            return *this;
        }

        [[gnu::always_inline]] constexpr self_type &operator++() & {
            ++pos;
            return *this;
        }

        [[gnu::always_inline]] constexpr self_type &operator--() & {
            --pos;
            return *this;
        }

        [[gnu::always_inline, nodiscard]] constexpr self_type operator++(i32) & {
            self_type tmp = *this;
            operator++();
            return tmp;
        }

        [[gnu::always_inline, nodiscard]] constexpr self_type operator--(i32) & {
            self_type tmp = *this;
            operator--();
            return tmp;
        }

        [[gnu::always_inline, nodiscard]] constexpr value_type operator*() const {
            return operator[](0);
        }

        [[gnu::always_inline, nodiscard]] constexpr value_type operator[](difference_type n) const {
            return bitset_ptr->operator[](pos + n);
        }

        [[gnu::always_inline, nodiscard]] friend constexpr self_type operator+(self_type lhs, difference_type n) {
            return lhs += n;
        }

        [[gnu::always_inline, nodiscard]] friend constexpr self_type operator-(self_type lhs, difference_type n) {
            return lhs -= n;
        }

        [[gnu::always_inline, nodiscard]] friend constexpr difference_type operator-(const self_type &lhs,
                                                                                     const self_type &rhs) {
            return static_cast<difference_type>(lhs.pos) - rhs.pos;
        }

        // WARNING: Comparison of underlying bitset is not performed
        [[gnu::always_inline, nodiscard]] constexpr bool operator==(const self_type &other) const {
            return pos == other.pos;
        }

    private:
        iterator(bitset *const b_ptr, usize p)
            : bitset_ptr(b_ptr), pos(p) {}

        bitset *const bitset_ptr;
        usize pos;

        friend class bitset;
    };

    using reverse_iterator = std::reverse_iterator<iterator>;

    using word_iterator = typename storage_type::iterator;
    using const_word_iterator = typename storage_type::const_iterator;

    using reverse_word_iterator = typename storage_type::reverse_iterator;
    using const_reverse_word_iterator = typename storage_type::const_reverse_iterator;

    [[gnu::always_inline, nodiscard]] constexpr iterator begin() {
        return iterator(this, 0);
    }

    [[gnu::always_inline, nodiscard]] constexpr iterator end() {
        return iterator(this, size());
    }

    [[gnu::always_inline, nodiscard]] constexpr reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    [[gnu::always_inline, nodiscard]] constexpr reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    [[gnu::always_inline, nodiscard]] constexpr word_iterator wbegin() {
        return d.begin();
    }

    [[gnu::always_inline, nodiscard]] constexpr word_iterator wend() {
        return d.end();
    }

    [[gnu::always_inline, nodiscard]] constexpr const_word_iterator cwbegin() const {
        return d.cbegin();
    }

    [[gnu::always_inline, nodiscard]] constexpr const_word_iterator cwend() const {
        return d.cend();
    }

    [[gnu::always_inline, nodiscard]] constexpr reverse_word_iterator rwbegin() {
        return d.rbegin();
    }

    [[gnu::always_inline, nodiscard]] constexpr reverse_word_iterator rwend() {
        return d.rend();
    }

    [[gnu::always_inline, nodiscard]] constexpr const_reverse_word_iterator crwbegin() const {
        return d.crbegin();
    }

    [[gnu::always_inline, nodiscard]] constexpr const_reverse_word_iterator crwend() const {
        return d.crend();
    }

    template <typename ExprT, order Ord>
    [[gnu::always_inline]] explicit constexpr bitset(const expr<N, WordT, ExprT, Ord> &expr) {
        materialize(expr);
    }

    template <typename CharT, typename Traits>
    [[gnu::always_inline]] explicit constexpr bitset(std::basic_string_view<CharT, Traits> str, CharT zero = CharT('0'),
                                                     CharT one = CharT('1')) {
        from_string(str, zero, one);
    }

    template <typename ExprT, order Ord>
    [[gnu::always_inline]] constexpr bitset &operator=(const expr<N, WordT, ExprT, Ord> &expr) {
        materialize(expr);
        return *this;
    }

    [[gnu::always_inline]] constexpr void swap(bitset &other) noexcept {
        std::swap(d, other.d);
    }

    template <typename ExprT, order Ord>
    [[gnu::always_inline]] constexpr bitset &operator|=(const expr<N, WordT, ExprT, Ord> &expr) & {
        return *this = *this | expr;
    }

    template <typename ExprT, order Ord>
    [[gnu::always_inline]] constexpr bitset &operator&=(const expr<N, WordT, ExprT, Ord> &expr) & {
        return *this = *this & expr;
    }

    template <typename ExprT, order Ord>
    [[gnu::always_inline]] constexpr bitset &operator^=(const expr<N, WordT, ExprT, Ord> &expr) & {
        return *this = *this ^ expr;
    }

    template <typename ExprT, order Ord>
    [[gnu::always_inline]] constexpr bitset &operator-=(const expr<N, WordT, ExprT, Ord> &expr) & {
        return *this = *this - expr;
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

    [[gnu::always_inline, nodiscard]] constexpr word_type word(usize wpos) const {
        return d[wpos];
    }

    // WARNING: Assumes that bitset is zeroed beforehand
    [[gnu::always_inline]] constexpr void set(usize pos, bool val) {
        d[whichword(pos)] |= static_cast<word_type>(val) << whichbit(pos);
    }

    [[gnu::always_inline]] constexpr void set(usize pos) {
        d[whichword(pos)] |= maskbit(pos);
    }

    [[gnu::always_inline]] constexpr void unset(usize pos) {
        d[whichword(pos)] &= ~maskbit(pos);
    }

    [[gnu::always_inline]] constexpr void flip(usize pos) {
        d[whichword(pos)] ^= maskbit(pos);
    }

    [[gnu::always_inline]] constexpr void set() {
        if constexpr (!has_excess_bits()) return std::fill(d.begin(), d.end(), static_cast<word_type>(-1));

        auto last = std::prev(d.end());
        std::fill(d.begin(), last, static_cast<word_type>(-1));

        *last = mask();
    }

    [[gnu::always_inline]] constexpr void unset() {
        std::fill(d.begin(), d.end(), static_cast<word_type>(0));
    }

    [[gnu::always_inline]] constexpr void flip() {
        operator~();
    }

    template <typename CharT = char, typename Traits = std::char_traits<CharT>,
              typename Allocator = std::allocator<CharT>>
    [[gnu::always_inline, nodiscard]] std::basic_string<CharT, Traits, Allocator>
    to_string(CharT zero = CharT('0'), CharT one = CharT('1')) const {
        std::basic_string<CharT, Traits, Allocator> s(size(), 0);
        for (usize i = 0, j = size(); i < size(); ++i) s[--j] = operator[](i) ? one : zero;

        return s;
    }

    // WARNING: Assumes that bitset is zeroed beforehand
    [[gnu::always_inline]] friend std::istream &operator>>(std::istream &is, bitset &b) {
        std::string s;
        s.reserve(size());

        is >> s;
        b.from_string(std::string_view(s));

        return is;
    }

    [[gnu::always_inline]] friend std::ostream &operator<<(std::ostream &os, const bitset &b) {
        os << b.to_string();
        return os;
    }
};

namespace std {
template <usize N, typename WordT>
constexpr void swap(::bitset<N, WordT> &a, ::bitset<N, WordT> &b) noexcept {
    a.swap(b);
}
} // namespace std

#endif // LIB_BITSET_HPP
