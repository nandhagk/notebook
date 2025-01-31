#ifndef LIB_RANGE_GRAPH_HPP
#define LIB_RANGE_GRAPH_HPP 1

#include <lib/prelude.hpp>

struct range_graph {
    i32 n, m;

    range_graph() {}

    template <typename F>
    range_graph(i32 p, F f) {
        build(p, f);
    }

    template <typename F>
    void build(i32 p, F f) {
        n = p;
        m = 3 * n;

        for (i32 i = 2; i < n + n; ++i) f(uid(i / 2), uid(i));
        for (i32 i = 2; i < n + n; ++i) f(lid(i), lid(i / 2));
    }

    template <typename F>
    static void add(i32 u, i32 v, F f) {
        f(u, v);
    }

    template <typename F>
    void add_from(i32 l, i32 r, i32 v, F f) const {
        for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
            if (l & 1) add(lid(l++), v, f);
            if (r & 1) add(lid(--r), v, f);
        }
    }

    template <typename F>
    void add_to(i32 u, i32 l, i32 r, F f) const {
        for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
            if (l & 1) add(u, uid(l++), f);
            if (r & 1) add(u, uid(--r), f);
        }
    }

    template <typename F>
    void add_range(i32 ul, i32 ur, i32 vl, i32 vr, F f) {
        const i32 z = m++;

        add_from(ul, ur, z, f);
        add_to(z, vl, vr, f);
    }

private:
    inline i32 uid(i32 i) const {
        return i >= n ? i - n : n + i;
    }

    inline i32 lid(i32 i) const {
        return i >= n ? i - n : n + n + i;
    }
};

#endif // LIB_RANGE_GRAPH_HPP
