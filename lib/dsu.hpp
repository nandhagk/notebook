#ifndef LIB_DSU_HPP
#define LIB_DSU_HPP 1

#include <algorithm>
#include <cassert>
#include <vector>

#include <lib/csr_array.hpp>
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

    i32 find(i32 u) {
        assert(0 <= u && u < n);

        if (d[u] < 0) return u;
        return d[u] = find(d[u]);
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

    bool same(i32 u, i32 v) {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);

        return find(u) == find(v);
    }

    i32 size(i32 u) {
        assert(0 <= u && u < n);

        return -d[find(u)];
    }

    std::pair<i32, std::vector<i32>> ids() {
        std::vector<std::pair<i32, i32>> par(n);

        for (i32 u = 0; u < n; ++u) par[u] = {find(u), u};
        csr_array cc(n, par);

        std::vector<i32> ids(n);

        i32 group{};
        for (i32 u = 0; u < n; ++u) {
            if (cc[u].empty()) continue;

            for (const i32 v : cc[u]) ids[v] = group;
            ++group;
        }

        assert(group == ccs);
        return {group, std::move(ids)};
    }
};

#endif // LIB_DSU_HPP
