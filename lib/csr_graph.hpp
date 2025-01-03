#ifndef LIB_CSR_GRAPH_HPP
#define LIB_CSR_GRAPH_HPP 1

#include <numeric>
#include <vector>

#include <lib/prelude.hpp>

struct simple_edge {
    i32 v;

    simple_edge() {}
    simple_edge(i32 to) : v{to} {}

    i32 to() const { return v; }

    operator int() const { return v; }
};

template <typename W>
struct weighted_edge {
    using CostT = W;

    i32 v;
    W w;

    weighted_edge() {}
    weighted_edge(i32 to, W cost) : v{to}, w{cost} {}

    i32 to() const { return v; }

    operator int() const { return v; }
};

template <typename Edge>
struct csr_graph {
    using E = Edge;

    csr_graph() {}
    explicit csr_graph(i32 p, const std::vector<std::pair<i32, Edge>> &es) { build(p, es); }

    void build(i32 p, const std::vector<std::pair<i32, Edge>> &es) {
        n = p;
        m = static_cast<i32>(es.size());

        deg.assign(n, 0);
        indeg.assign(n, 0);
        outdeg.assign(n, 0);

        elist.resize(m);
        start.resize(n);

        for (const auto &[u, e] : es) {
            ++deg[u];
            ++deg[e.to()];

            ++indeg[e.to()];
            ++outdeg[u];
        }

        std::inclusive_scan(outdeg.begin(), outdeg.end(), start.begin());
        for (const auto &[u, e] : es) elist[--start[u]] = e;
    }

    i32 size() const { return n; }

    struct edge_range {
        const E *es;
        usize size;

        edge_range(const E *e, usize sz) : es{e}, size{sz} {}

        const E *begin() { return &es[0]; }

        const E *end() { return &es[size]; }

        const E *begin() const { return &es[0]; }

        const E *end() const { return &es[size]; }
    };

    edge_range operator[](i32 u) const { return edge_range(elist.data() + start[u], outdeg[u]); }

    csr_graph<E> reverse() const {
        std::vector<std::pair<i32, E>> es;
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

    i32 n, m;
    std::vector<i32> deg, indeg, outdeg, start;
    std::vector<E> elist;
};

#endif // LIB_CSR_GRAPH_HPP
