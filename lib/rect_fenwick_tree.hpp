#ifndef LIB_RECT_FENWICK_TREE_HPP
#define LIB_RECT_FENWICK_TREE_HPP 1

#include <algorithm>
#include <vector>

#include <lib/fenwick_tree.hpp>
#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename T, typename Monoid, is_abelian_group_t<Monoid> * = nullptr> struct rect_fenwick_tree {
    using MX = Monoid;
    using X = typename MX::ValueT;

    i32 n;

    using pt = std::pair<T, T>;
    std::vector<pt> pts;
    std::vector<std::vector<T>> ys;
    std::vector<fenwick_tree<Monoid>> fw;

    rect_fenwick_tree() {}

    explicit rect_fenwick_tree(i32 m) { pts.reserve(m); }

    void add_point(T x, T y) { pts.emplace_back(x, y); }

    void build() {
        std::sort(pts.begin(), pts.end());
        pts.erase(std::unique(pts.begin(), pts.end()), pts.end());

        n = static_cast<i32>(pts.size());
        fw.resize(n + 1);
        ys.resize(n + 1);

        for (i32 i = 0; i <= n; ++i) {
            for (i32 j = i + 1; j <= n; j += (j & -j)) ys[j].push_back(pts[i].second);

            std::sort(ys[i].begin(), ys[i].end());
            ys[i].erase(std::unique(ys[i].begin(), ys[i].end()), ys[i].end());

            fw[i].build(static_cast<i32>(ys[i].size()));
        }
    }

    void multiply(T x, T y, X v) {
        i32 i = static_cast<i32>(std::lower_bound(pts.begin(), pts.end(), pt{x, y}) - pts.begin());
        assert(i < n && pts[i].first == x && pts[i].second == y);

        for (++i; i <= n; i += (i & -i)) fw[i].multiply(id(i, y), v);
    }

    void set(T x, T y, X v) { multiply(x, y, MX::op(MX::inv(get(x, y)), v)); }

    X prod(T xr, T yr) const {
        X r = MX::unit();
        for (i32 a = id(xr); a > 0; a -= (a & -a)) r = MX::op(r, fw[a].prod(id(a, yr)));

        return r;
    }

    X prod(T xl, T xr, T yl, T yr) const {
        X vl = MX::unit();
        X vr = MX::unit();

        i32 a = id(xl);
        i32 b = id(xr);

        for (; a < b; b -= (b & -b)) vr = MX::op(vr, fw[b].prod(id(b, yl), id(b, yr)));
        for (; b < a; a -= (a & -a)) vl = MX::op(vl, fw[a].prod(id(a, yl), id(a, yr)));

        return MX::op(MX::inv(vl), vr);
    }

    X get(T x, T y) const { return prod(x, x + 1, y, y + 1); }

  private:
    inline i32 id(T x) const {
        const auto cmp = [](const pt &a, const pt &b) { return a.first < b.first; };
        return static_cast<i32>(std::lower_bound(pts.begin(), pts.end(), pt{x, T()}, cmp) - pts.begin());
    }

    inline i32 id(i32 i, T y) const {
        return static_cast<i32>(std::lower_bound(ys[i].begin(), ys[i].end(), y) - ys[i].begin());
    }
};

#endif // LIB_RECT_FENWICK_TREE_HPP
