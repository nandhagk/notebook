#ifndef LIB_HLD_HPP
#define LIB_HLD_HPP 1

#include <algorithm>
#include <cassert>
#include <optional>
#include <vector>

#include <lib/prelude.hpp>

struct hld {
    i32 n, time;
    std::vector<i32> sz, tin, depth, par, tour, best, start;

    hld() {}

    template <typename Tree>
    explicit hld(const Tree &g)
        : hld(g, 0) {}

    template <typename Tree>
    hld(const Tree &g, i32 root) {
        build(g, root);
    }

    template <typename Tree>
    void build(const Tree &g) {
        build(g, 0);
    }

    template <typename Tree>
    void build(const Tree &g, i32 root) {
        n = static_cast<i32>(g.size());
        time = 0;

        sz.assign(n, 1);
        tin.assign(n, 0);
        depth.assign(n, 0);
        par.assign(n, 0);
        tour.assign(n, 0);
        best.assign(n, -1);
        start.assign(n, 0);

        par[root] = -1;
        dfs_sz(g, root);

        start[root] = root;
        dfs_hld(g, root);
    }

    bool is_ancestor(i32 u, i32 v) const {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);

        return tin[u] <= tin[v] && tin[u] + sz[u] > tin[v];
    }

    std::optional<i32> jump(i32 u, i32 k) const {
        assert(0 <= u && u < n);
        assert(0 <= k);

        if (depth[u] < k) return std::nullopt;

        while (u != -1) {
            const i32 s = start[u];
            if (depth[s] <= depth[u] - k) return tour[tin[u] - k];

            k -= depth[u] - depth[s] + 1;
            u = par[s];
        }

        return u;
    }

    i32 lca(i32 u, i32 v) const {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);

        if (is_ancestor(u, v)) return u;
        if (is_ancestor(v, u)) return v;

        for (; start[u] != start[v]; v = par[start[v]])
            if (depth[start[u]] > depth[start[v]]) std::swap(u, v);

        return depth[u] < depth[v] ? u : v;
    }

    i32 dist(i32 u, i32 v) const {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);

        return depth[u] + depth[v] - 2 * depth[lca(u, v)];
    }

    bool is_on_path(i32 u, i32 v, i32 s) const {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);
        assert(0 <= s && s < n);

        return lca(u, v) == s || (is_ancestor(s, u) ^ is_ancestor(s, v));
    }

    std::optional<i32> jump(i32 u, i32 v, i32 k) const {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);
        assert(0 <= k);

        const i32 du = depth[u];
        const i32 dv = depth[v];
        const i32 dx = depth[lca(u, v)];

        const i32 l = du - dx;
        const i32 r = dv - dx;

        if (l + r < k)
            return std::nullopt;
        else if (k < l)
            return jump(u, k);
        else
            return jump(v, l + r - k);
    }

    std::vector<std::pair<i32, i32>> decompose(i32 u, i32 v) const {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);

        std::vector<std::pair<i32, i32>> up, dn;

        while (start[u] != start[v]) {
            if (tin[u] < tin[v]) {
                dn.emplace_back(start[v], v);
                v = par[start[v]];
            } else {
                up.emplace_back(u, start[u]);
                u = par[start[u]];
            }
        }

        if (tin[u] < tin[v])
            dn.emplace_back(u, v);
        else
            up.emplace_back(u, v);

        up.insert(up.end(), dn.rbegin(), dn.rend());
        return up;
    }

private:
    template <typename Tree>
    void dfs_sz(const Tree &g, i32 u) {
        i32 &x = best[u];
        const i32 t = par[u];

        for (const i32 v : g[u]) {
            if (v == t) continue;

            par[v] = u;
            depth[v] = depth[u] + 1;

            dfs_sz(g, v);

            sz[u] += sz[v];
            if (x == -1 || sz[v] > sz[x]) x = v;
        }
    }

    template <typename Tree>
    void dfs_hld(const Tree &g, i32 u) {
        tour[time] = u;
        tin[u] = time++;

        const i32 x = best[u];
        const i32 t = par[u];

        if (x != -1) {
            start[x] = start[u];
            dfs_hld(g, x);
        }

        for (const i32 v : g[u]) {
            if (v == t || v == x) continue;

            start[v] = v;
            dfs_hld(g, v);
        }
    }
};

#endif // LIB_HLD_HPP
