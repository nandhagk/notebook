#ifndef LIB_DIAMETER_HPP
#define LIB_DIAMETER_HPP 1

#include <algorithm>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename Graph>
inline std::tuple<i32, i32, std::vector<graph_weight_t<Graph>>, std::vector<i32>> diameter(const Graph &g) {
    using W = graph_weight_t<Graph>;
    const i32 n = static_cast<i32>(g.size());

    std::vector<W> dst(n, inf<W>);
    std::vector<i32> prv(n, -1);

    const auto dfs = [&](auto &&self, i32 u, i32 t = -1) -> void {
        for (const auto &[v, w] : g[u]) {
            if (v == t) continue;

            dst[v] = dst[u] + w;
            prv[v] = u;
            self(self, v, u);
        }
    };

    dst[0] = W(0);
    dfs(dfs, 0);

    const i32 s = static_cast<i32>(std::max_element(dst.begin(), dst.end()) - dst.begin());

    dst.assign(n, inf<W>);
    prv.assign(n, -1);

    dst[s] = W(0);
    dfs(dfs, s);

    const i32 t = static_cast<i32>(std::max_element(dst.begin(), dst.end()) - dst.begin());
    return {s, t, std::move(dst), std::move(prv)};
}

#endif // LIB_DIAMETER_HPP
