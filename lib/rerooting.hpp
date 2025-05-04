#ifndef LIB_REROOTING_HPP
#define LIB_REROOTING_HPP 1

#include <vector>

#include <lib/prelude.hpp>

template <typename Rerooter>
struct rerooting {
    using RR = Rerooter;
    using V = typename RR::NodeT;
    using E = typename RR::EdgeT;

    std::vector<std::vector<V>> suml, sumr;
    std::vector<V> sum, psum;

    rerooting() {}

    template <typename Tree>
    rerooting(const Tree &g, const std::vector<V> &x) {
        build(g, x);
    }

    template <typename Tree>
    void build(const Tree &g, const std::vector<V> &x) {
        const i32 n = static_cast<i32>(g.size());

        suml.resize(n);
        sumr.resize(n);

        sum = x;
        psum.resize(n, RR::unit());

        dfs_up(g, x, 0);
        dfs_down(g, x, 0);

        for (i32 u = 0; u < n; ++u) {
            if (suml[u].empty()) continue;
            sum[u] = RR::parent(x[u], RR::sibling(suml[u].back(), psum[u]));
        }
    }

    template <typename Tree>
    V dfs_up(const Tree &g, const std::vector<V> &x, i32 u, i32 t = -1) {
        const i32 sz = static_cast<i32>(g[u].size());
        suml[u].resize(sz, RR::unit());

        V res = RR::unit();

        i32 i = -1;
        for (const i32 v : g[u]) {
            ++i;
            if (v == t) continue;

            suml[u][i] = RR::up(v, dfs_up(g, x, v, u));
            res = RR::sibling(res, suml[u][i]);
        }

        return RR::parent(x[u], res);
    }

    template <typename Tree>
    void dfs_down(const Tree &g, const std::vector<V> &x, i32 u, i32 t = -1) {
        const i32 sz = static_cast<i32>(g[u].size());
        sumr[u] = suml[u];

        for (i32 i = sz - 1; i > 0; --i) sumr[u][i - 1] = RR::sibling(sumr[u][i - 1], sumr[u][i]);
        for (i32 i = 1; i < sz; ++i) suml[u][i] = RR::sibling(suml[u][i], suml[u][i - 1]);

        i32 i = -1;
        for (const i32 v : g[u]) {
            ++i;
            if (v == t) continue;

            const V left = i == 0 ? RR::unit() : suml[u][i - 1];
            const V right = i == sz - 1 ? RR::unit() : sumr[u][i + 1];
            const V mid = RR::sibling(left, RR::sibling(psum[u], right));

            psum[v] = RR::up(v, RR::parent(x[u], mid));
            dfs_down(g, x, v, u);
        }
    }
};

#endif // LIB_REROOTING_HPP
