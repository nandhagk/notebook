#ifndef LIB_HAVEL_HAKIMI_HPP
#define LIB_HAVEL_HAKIMI_HPP 1

#include <cassert>
#include <numeric>
#include <vector>

#include <lib/prelude.hpp>

// Will panic if not possible to construct
inline std::vector<std::pair<i32, i32>> havel_hakimi(std::vector<i32> deg) {
    const i32 n = static_cast<i32>(deg.size());

    std::vector<std::vector<i32>> d(n);
    for (i32 u = 0; u < n; ++u) d[deg[u]].push_back(u);

    std::vector<std::pair<i32, i32>> es;
    es.reserve(std::reduce(deg.begin(), deg.end()));

    i32 mx = n - 1;
    for (i32 i = 0; i < n; ++i) {
        while (mx >= 0 && d[mx].empty()) --mx;

        const i32 u = d[mx].back();
        d[mx].pop_back();

        std::vector<i32> nbd;

        i32 k = mx;
        while (static_cast<i32>(nbd.size()) < deg[u]) {
            assert(k != 0);
            if (d[k].empty()) {
                --k;
                continue;
            }

            const i32 v = d[k].back();
            d[k].pop_back();

            nbd.push_back(v);
        }

        for (const i32 v : nbd) {
            es.emplace_back(u, v);

            --deg[v];
            d[deg[v]].push_back(v);
        }

        deg[u] = 0;
    }

    return es;
}

#endif // LIB_HAVEL_HAKIMI_HPP
