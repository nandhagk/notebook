#ifndef LIB_PRUFER_CODE_HPP
#define LIB_PRUFER_CODE_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>

template <typename Tree>
inline std::vector<i32> to_prufer_code(const Tree &g) {
    const i32 n = static_cast<i32>(g.size());

    std::vector<i32> par(n, -1);
    const auto dfs = [&](auto &&self, i32 u, i32 t = -1) -> void {
        for (const i32 v : g[u]) {
            if (v == t) continue;

            par[v] = u;
            self(self, v, u);
        }
    };

    dfs(dfs, n - 1);

    i32 ptr = -1;
    std::vector<i32> deg(n);
    for (i32 u = 0; u < n; ++u) {
        deg[u] = static_cast<i32>(g[u].size());
        if (deg[u] == 1 && ptr == -1) ptr = u;
    }

    std::vector<i32> code;
    code.reserve(n);

    i32 leaf = ptr;
    for (i32 i = 0; i < n - 2; ++i) {
        const i32 nxt = par[leaf];
        code.push_back(nxt);

        if (--deg[nxt] == 1 && nxt < ptr) {
            leaf = nxt;
            continue;
        }

        ++ptr;
        while (ptr < n && deg[ptr] != 1) ++ptr;

        leaf = ptr;
    }

    return code;
}

inline std::vector<i32> from_prufer_code(const std::vector<i32> &code) {
    const i32 n = static_cast<i32>(code.size()) + 2;

    std::vector<i32> par(n, -1);
    if (n == 1) return par;

    std::vector<i32> deg(n, 1);
    for (const i32 u : code) ++deg[u];

    i32 ptr = 0;
    while (ptr < n && deg[ptr] != 1) ++ptr;

    i32 leaf = ptr;
    for (const i32 u : code) {
        par[leaf] = u;
        --deg[leaf];

        if (--deg[u] == 1 && u < ptr) {
            leaf = u;
            continue;
        }

        ++ptr;
        while (ptr < n && deg[ptr] != 1) ++ptr;

        leaf = ptr;
    }

    for (i32 u = 0; u < n - 1; ++u)
        if (deg[u] == 1) par[u] = n - 1;

    return par;
}

#endif // LIB_PRUFER_CODE_HPP
