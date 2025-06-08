#ifndef LIB_GRAPH_TRAITS_HPP
#define LIB_GRAPH_TRAITS_HPP 1

#include <utility>

#include <lib/prelude.hpp>

template <typename Graph>
constexpr auto graph_weight(const Graph &g) {
    const auto &[_, w] = *g[0].begin();
    return w;
}

template <typename Graph>
using graph_weight_t = decltype(graph_weight(std::declval<Graph>()));

#endif // LIB_GRAPH_TRAITS_HPP
