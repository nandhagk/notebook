#ifndef LIB_BCC_HPP
#define LIB_BCC_HPP 1

#include <vector>

#include <lib/csr_graph.hpp>
#include <lib/prelude.hpp>

template <typename Graph>
inline std::pair<std::vector<bool>, std::vector<std::vector<i32>>> bcc(const Graph &g) {
    const i32 n = static_cast<i32>(g.size());

    std::vector<i32> seen, tin(n, -1), low(n);
    seen.reserve(n);

    std::vector<std::vector<i32>> ccs;
    std::vector<bool> c(n, false);

    i32 time{};
    const auto dfs = [&](auto &&self, i32 u, i32 t = -1) -> void {
        seen.push_back(u);
        tin[u] = low[u] = time++;

        i32 child{};
        for (const i32 v : g[u]) {
            if (v == t) continue;

            if (tin[v] != -1) {
                low[u] = std::min(low[u], tin[v]);
                continue;
            }

            ++child;
            const i32 s = static_cast<i32>(seen.size());

            self(self, v, u);
            low[u] = std::min(low[u], low[v]);

            if ((t == -1 && child > 1) || (t != -1 && low[v] >= tin[u])) {
                c[u] = true;
                auto &cc = ccs.back();

                cc.push_back(u);
                while (static_cast<i32>(seen.size()) > s) {
                    cc.push_back(seen.back());
                    seen.pop_back();
                }

                ccs.emplace_back();
            }
        }
    };

    for (i32 u = 0; u < n; ++u) {
        if (tin[u] != -1) continue;

        ccs.emplace_back();
        dfs(dfs, u);

        auto &cc = ccs.back();
        for (const i32 v : seen) cc.push_back(v);

        seen.clear();
    }

    return {std::move(c), std::move(ccs)};
}

template <typename Graph>
inline std::tuple<i32, std::vector<i32>, csr_graph<simple_edge>>
block_cut_tree(const Graph &g, const std::vector<bool> &c, const std::vector<std::vector<i32>> &ccs) {
    const i32 n = static_cast<i32>(g.size());
    std::vector<i32> ids(n, -1);

    i32 group{};
    for (i32 u = 0; u < n; ++u)
        if (c[u]) ids[u] = group++;

    std::vector<std::pair<i32, simple_edge>> es;
    es.reserve(2 * (group + ccs.size()));

    for (const auto &cc : ccs) {
        for (const i32 u : cc) {
            if (!c[u]) {
                ids[u] = group;
                continue;
            }

            es.emplace_back(ids[u], group);
            es.emplace_back(group, ids[u]);
        }

        ++group;
    }

    return {group, std::move(ids), csr_graph(group, es)};
}

#endif // LIB_BCC_HPP
