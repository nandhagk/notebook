#ifndef LIB_DUAL_CTD_PRODUCT_HPP
#define LIB_DUAL_CTD_PRODUCT_HPP 1

#include <algorithm>
#include <array>
#include <cassert>
#include <queue>
#include <random>
#include <utility>
#include <vector>

#include <lib/algebraic_traits.hpp>
#include <lib/csr_graph.hpp>
#include <lib/prelude.hpp>
#include <lib/random.hpp>

template <typename DualRangeProduct, is_commutative_monoid_t<typename DualRangeProduct::MA> * = nullptr>
struct dual_ctd_product {
    using MA = typename DualRangeProduct::MA;
    using A = typename MA::ValueT;

    using sequence_t = DualRangeProduct;
    using aux_info = std::pair<bool, i32>;

    struct tree_node {
        i32 info;
        A d;
    };

    i32 n;
    std::vector<tree_node> nodes;
    std::vector<i32> par;
    std::vector<std::array<aux_info, 30>> info;
    std::vector<std::array<sequence_t, 2>> subtrees;

    std::vector<i32> ord;

    dual_ctd_product() {}

    template <typename Tree>
    explicit dual_ctd_product(const Tree &g) {
        build(g);
    }

    template <typename Tree>
    dual_ctd_product(const Tree &g, const std::vector<A> &a) {
        build(g, a);
    }

    template <typename Tree, typename F>
    dual_ctd_product(const Tree &g, F f) {
        build(g, f);
    }

    template <typename Tree>
    void build(const Tree &g) {
        build(g, [&](i32) -> A { return MA::unit(); });
    }

    template <typename Tree>
    void build(const Tree &g, const std::vector<A> &a) {
        build(g, [&](i32 i) -> A { return a[i]; });
    }

    template <typename Tree, typename F>
    void build(const Tree &g, F f) {
        n = static_cast<i32>(g.size());

        nodes.resize(n);
        par.assign(2 * n, -1);
        info.resize(n);
        subtrees.resize(2 * n);

        reorder(g, std::uniform_int_distribution<i32>(0, n - 1)(MT));

        std::vector<std::pair<i32, simple_edge>> es;
        es.reserve(2 * n);
        for (i32 u = 0; u < n; ++u) {
            for (const i32 v : g[u]) es.emplace_back(ord[u], ord[v]);
            nodes[ord[u]].d = f(u);
        }

        csr_graph h(n, es);

        i32 w = n;
        std::vector<i32> sub_sz(2 * n), ctr(2 * n, -1);

        std::vector<i32> head(2 * n), tail(2 * n), link(2 * n);
        for (i32 i = 0; i < n; ++i) head[i] = tail[i] = i;

        std::vector<bool> rm(n);
        const auto rec = [&](auto &&dfs, i32 r, i32 sz) -> i32 {
            i32 c = -1;
            const auto get_centroid = [&](auto &&self, i32 u, i32 p) -> void {
                sub_sz[u] = 1;
                for (const i32 v : h[u]) {
                    if (v == p || rm[v]) continue;

                    self(self, v, u);
                    if (v == c) {
                        sub_sz[u] = sz - sub_sz[c];
                        break;
                    }

                    sub_sz[u] += sub_sz[v];
                }

                if (c == -1 && sub_sz[u] * 2 > sz) c = u;
            };

            get_centroid(get_centroid, r, -1);
            rm[c] = true;
            for (const i32 v : h[c]) {
                if (rm[v]) continue;

                const i32 comp_sz = sub_sz[v];
                ctr[v] = dfs(dfs, v, comp_sz);
                sub_sz[v] = comp_sz;
            }

            const auto cmp = [&](i32 i, i32 j) { return sub_sz[i] > sub_sz[j]; };
            std::priority_queue<i32, std::vector<i32>, decltype(cmp)> pq{cmp};

            for (const i32 v : h[c]) {
                if (rm[v]) continue;

                link[v] = -1;
                pq.push(v);
            }

            const auto build_sequence = [&](const i32 root_head, const bool child_index) {
                std::queue<std::pair<i32, i32>> dq;

                for (i32 root = root_head; root >= 0; root = link[root]) dq.emplace(root, -1);
                A x = MA::unit();

                i32 z = 0;
                i32 nxt = -1;

                while (!dq.empty()) {
                    const auto [u, pu] = dq.front();
                    dq.pop();

                    if (u == nxt) {
                        ++z;
                        nxt = -1;
                    }

                    info[u][nodes[u].info++] = {child_index, z};

                    x = MA::op(x, nodes[u].d);
                    for (const i32 v : h[u]) {
                        if (v == pu || rm[v]) continue;

                        dq.emplace(v, u);
                        if (nxt == -1) nxt = v;
                    }
                }

                return ++z;
            };

            while (pq.size() >= 2) {
                const i32 u = pq.top();
                pq.pop();

                const i32 v = pq.top();
                pq.pop();

                if (pq.empty()) {
                    par[ctr[u]] = par[ctr[v]] = c;
                    subtrees[c][0].build(build_sequence(head[u], 0));
                    subtrees[c][1].build(build_sequence(head[v], 1));
                    break;
                }

                sub_sz[w] = sub_sz[u] + sub_sz[v];
                ctr[w] = w;

                par[ctr[u]] = par[ctr[v]] = w;
                subtrees[w][0].build(build_sequence(head[u], 0));
                subtrees[w][1].build(build_sequence(head[v], 1));

                head[w] = head[u];
                tail[w] = tail[v];
                link[tail[u]] = head[v];

                pq.push(w);
                ++w;
            }

            if (!pq.empty()) {
                const i32 u = pq.top();
                pq.pop();

                par[ctr[u]] = c;
                subtrees[c][0].build(build_sequence(head[u], 0));
            }

            rm[c] = false;
            return c;
        };

        rec(rec, 0, n);

        par.resize(w);
        par.shrink_to_fit();

        subtrees.resize(w);
        subtrees.shrink_to_fit();
    }

