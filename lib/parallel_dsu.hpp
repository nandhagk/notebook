#ifndef LIB_PARALLEL_DSU_HPP
#define LIB_PARALLEL_DSU_HPP 1

#include <vector>

#include <lib/bits.hpp>
#include <lib/dsu.hpp>
#include <lib/prelude.hpp>

struct parallel_dsu {
    i32 n, log;
    std::vector<dsu> ds;

    parallel_dsu() {}
    explicit parallel_dsu(i32 m) {
        build(m);
    }

    void build(i32 m) {
        n = m;

        log = 1;
        while ((1 << log) < n) ++log;

        ds.resize(log);
        for (i32 i = 0; i < log; ++i) ds[i].build(n - (1 << i) + 1);
    }

    template <typename F>
    void merge(i32 L1, i32 L2, i32 x, F f) {
        if (x == 0) return;
        if (x == 1) return merge_inner(0, L1, L2, f);

        const i32 k = topbit(x - 1);
        merge_inner(k, L1, L2, f);
        merge_inner(k, L1 + x - (1 << k), L2 + x - (1 << k), f);
    }

    template <typename F>
    void merge(i32 i, i32 j, F f) {
        merge_inner(0, i, j, f);
    }

private:
    template <typename F>
    void merge_inner(i32 k, i32 L1, i32 L2, F f) {
        if (k == 0) {
            const i32 a = ds[0].find(L1);
            const i32 b = ds[0].find(L2);

            if (a == b) return;

            ds[0].merge(a, b);
            const i32 c = ds[0].find(a);

            return f(c, a ^ b ^ c);
        }

        if (!ds[k].merge(L1, L2)) return;

        merge_inner(k - 1, L1, L2, f);
        merge_inner(k - 1, L1 + (1 << (k - 1)), L2 + (1 << (k - 1)), f);
    }
};

#endif // LIB_PARALLEL_DSU_HPP
