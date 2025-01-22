#ifndef LIB_CTD_PRODUCT_HPP
#define LIB_CTD_PRODUCT_HPP 1

#include <algorithm>
#include <array>
#include <cassert>
#include <queue>
#include <random>
#include <utility>
#include <vector>

#include <lib/csr_graph.hpp>
#include <lib/prelude.hpp>
#include <lib/random.hpp>
#include <lib/type_traits.hpp>

template <typename RangeProduct, is_commutative_monoid_t<typename RangeProduct::MX> * = nullptr>
struct ctd_product {
    using MX = typename RangeProduct::MX;
    using X = typename MX::ValueT;

    using sequence_t = RangeProduct;
    using aux_info = std::pair<bool, i32>;

    struct tree_node {
        i32 info;
        X d;
    };

    i32 n;
    std::vector<tree_node> nodes;
    std::vector<i32> par;
    std::vector<std::array<aux_info, 30>> info;
    std::vector<std::array<sequence_t, 2>> subtrees;

    std::vector<i32> ord;

    ctd_product() {}

    template <typename Graph>
    explicit ctd_product(const Graph &g) {
        build(g);
    }

    template <typename Graph>
    ctd_product(const Graph &g, const std::vector<X> &a) {
        build(g, a);
    }

    template <typename Graph, typename F>
    ctd_product(const Graph &g, F f) {
        build(g, f);
    }

    template <typename Graph>
    void build(const Graph &g) {
        build(g, [&](i32) -> X { return MX::unit(); });
    }

    template <typename Graph>
    void build(const Graph &g, const std::vector<X> &a) {
        build(g, [&](i32 i) -> X { return a[i]; });
    }

    template <typename Graph, typename F>
    void build(const Graph &g, F f) {
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

        std::vector<X> d(n, MX::unit());
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
                X x = MX::unit();

                i32 z = 0;
                i32 nxt = -1;

                while (!dq.empty()) {
                    const auto [u, pu] = dq.front();
                    dq.pop();

                    if (u == nxt) {
                        d[z++] = std::exchange(x, MX::unit());
                        nxt = -1;
                    }

                    info[u][nodes[u].info++] = {child_index, z};

                    x = MX::op(x, nodes[u].d);
                    for (const i32 v : h[u]) {
                        if (v == pu || rm[v]) continue;

                        dq.emplace(v, u);
                        if (nxt == -1) nxt = v;
                    }
                }

                d[z++] = x;
                return z;
            };

            const auto fill = [&](i32 i) -> X { return d[i]; };

            while (pq.size() >= 2) {
                const i32 u = pq.top();
                pq.pop();

                const i32 v = pq.top();
                pq.pop();

                if (pq.empty()) {
                    par[ctr[u]] = par[ctr[v]] = c;
                    subtrees[c][0].build(build_sequence(head[u], 0), fill);
                    subtrees[c][1].build(build_sequence(head[v], 1), fill);
                    break;
                }

                sub_sz[w] = sub_sz[u] + sub_sz[v];
                ctr[w] = w;

                par[ctr[u]] = par[ctr[v]] = w;
                subtrees[w][0].build(build_sequence(head[u], 0), fill);
                subtrees[w][1].build(build_sequence(head[v], 1), fill);

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
                subtrees[c][0].build(build_sequence(head[u], 0), fill);
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

    X get(i32 u) const {
        return nodes[ord[u]].d;
    }

    void multiply(i32 u, const X &x) {
        assert(0 <= u && u < n);

        u = ord[u];
        nodes[u].d = MX::op(nodes[u].d, x);

        i32 v = par[u];
        for (i32 i = 0; i < nodes[u].info; ++i) {
            const auto &[b, dist] = info[u][i];
            subtrees[std::exchange(v, par[v])][b].multiply(dist, x);
        }
    }

    X prod(i32 u, i32 d) const {
        return prod(u, d, d + 1);
    }

    X prod(i32 u, i32 dl, i32 dr) const {
        assert(0 <= u && u < n);

        u = ord[u];

        X res = dl <= 0 && 0 < dr ? nodes[u].d : MX::unit();
        res = MX::op(res, prod(subtrees[u][0], dl - 1, dr - 1));
        res = MX::op(res, prod(subtrees[u][1], dl - 1, dr - 1));

        i32 v = par[u];
        for (i32 i = 0; i < nodes[u].info; ++i) {
            const auto &[b, dist] = info[u][i];

            const i32 ql = dl - dist - 1;
            const i32 qr = dr - dist - 1;

            if (v < n && ql <= 0 && 0 < qr) res = MX::op(res, nodes[v].d);
            res = MX::op(res, prod(subtrees[std::exchange(v, par[v])][b ^ 1], ql - 1, qr - 1));
        }

        return res;
    }

private:
    template <typename Graph>
    void reorder(const Graph &g, i32 s) {
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

    X prod(const sequence_t &seq, i32 l, i32 r) const {
        l = std::max(0, l);
        r = std::min(r, seq.n);

        return l < r ? seq.prod(l, r) : MX::unit();
    };
};

#endif // LIB_CTD_PRODUCT_HPP