    A get(i32 u) {
        assert(0 <= u && u < n);

        u = ord[u];
        A res = nodes[u].d;

        i32 v = par[u];
        for (i32 i = 0; i < nodes[u].info; ++i) {
            const auto &[b, dist] = info[u][i];
            res = MA::op(subtrees[std::exchange(v, par[v])][b].get(dist), res);
        }

        return res;
    }

    void apply(i32 u, i32 dl, i32 dr, const A &a) {
        assert(0 <= u && u < n);

        u = ord[u];

        if (dl <= 0 && 0 < dr) nodes[u].d = MA::op(a, nodes[u].d);
        apply(subtrees[u][0], dl - 1, dr - 1, a);
        apply(subtrees[u][1], dl - 1, dr - 1, a);

        i32 v = par[u];
        for (i32 i = 0; i < nodes[u].info; ++i) {
            const auto &[b, dist] = info[u][i];

            const i32 ql = dl - dist - 1;
            const i32 qr = dr - dist - 1;

            if (v < n && ql <= 0 && 0 < qr) nodes[v].d = MA::op(a, nodes[v].d);
            apply(subtrees[std::exchange(v, par[v])][b ^ 1], ql - 1, qr - 1, a);
        }
    }

private:
    template <typename Tree>
    void reorder(const Tree &g, i32 s) {
        ord.assign(n, -1);

        std::queue<i32> q;
        q.push(s);

        i32 t = 0;
        while (!q.empty()) {
            const i32 u = q.front();
            q.pop();

            ord[u] = t++;
            for (const i32 v : g[u])
                if (ord[v] == -1) q.push(v);
        }

        assert(t == n);
    }

    void apply(sequence_t &seq, i32 l, i32 r, const A &a) {
        l = std::max(0, l);
        r = std::min(r, seq.n);

        if (l < r) seq.apply(l, r, a);
    };
};

#endif // LIB_DUAL_CTD_PRODUCT_HPP
