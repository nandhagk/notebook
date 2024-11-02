#ifndef LIB_DSU_HPP
#define LIB_DSU_HPP 1

#include <algorithm>
#include <cassert>
#include <vector>

#include <lib/prelude.hpp>

struct dsu {
        i32 n, ccs;
        std::vector<i32> d;

        dsu() {}
        explicit dsu(i32 m) {
                build(m);
        }

        void build(i32 m) {
                n = ccs = m;
                d.assign(n, -1);
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
};

#endif // LIB_DSU_HPP
