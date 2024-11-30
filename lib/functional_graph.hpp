#ifndef LIB_FUNCTIONAL_GRAPH_HPP
#define LIB_FUNCTIONAL_GRAPH_HPP 1

#include <vector>
#include <cassert>

#include <lib/prelude.hpp>
#include <lib/dsu.hpp>
#include <lib/graph.hpp>
#include <lib/hld.hpp>

struct functional_graph {
        i32 n;
        std::vector<i32> to, root;
        hld<csr_graph<simple_edge>> h;

        functional_graph() {}
        explicit functional_graph(const std::vector<i32> &t) {
                build(t);
        }

        bool in_cycle(i32 u) const {
                assert(0 <= u && u < n);

                const i32 r = root[u];
                const i32 b = to[r];

                return h.is_ancestor(u, b);
        }

        i32 dist(i32 u, i32 v) const {
                assert(0 <= u && u < n);
                assert(0 <= v && v < n);

                if (h.is_ancestor(v, u)) return h.depth[u] - h.depth[v];

                const i32 r = root[u];
                const i32 b = to[r];

                return h.is_ancestor(v, b) ? h.depth[u] - h.depth[r] + h.depth[b] - h.depth[v] + 1 : -1;
        }

        i32 jump(i32 u, i32 k) const {
                assert(0 <= u && u < n);
                assert(0 <= k);

                const i32 d = h.depth[u];
                if (k <= d - 1) return h.jump(u, k);

                u = root[u];
                k -= d - 1;

                const i32 b = to[u];
                const i32 c = h.depth[b];

                k %= c;
                return k == 0 ? u : h.jump(b, k - 1);
        }

        void build(const std::vector<i32> &t) {
                n = static_cast<i32>(t.size());

                to = t;
                root.assign(n, 0);

                std::vector<std::pair<i32, simple_edge>> es;
                es.reserve(2 * n);

                dsu dsu(n);
                for (i32 u = 0; u < n; ++u) {
                        if (!dsu.merge(u, to[u])) root[u] = u;
                }

                for (i32 u = 0; u < n; ++u) {
                        if (root[u] == u) root[dsu.find(u)] = u;
                }

                for (i32 u = 0; u < n; ++u) {
                        root[u] = root[dsu.find(u)];
                }

                for (i32 u = 0; u < n; ++u) {
                        if (root[u] == u) {
                                es.emplace_back(u, n);
                                es.emplace_back(n, u);
                        } else {
                                es.emplace_back(to[u], u);
                                es.emplace_back(u, to[u]);
                        }
                }

                csr_graph<simple_edge> g(n + 1, es);
                h.build(g, n);
        }
};

#endif // LIB_FUNCTIONAL_GRAPH_HPP
