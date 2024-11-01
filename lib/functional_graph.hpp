#ifndef LIB_FUNCTIONAL_GRAPH_HPP
#define LIB_FUNCTIONAL_GRAPH_HPP 1

#include <vector>
#include <cassert>

#include <lib/prelude.hpp>
#include <lib/dsu.hpp>
#include <lib/hld.hpp>

struct functional_graph {
        i32 n;
        std::vector<i32> to, root;
        hld h;

        functional_graph() {}
        explicit functional_graph(const std::vector<i32> &to_) {
                build(to_);
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

        void build(const std::vector<i32> &to_) {
                n = static_cast<i32>(to_.size());

                to = to_;
                root.assign(n, 0);

                std::vector<std::vector<i32>> t(n + 1);

                dsu dsu(n);
                for (i32 u = 0; u < n; ++u) {
                        if (!dsu.merge(u, to[u])) root[u] = u;
                }

                for (i32 u = 0; u < n; ++u) {
                        if (root[u] == u) root[dsu[u]] = u;
                }

                for (i32 u = 0; u < n; ++u) {
                        root[u] = root[dsu[u]];
                }

                for (i32 u = 0; u < n; ++u) {
                        if (root[u] == u) {
                                t[u].push_back(n);
                                t[n].push_back(u);
                        } else {
                                t[to[u]].push_back(u);
                                t[u].push_back(to[u]);
                        }
                }

                h.build(t, n);
        }
};

#endif // LIB_FUNCTIONAL_GRAPH_HPP
