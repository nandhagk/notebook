#include <bits/stdc++.h>
#include <contest/debug.hpp>

#include <lib/prelude.hpp>
#include <lib/scc.hpp>

void solve() {
        i32 n, m;
        std::cin >> n >> m;
 
        std::vector<std::vector<i32>> g(n);
        while (m--) {
                i32 u, v;
                std::cin >> u >> v;
 
                --u;
                --v;
 
                g[u].push_back(v);
        }
 
        const auto ids = scc(g);
        const auto es = make_strongly_connected(g, ids);
 
        std::cout << static_cast<i32>(es.size()) << '\n';
        for (const auto &[u, v] : es) {
                std::cout << u + 1 << ' ' << v + 1 << '\n';
        }
}

i32 main() {
        std::ios_base::sync_with_stdio(false);
        std::cin.tie(nullptr);

        solve();
}
