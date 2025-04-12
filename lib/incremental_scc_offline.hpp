#ifndef LIB_INCREMENTAL_SCC_OFFLINE_HPP
#define LIB_INCREMENTAL_SCC_OFFLINE_HPP 1

#include <vector>

#include <lib/csr_graph.hpp>
#include <lib/dsu.hpp>
#include <lib/prelude.hpp>
#include <lib/scc.hpp>

inline std::vector<i32> incremental_scc_offline(i32 n, const std::vector<std::pair<i32, i32>> &es) {
    if (es.empty()) return {};

    const i32 m = static_cast<i32>(es.size());
    std::vector<i32> t(m, m);
    std::vector<i32> idx(n, -1);

    const auto dfs = [&](auto &&self, i32 l, i32 r, std::vector<std::tuple<i32, i32, i32>> e) -> void {
        const i32 mid = (l + r) / 2;
        std::vector<std::tuple<i32, i32, i32>> el, er;

        std::vector<std::pair<i32, simple_edge>> fs;

        i32 p{};
        for (const auto &[id, u, v] : e) {
            if (r - l > 1 && id >= mid) break;
            if (idx[u] == -1) idx[u] = p++;
            if (idx[v] == -1) idx[v] = p++;
            fs.emplace_back(idx[u], idx[v]);
        }

        csr_graph g(p, fs);
        const auto &[_, group] = scc(g);

        if (r - l == 1) {
            for (const auto &[id, u, v] : e) {
                if (idx[u] == -1) idx[u] = p++;
                if (idx[v] == -1) idx[v] = p++;

                const i32 x = idx[u];
                const i32 y = idx[v];

                if (x < g.n && y < g.n && group[x] == group[y]) t[id] = l;
            }

            for (const auto &[id, u, v] : e) idx[u] = idx[v] = -1;
            return;
        }

        for (const auto &[id, u, v] : e) {
            if (idx[u] == -1) idx[u] = p++;
            if (idx[v] == -1) idx[v] = p++;

            const i32 x = idx[u];
            const i32 y = idx[v];

            if (id < mid) {
                if (group[x] == group[y])
                    el.emplace_back(id, u, v);
                else
                    er.emplace_back(id, group[x], group[y]);
            } else if (x < g.n && y < g.n && group[x] == group[y]) {
                t[id] = id;
            } else {
                const i32 w = x < g.n ? group[x] : x;
                const i32 z = y < g.n ? group[y] : y;
                er.emplace_back(id, w, z);
            }
        }

        for (const auto &[id, u, v] : e) idx[u] = idx[v] = -1;

        e.clear();
        e.shrink_to_fit();

        if (!el.empty()) self(self, l, mid, std::move(el));
        if (!er.empty()) self(self, mid, r, std::move(er));
    };

    std::vector<std::tuple<i32, i32, i32>> fs;
    fs.reserve(m);

    for (i32 i = 0; i < m; ++i) fs.emplace_back(i, es[i].first, es[i].second);

    dfs(dfs, 0, m, std::move(fs));
    return t;
}

#endif // LIB_INCREMENTAL_SCC_OFFLINE_HPP
