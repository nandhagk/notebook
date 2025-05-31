#ifndef LIB_WARY_TREE_HPP
#define LIB_WARY_TREE_HPP 1

#include <cassert>
#include <string>
#include <vector>

#include <lib/prelude.hpp>

struct wary_tree {
    static constexpr u32 B = 64;

    i32 n, log;
    std::vector<std::vector<u64>> seg;

    wary_tree() {}
    wary_tree(i32 m) {
        build(m);
    }

    template <typename F>
    wary_tree(i32 m, F f) {
        build(m, f);
    }

    void build(i32 m) {
        seg.clear();
        n = m;
        do {
            seg.emplace_back((m + B - 1) / B);
            m = (m + B - 1) / B;
        } while (m > 1);
        log = static_cast<i32>(seg.size());
    }

    template <typename F>
    void build(i32 m, F f) {
        build(m);

        for (i32 i = 0; i < n; ++i) seg[0][i / B] |= u64(f(i)) << (i % B);
        for (i32 h = 0; h < log - 1; ++h)
            for (i32 i = 0; i < static_cast<i32>(seg[h].size()); ++i)
                seg[h + 1][i / B] |= u64(bool(seg[h][i])) << (i % B);
    }

    bool operator[](i32 i) const {
        return seg[0][i / B] >> (i % B) & 1;
    }

    void insert(i32 i) {
        assert(0 <= i && i < n);
        for (i32 h = 0; h < log; h++) seg[h][i / B] |= u64(1) << (i % B), i /= B;
    }

    void erase(i32 i) {
        assert(0 <= i && i < n);
        u64 x = 0;
        for (i32 h = 0; h < log; h++) {
            seg[h][i / B] &= ~(u64(1) << (i % B));
            seg[h][i / B] |= x << (i % B);
            x = bool(seg[h][i / B]);
            i /= B;
        }
    }

    i32 next(i32 i) {
        assert(0 <= i && i <= n);

        for (i32 h = 0; h < log; ++h) {
            if (i / B == seg[h].size()) break;

            u64 d = seg[h][i / B] >> (i % B);
            if (!d) {
                i = i / B + 1;
                continue;
            }

            i += lowbit(d);
            for (i32 g = h - 1; g >= 0; --g) {
                i *= B;
                i += lowbit(seg[g][i / B]);
            }

            return i;
        }

        return n;
    }

    i32 prev(i32 i) {
        assert(0 <= i && i <= n);

        for (i32 h = 0; h < log; ++h) {
            if (i == -1) break;

            u64 d = seg[h][i / B] << (63 - i % B);
            if (!d) {
                i = i / B - 1;
                continue;
            }

            i -= __builtin_clzll(d);
            for (i32 g = h - 1; g >= 0; --g) {
                i *= B;
                i += topbit(seg[g][i / B]);
            }

            return i;
        }

        return -1;
    }

    bool any(i32 l, i32 r) {
        return next(l) < r;
    }

    template <typename F>
    void enumerate(i32 l, i32 r, F f) {
        for (i32 x = next(l); x < r; x = next(x + 1)) f(x);
    }

    std::string to_string() {
        std::string s(n, '?');
        for (i32 i = 0; i < n; ++i) s[i] = ((*this)[i] ? '1' : '0');
        return s;
    }
};

#endif // LIB_WARY_TREE_HPP
