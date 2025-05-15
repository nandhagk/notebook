#ifndef LIB_DAG_ANTICHAIN_HPP
#define LIB_DAG_ANTICHAIN_HPP 1

#include <vector>

#include <lib/csr_graph.hpp>
#include <lib/hopcroft_karp.hpp>
#include <lib/konig.hpp>
#include <lib/prelude.hpp>

template <typename DAG>
std::vector<i32> dag_antichain(const DAG &g) {
    const i32 n = static_cast<i32>(g.size());

    std::vector<std::pair<i32, simple_edge>> es;
    for (i32 u = 0; u < n; ++u)
        for (const i32 v : g[u]) es.emplace_back(u, v + n);

    csr_graph h(2 * n, es);

    std::vector<i32> color(2 * n, 1);
    color.assign(n, 0);

    const auto mate = hopcroft_karp(h, color);
    const auto cover = konig(h, color, mate);

    std::vector<bool> ok(n, true);
    for (const i32 u : cover) ok[u % n] = false;

    std::vector<i32> antichain;
    antichain.reserve(n);

    for (i32 u = 0; u < n; ++u)
        if (ok[u]) antichain.push_back(u);

    return antichain;
}

#endif // LIB_DAG_ANTICHAIN_HPP
