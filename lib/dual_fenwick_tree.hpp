#ifndef LIB_DUAL_FENWICK_TREE_HPP
#define LIB_DUAL_FENWICK_TREE_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_abelian_group_t<Monoid> * = nullptr>
struct dual_fenwick_tree {
    using MA = Monoid;
    using A = typename MA::ValueT;

    i32 n;
    std::vector<A> d;

    dual_fenwick_tree() {}

    explicit dual_fenwick_tree(i32 m) { build(m); }

    void build(i32 m) {
        n = m;
        d.assign(n, MA::unit());
    }

    A get(i32 p) const {
        assert(0 <= p && p < n);

        A x = MA::unit();
        for (++p; p <= n; p += p & -p) x = MA::op(x, d[p - 1]);

        return x;
    }

    void apply(i32 l, i32 r, A a) {
        assert(0 <= l && l <= r && r <= n);

        A b = MA::inv(a);
        for (; l < r; r -= r & -r) d[r - 1] = MA::op(a, d[r - 1]);
        for (; r < l; l -= l & -l) d[l - 1] = MA::op(b, d[l - 1]);
    }
};

#endif // LIB_DUAL_FENWICK_TREE_HPP
