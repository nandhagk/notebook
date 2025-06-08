#include <bits/stdc++.h>
#include <contest/debug.hpp>

#include <lib/prelude.hpp>
#include <lib/csr_graph.hpp>
#include <lib/dijkstra.hpp>

void solve() {
    i32 n, m, s, t;
    std::cin >> n >> m >> s >> t;

    using edge = weighted_edge<i64>;

    std::vector<std::pair<i32, edge>> es;
    es.reserve(m);

    while (m--) {
        i32 u, v;
        i64 w;
        std::cin >> u >> v >> w;

        es.emplace_back(u, edge{v, w});
    }

    csr_graph g(n, es);
    const auto [dst, prv] = dijkstra(g, s, t);

    if (dst[t] == inf<i64>) {
        std::cout << -1 << '\n';
        return;
    }

    std::vector<i32> pth;
    pth.reserve(n);

    for (i32 cur = t; cur != -1; cur = prv[cur]) pth.push_back(cur);
    std::reverse(pth.begin(), pth.end());

    const i32 k = static_cast<i32>(pth.size());

    std::cout << dst[t] << ' ' << k - 1 << '\n';
    for (i32 i = 1; i < k; ++i)
        std::cout << pth[i - 1] <<  ' ' << pth[i] << '\n';
}

i32 main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    solve();
}
