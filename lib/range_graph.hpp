#ifndef LIB_RANGE_GRAPH_HPP
#define LIB_RANGE_GRAPH_HPP 1

#include <vector>

#include <lib/csr_graph.hpp>

template <typename W>
struct range_graph {
    i32 n, m;

    using edge = weighted_edge<W>;
    std::vector<std::pair<i32, edge>> es;

    range_graph() {}

    explicit range_graph(i32 p) {
        build(p);
    }

    void build(i32 p) {
        n = p;
        m = 3 * n;
        
        for (i32 i = 2; i < n + n; ++i)
            es.emplace_back(uid(i / 2), edge{uid(i), 0});

        for (i32 i = 2; i < n + n; ++i)
            es.emplace_back(lid(i), edge{lid(i / 2), 0});
    }

    inline i32 uid(i32 i) const {
        return i >= n ? i - n : n + i;
    }

    inline i32 lid(i32 i) const {
        return i >= n ? i - n : n + n + i;
    }

    void add(i32 u, i32 v, W w) {
        es.emplace_back(u, edge{v, w});
    }

    void add_from(i32 l, i32 r, i32 v, W w) {
        for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
            if (l & 1) add(lid(l++), v, w);  
            if (r & 1) add(lid(--r), v, w);  
        }
    }

    void add_to(i32 u, i32 l, i32 r, W w) {
        for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
            if (l & 1) add(u, uid(l++), w);  
            if (r & 1) add(u, uid(--r), w);  
        }
    }

    void add_range(i32 ul, i32 ur, i32 vl, i32 vr, W w) {
        const i32 z = m++;
        add_from(ul, ur, z, w);
        add_to(z, vl, vr, W(0));
    }
};

#endif // LIB_RANGE_GRAPH_HPP
