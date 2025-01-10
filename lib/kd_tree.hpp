#ifndef LIB_KD_TREE_HPP
#define LIB_KD_TREE_HPP 1

#include <algorithm>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename PT, typename Monoid, u32 K = std::tuple_size<PT>::value, is_monoid_t<Monoid> * = nullptr>
struct kd_tree {
    using MX = Monoid;
    using X = typename MX::ValueT;

    template <usize... Is>
    PT min_pt(const PT &a, const PT &b, std::index_sequence<Is...>) {
        return {std::min(std::get<Is>(a), std::get<Is>(b))...};
    }
    template <usize... Is>
    PT max_pt(const PT &a, const PT &b, std::index_sequence<Is...>) {
        return {std::max(std::get<Is>(a), std::get<Is>(b))...};
    }

    template <usize... Is>
    bool cle_pt_impl(const PT &a, const PT &b, std::index_sequence<Is...>) {
        return ((std::get<Is>(a) <= std::get<Is>(b)) && ...);
    }

    bool cle_pt(const PT &a, const PT &b) {
        return cle_pt_impl(a, b, std::make_index_sequence<K>{});
    }

    struct node {
        // Vector sp -> ep describes the hypercube.
        PT sp, ep;
        X val;

        node() {}
        node(const PT &s, const PT &e, X x = MX::unit())
            : sp(s), ep(e), val(x) {}
    };

    i32 n;
    std::vector<node> d;
    std::vector<i32> rnk;

    kd_tree() {}
    kd_tree(i32 z, const std::vector<PT> &p, const std::vector<X> &s) {
        n = z;
        rnk.assign(n, 0);

        i32 m = 1;
        while (m < n) m += m;

        d.resize(m * 2);

        std::vector<i32> idx(n);
        for (i32 i = 0; i < n; i++) idx[i] = i;

        build_rec<0>(p, s, idx, 0, n, 1);
    }

    // Vector st -> ed describes a hypercube.
    X prod(const PT &st, const PT &ed) {
        return prod_rec(st, ed, 0, n, 1);
    }

    X get(i32 x) {
        return get_rec(x, 0, n, 1);
    }

    void set(i32 x, X v) {
        set_rec(rnk[x], v, 0, n, 1);
    }

    template <i32 D>
    void build_rec(const std::vector<PT> &p, const std::vector<X> &s, std::vector<i32> &idx, i32 l, i32 r, i32 x) {
        if (l + 1 == r) {
            i32 idn = idx[l];
            d[x] = {p[idn], p[idn], s[idn]};
            rnk[idn] = l;
            return;
        }

        const i32 mid = (l + r) >> 1;
        std::nth_element(idx.data() + l, idx.data() + mid, idx.data() + r,
                         [&p](i32 a, i32 b) { return std::get<D>(p[a]) < std::get<D>(p[b]); });

        build_rec<(D + 1) % K>(p, s, idx, l, mid, x * 2);
        build_rec<(D + 1) % K>(p, s, idx, mid, r, x * 2 + 1);

        d[x].sp = min_pt(d[x * 2].sp, d[x * 2 + 1].sp, std::make_index_sequence<K>{});
        d[x].ep = max_pt(d[x * 2].ep, d[x * 2 + 1].ep, std::make_index_sequence<K>{});
        d[x].val = MX::op(d[x * 2].val, d[x * 2 + 1].val);
    }

private:
    X prod_rec(const PT &st, const PT &ed, i32 l, i32 r, i32 x) {
        if (cle_pt(st, d[x].sp) && cle_pt(d[x].ep, ed)) return d[x].val;
        if (!cle_pt(d[x].sp, ed) || !cle_pt(st, d[x].ep)) return MX::unit();

        const i32 mid = (l + r) >> 1;
        return MX::op(prod_rec(st, ed, l, mid, x * 2), prod_rec(st, ed, mid, r, x * 2 + 1));
    }

    void set_rec(i32 p, X v, i32 l, i32 r, i32 x) {
        if (l + 1 == r) {
            d[x].val = v;
            return;
        }

        const i32 mid = (l + r) >> 1;

        if (p < mid)
            set_rec(p, v, l, mid, x * 2);
        else
            set_rec(p, v, mid, r, x * 2 + 1);

        d[x].val = MX::op(d[x * 2].val, d[x * 2 + 1].val);
    }

    X get_rec(i32 p, i32 l, i32 r, i32 x) {
        if (l + 1 == r) return d[x].val;

        const i32 mid = (l + r) >> 1;
        if (p < mid) return get_rec(p, l, mid, x * 2);
        return get_rec(p, mid, r, x * 2 + 1);
    }
};

template <typename PT, typename Monoid, u32 K = std::tuple_size<PT>::value, is_monoid_t<Monoid> * = nullptr>
struct kd_forest {
    using MX = Monoid;
    using X = typename MX::ValueT;

    using KDT = kd_tree<PT, MX, K>;

    std::vector<KDT> f;

    kd_forest() {}
    void push_back(const PT &v, X w) {
        i32 ns = 1;

        std::vector<PT> p(1, v);
        std::vector<X> s(1, w);

        while (!f.empty() && f.back().n == ns) {
            p.resize(ns * 2), s.resize(ns * 2);
            auto &nw = f.back();

            for (i32 i = 0; i < ns; i++) {
                auto &d = nw.d[nw.rnk[i] + ns];
                p[ns * 2 - i - 1] = d.sp;
                s[ns * 2 - i - 1] = d.val;
            }

            ns *= 2;
            f.pop_back();
        }

        std::reverse(p.begin(), p.end());
        std::reverse(s.begin(), s.end());

        f.emplace_back(ns, p, s);
    }

    X prod(const PT &st, const PT &ed) {
        X res = MX::unit();
        for (auto &d : f) res = MX::op(res, d.prod(st, ed));

        return res;
    }

    void set(i32 x, X s) {
        for (auto &d : f) {
            if (x < d.n) {
                d.set(x, s);
                break;
            }

            x -= d.n;
        }
    }

    X get(i32 x) {
        for (auto &d : f) {
            if (x < d.n) return d.get(x);
            x -= d.n;
        }

        return MX::unit();
    }
};

#endif // LIB_KD_TREE_HPP
