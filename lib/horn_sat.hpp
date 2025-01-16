#ifndef LIB_HORN_SAT_HPP
#define LIB_HORN_SAT_HPP 1

#include <optional>
#include <queue>
#include <vector>

#include <lib/csr_graph.hpp>
#include <lib/prelude.hpp>

struct horn_sat {
    i32 n, m;
    std::vector<std::optional<i32>> ps;
    std::vector<std::pair<i32, simple_edge>> es;

    horn_sat() {}

    explicit horn_sat(i32 p) {
        build(p);
    }

    void build(i32 p) {
        n = p;
        m = 0;

        ps.clear();
        es.clear();
    }

    void add_clause(const std::vector<i32> neg, std::optional<i32> pos = std::nullopt) {
        for (const i32 u : neg) {
            es.emplace_back(u, n + m);
            es.emplace_back(n + m, u);
        }

        ps.push_back(pos);
        ++m;
    }

    std::optional<std::vector<bool>> satisfiable() {
        std::vector<bool> ans(n, false), vis(n, false);

        csr_graph g(n + m, es);
        std::queue<i32> confirmed;

        auto neg = g.outdeg;
        for (i32 i = 0; i < m; ++i)
            if (neg[i + n] == 0 && ps[i].has_value()) confirmed.push(i);

        while (!confirmed.empty()) {
            const i32 u = confirmed.front();
            confirmed.pop();

            if (const auto r = ps[u]; r.has_value() && !vis[*r]) {
                const i32 v = *r;

                ans[v] = true;
                vis[v] = true;

                for (const i32 w : g[v])
                    if (--neg[w] == 0) confirmed.push(w - n);
            }
        }

        for (i32 i = 0; i < m; ++i) {
            i32 t{};

            for (const i32 v : g[n + i])
                if (!ans[v]) ++t;

            if (ps[i].has_value() && ans[*ps[i]]) ++t;
            if (t == 0) return std::nullopt;
        }

        return ans;
    }
};

#endif // LIB_HORN_SAT_HPP
