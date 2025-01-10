#ifndef LIB_CYCLE_HPP
#define LIB_CYCLE_HPP 1

#include <algorithm>
#include <optional>
#include <vector>

#include <lib/prelude.hpp>

template <typename Graph>
inline std::optional<std::vector<i32>> cycle_directed(const Graph &g) {
    const i32 n = static_cast<i32>(g.size());

    std::vector<i32> state(n);
    std::vector<std::pair<i32, i32>> par(n);

    std::vector<i32> ans;
    const auto dfs = [&](auto &&self, i32 u) -> bool {
        state[u] = 1;
        for (const auto &[v, i] : g[u]) {
            if (!state[v]) {
                par[v] = {u, i};
                if (self(self, v)) return true;
            } else if (state[v] == 1) {
                ans.push_back(i);
                for (i32 x = u; x != v; x = par[x].first) ans.push_back(par[x].second);

                std::reverse(ans.begin(), ans.end());
                return true;
            }
        }

        state[u] = 2;
        return false;
    };

    for (i32 u = 0; u < n; ++u) {
        if (state[u]) continue;
        if (dfs(dfs, u)) return ans;
    }

    return std::nullopt;
}

template <typename Graph>
inline std::optional<std::pair<std::vector<i32>, std::vector<i32>>> cycle_undirected(const Graph &g) {
    const i32 n = static_cast<i32>(g.size());

    std::vector<i32> buf(n, -1);
    std::vector<i32> es, vs;

    const auto dfs = [&](auto &&self, i32 u, i32 ei, i32 dep) -> i32 {
        buf[u] = dep;
        for (const auto &[v, i] : g[u]) {
            if (i == ei) continue;
            if (buf[v] != -1) {
                es.resize(dep - buf[v] + 1);
                vs.resize(dep - buf[v] + 1);

                es.back() = i;
                vs.back() = u;

                return buf[v];
            }

            if (const i32 r = self(self, v, i, dep + 1); r != -1) {
                if (dep < r) return r;
                es[dep - r] = i;
                vs[dep - r] = u;
                return r;
            }
        }

        return -1;
    };

    for (i32 u = 0; u < n; ++u) {
        if (buf[u] != -1) continue;
        if (dfs(dfs, u, -1, 0) != -1) return std::make_pair(std::move(es), std::move(vs));
    }

    return std::nullopt;
}

#endif // LIB_CYCLE_HPP
