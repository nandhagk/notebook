#ifndef LIB_DAG_PATH_COVER_HPP
#define LIB_DAG_PATH_COVER_HPP 1

#include <vector>

#include <lib/csr_graph.hpp>
#include <lib/hopcroft_karp.hpp>
#include <lib/prelude.hpp>
#include <lib/random.hpp>

template <typename DAG>
std::vector<std::vector<i32>> dag_path_cover(const DAG &g) {
    const i32 n = static_cast<i32>(g.size());

    std::vector<std::pair<i32, simple_edge>> fs;
    for (i32 u = 0; u < n; ++u) {
        for (const i32 v : g[u]) {
            fs.emplace_back(u, v + n);
            fs.emplace_back(v + n, u);
        }
    }

    std::shuffle(fs.begin(), fs.end(), MT);
    csr_graph h(2 * n, fs);

    std::vector<i32> color(2 * n, 1);
    color.assign(n, 0);

    const auto mate = hopcroft_karp(h, color);

    std::vector<i32> us;
    us.reserve(n);

    for (i32 i = n; i < 2 * n; ++i)
        if (mate[i] == -1) us.push_back(i - n);

    const i32 z = static_cast<i32>(us.size());

    std::vector<std::vector<i32>> ps(z);
    for (i32 i = 0; i < z; ++i)
        for (i32 u = us[i]; u >= 0; u = mate[u] - n) ps[i].push_back(u);

    return ps;
}

#endif // LIB_DAG_PATH_COVER_HPP
