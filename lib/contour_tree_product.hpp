#ifndef LIB_CONTOUR_TREE_PRODUCT_HPP
#define LIB_CONTOUR_TREE_PRODUCT_HPP 1

#include <algorithm>
#include <array>
#include <cassert>
#include <queue>
#include <random>
#include <utility>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/random.hpp>
#include <lib/type_traits.hpp>

template <typename RangeProduct, is_commutative_monoid_t<typename RangeProduct::MX> * = nullptr>
struct contour_tree_product {
    using MX = typename RangeProduct::MX;
    using X = typename MX::ValueT;

    using sequence_t = RangeProduct;
    using aux_info = std::pair<bool, i32>;

    struct tree_node {
        std::vector<i32> adj;
        i32 info;
        X d;
    };

    i32 n;
    std::vector<tree_node> nodes;
    std::vector<i32> par;
    std::vector<std::array<aux_info, 30>> info;
    std::vector<std::array<sequence_t, 2>> subtrees;

    std::vector<i32> ord;

    contour_tree_product() {}

    template <typename Graph>
    contour_tree_product(const Graph &g, const std::vector<X> &a) {
        build(g, a);
    }

    template <typename Graph, typename F>
    contour_tree_product(const Graph &g, F f) {
        build(g, f);
    }

    template <typename Graph>
    void build(const Graph &g, const std::vector<X> &a) {
        build(g, [&](i32 i) -> X { return a[i]; });
    }

    template <typename Graph, typename F>
    void build(const Graph &g, F f) {
        n = g.n;

        nodes.resize(n);
        par.assign(2 * n, -1);
        info.resize(n);
        subtrees.resize(2 * n);

        for (i32 u = 0; u < n; ++u) {
            nodes[u].adj.reserve(g[u].size());
            for (const i32 v : g[u]) nodes[u].adj.push_back(v);

            nodes[u].d = f(u);
        }

        const i32 s = std::uniform_int_distribution<i32>(0, n - 1)(MT);
        reorder(s);

        i32 new_node = n;
        std::vector<i32> sub_sz(2 * n), ctr(2 * n, -1);

        std::vector<i32> head(2 * n), tail(2 * n), link(2 * n);
        for (i32 i = 0; i < n; ++i) head[i] = tail[i] = i;

        std::vector<X> d(n, MX::unit());
        const auto rec = [&](auto &&dfs, i32 r, i32 sz) -> i32 {
            i32 c = -1;
            const auto get_centroid = [&](auto &&self, i32 u, i32 p) -> void {
                sub_sz[u] = 1;

                for (const i32 v : nodes[u].adj) {
                    if (v == p) continue;

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

            assert(c != -1);
            for (const i32 v : nodes[c].adj) {
                const i32 comp_sz = sub_sz[v];

                nodes[v].adj.erase(std::find(nodes[v].adj.begin(), nodes[v].adj.end(), c));
                ctr[v] = dfs(dfs, v, comp_sz);

                sub_sz[v] = comp_sz;
            }

            const auto cmp = [&](i32 i, i32 j) { return sub_sz[i] > sub_sz[j]; };
            std::priority_queue<i32, std::vector<i32>, decltype(cmp)> pq{cmp};

            for (const i32 v : nodes[c].adj) {
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
                    for (const i32 v : nodes[u].adj) {
                        if (v == pu) continue;

                        dq.emplace(v, u);
                        if (nxt == -1) nxt = v;
                    }
                }

                d[z++] = x;
                return z;
            };

            while (pq.size() >= 2) {
                const i32 u = pq.top();
                pq.pop();

                const i32 v = pq.top();
                pq.pop();

                if (pq.empty()) {
                    par[ctr[u]] = par[ctr[v]] = c;
                    subtrees[c][0].build(build_sequence(head[u], 0), [&](i32 i) -> X { return d[i]; });
                    subtrees[c][1].build(build_sequence(head[v], 1), [&](i32 i) -> X { return d[i]; });
                    break;
                }

                sub_sz[new_node] = sub_sz[u] + sub_sz[v];
                ctr[new_node] = new_node;

                par[ctr[u]] = par[ctr[v]] = new_node;
                subtrees[new_node][0].build(build_sequence(head[u], 0), [&](i32 i) -> X { return d[i]; });
                subtrees[new_node][1].build(build_sequence(head[v], 1), [&](i32 i) -> X { return d[i]; });

                head[new_node] = head[u];
                tail[new_node] = tail[v];
                link[tail[u]] = head[v];

                pq.push(new_node);
                ++new_node;
            }

            if (!pq.empty()) {
                const i32 u = pq.top();
                pq.pop();

                par[ctr[u]] = c;
                subtrees[c][0].build(build_sequence(head[u], 0), [&](i32 i) -> X { return d[i]; });
            }

            for (const i32 v : nodes[c].adj) nodes[v].adj.push_back(c);
            return c;
        };

        rec(rec, 0, n);

        par.resize(new_node);
        par.shrink_to_fit();

        subtrees.resize(new_node);
        subtrees.shrink_to_fit();
    }

    X get(i32 u) const { return nodes[ord[u]].d; }

    void set(i32 u, X x) {
        assert(0 <= u && u < n);

        u = ord[u];
        nodes[u].d = x;

        i32 v = par[u];
        for (i32 i = 0; i < nodes[u].info; ++i) {
            const auto &[b, dist] = info[u][i];
            subtrees[std::exchange(v, par[v])][b].set(dist, x);
        }
    }

    void multiply(i32 u, X x) {
        assert(0 <= u && u < n);

        u = ord[u];
        nodes[u].d = MX::op(nodes[u].d, x);

        i32 v = par[u];
        for (i32 i = 0; i < nodes[u].info; ++i) {
            const auto &[b, dist] = info[u][i];
            subtrees[std::exchange(v, par[v])][b].multiply(dist, x);
        }
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
    void reorder(i32 s) {
        ord.assign(n, -1);

        std::queue<i32> q;
        q.push(s);

        i32 t = 0;
        while (!q.empty()) {
            const i32 u = q.front();
            q.pop();

            ord[u] = t++;
            for (const i32 v : nodes[u].adj)
                if (ord[v] == -1) q.push(v);
        }

        assert(t == n);
        std::vector<tree_node> tmp(n);

        for (i32 u = 0; u < n; ++u) {
            for (i32 &v : nodes[u].adj) v = ord[v];
            tmp[ord[u]] = std::move(nodes[u]);
        }

        std::swap(nodes, tmp);
    }

    X prod(const sequence_t &seq, i32 l, i32 r) const {
        l = std::max(0, l);
        r = std::min(r, seq.n);

        return l < r ? seq.prod(l, r) : MX::unit();
    };
};

#endif // LIB_CONTOUR_TREE_PRODUCT_HPP
