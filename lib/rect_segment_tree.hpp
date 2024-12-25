#ifndef LIB_RECT_SEGMENT_TREE_HPP
#define LIB_RECT_SEGMENT_TREE_HPP 1

#include <algorithm>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/segment_tree.hpp>
#include <lib/type_traits.hpp>

template <typename T, typename Monoid, is_commutative_monoid_t<Monoid> * = nullptr>
struct rect_segment_tree {
    using MX = Monoid;
    using X = typename MX::ValueT;

    i32 n;

    using pt = std::pair<T, T>;
    std::vector<pt> pts;
    std::vector<std::vector<pt>> rs;
    std::vector<segment_tree<Monoid>> sg;

    rect_segment_tree() {}

    explicit rect_segment_tree(i32 m) { pts.reserve(m); }

    void add_point(T x, T y) { pts.emplace_back(x, y); }

    void build() {
        std::sort(pts.begin(), pts.end());
        pts.erase(std::unique(pts.begin(), pts.end()), pts.end());

        n = static_cast<i32>(pts.size());
        rs.resize(2 * n);
        sg.resize(2 * n);

        for (i32 i = 0; i < n; ++i) rs[n + i] = {{pts[i].second, pts[i].first}};
        for (i32 i = n - 1; i >= 0; --i) {
            const auto &l = rs[(i << 1)];
            const auto &r = rs[(i << 1) + 1];

            std::merge(l.begin(), l.end(), r.begin(), r.end(), std::back_inserter(rs[i]));
            rs[i].erase(std::unique(rs[i].begin(), rs[i].end()), rs[i].end());
        }

        for (i32 i = 0; i < 2 * n; ++i) sg[i].build(static_cast<i32>(rs[i].size()));
    }

    void set(T x, T y, X w) {
        i32 i = static_cast<i32>(std::lower_bound(pts.begin(), pts.end(), pt{x, y}) - pts.begin());
        assert(i < n && pts[i].first == x && pts[i].second == y);

        for (i += n; i; i >>= 1) set(i, {x, y}, w);
    }

    void multiply(T x, T y, X w) {
        i32 i = static_cast<i32>(std::lower_bound(pts.begin(), pts.end(), pt{x, y}) - pts.begin());
        assert(i < n && pts[i].first == x && pts[i].second == y);

        for (i += n; i; i >>= 1) multiply(i, {x, y}, w);
    }

    X prod(T xl, T xr, T yl, T yr) const {
        const auto cmp = [](const pt &l, const pt &r) { return l.first < r.first; };

        i32 l = static_cast<i32>(std::lower_bound(pts.begin(), pts.end(), pt{xl, yr}, cmp) - pts.begin());
        i32 r = static_cast<i32>(std::lower_bound(pts.begin(), pts.end(), pt{xr, yr}, cmp) - pts.begin());

        l += n;
        r += n;

        X x = MX::unit();
        while (l < r) {
            if (l & 1) x = MX::op(x, prod(l++, yl, yr));
            if (r & 1) x = MX::op(prod(--r, yl, yr), x);

            l >>= 1;
            r >>= 1;
        }

        return x;
    }

    X get(T x, T y) const { return prod(x, x + 1, y + 1); }

private:
    void set(i32 v, const pt &p, X x) {
        const i32 i =
            static_cast<i32>(std::lower_bound(rs[v].begin(), rs[v].end(), pt{p.second, p.first}) - rs[v].begin());
        sg[v].set(i, x);
    }

    void multiply(i32 v, const pt &p, X x) {
        const i32 i =
            static_cast<i32>(std::lower_bound(rs[v].begin(), rs[v].end(), pt{p.second, p.first}) - rs[v].begin());
        sg[v].multiply(i, x);
    }

    X prod(i32 v, T yl, T yr) const {
        const auto cmp = [&](const pt &l, const pt &r) { return l.first < r.first; };

        const i32 l = static_cast<i32>(std::lower_bound(rs[v].begin(), rs[v].end(), pt{yl, yl}, cmp) - rs[v].begin());
        const i32 r = static_cast<i32>(std::lower_bound(rs[v].begin(), rs[v].end(), pt{yr, yr}, cmp) - rs[v].begin());

        return sg[v].prod(l, r);
    }
};

#endif // LIB_RECT_SEGMENT_TREE_HPP
