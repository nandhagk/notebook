#ifndef LIB_ROLLBACK_DSU_HPP
#define LIB_ROLLBACK_DSU_HPP 1

#include <algorithm>
#include <cassert>
#include <vector>

#include <lib/prelude.hpp>

struct rollback_dsu {
        i32 n, ccs;
        std::vector<i32> d;
        std::vector<std::pair<i32, i32>> h;

        rollback_dsu() {}
        explicit rollback_dsu(i32 m) {
                build(m);
        }

        void build(i32 m) {
                n = ccs = m;
                d.assign(n, -1);
                h.clear();
        }

        i32 find(i32 u) const {
                return d[u] < 0 ? u : find(d[u]);
        }

        bool merge(i32 u, i32 v) {
                assert(0 <= u && u < n);
                assert(0 <= v && v < n);

                u = find(u);
                v = find(v);

                if (u == v) return false;
                if (-d[u] < -d[v]) std::swap(u, v);

                h.emplace_back(v, d[v]);
                d[u] += d[v];
                d[v] = u;
                --ccs;
                
                return true;
        }

        bool same(i32 u, i32 v) const {
                assert(0 <= u && u < n);
                assert(0 <= v && v < n);

                return find(u) == find(v);
        }

        i32 size(i32 u) const {
                assert(0 <= u && u < n);
                
                return -d[find(u)];
        }

        void rollback() {
                assert(!h.empty());

                const auto [v, val] = h.back();
                const i32 u = d[v];

                d[v] = val;
                d[u] -= val;

                ++ccs;
                h.pop_back();
        }

        void rollback(i32 k) {
                assert(0 <= k);

                while (k--) rollback();
        }
};

#endif // LIB_ROLLBACK_DSU_HPP
