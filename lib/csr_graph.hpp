#ifndef LIB_CSR_GRAPH_HPP
#define LIB_CSR_GRAPH_HPP 1

#include <numeric>
#include <vector>

#include <lib/csr_array.hpp>
#include <lib/prelude.hpp>

struct simple_edge {
    i32 v;

    simple_edge() = default;
    simple_edge(i32 to)
        : v{to} {}

    i32 to() const {
        return v;
    }

    operator int() const {
        return v;
    }
};

template <typename W>
struct weighted_edge {
    using CostT = W;

    i32 v;
    W w;

    weighted_edge() {}
    weighted_edge(i32 to, W cost)
        : v{to}, w{cost} {}

    i32 to() const {
        return v;
    }

    const W &weight() const {
        return w;
    }

    operator int() const {
        return v;
    }
};

template <typename Edge>
struct csr_graph : csr_array<Edge> {
    using super = csr_array<Edge>;

    using super::m;
    using super::n;
    using super::size;
    using super::operator[];

    csr_graph() {}
    csr_graph(i32 p, const std::vector<std::pair<i32, Edge>> &es) {
        build(p, es);
    }

    void build(i32 p, const std::vector<std::pair<i32, Edge>> &es) {
        super::build(p, es);
    }

    csr_graph reverse() const {
        std::vector<std::pair<i32, Edge>> es;
        es.reserve(m);

        for (i32 u = 0; u < n; ++u) {
            for (auto e : operator[](u)) {
                const i32 v = e.to();

                e.v = u;
                es.emplace_back(v, e);
            }
        }

        return csr_graph(n, es);
    }
};

#endif // LIB_CSR_GRAPH_HPP
