#ifndef LIB_STATIC_DISTINCT_OFFLINE_HPP
#define LIB_STATIC_DISTINCT_OFFLINE_HPP 1

#include <algorithm>
#include <vector>

#include <lib/compress.hpp>
#include <lib/fenwick_tree.hpp>
#include <lib/monoids/add.hpp>
#include <lib/prelude.hpp>

template <typename T>
struct static_distinct_offline {
    i32 n;
    std::vector<i32> nxt, first;
    std::vector<std::tuple<i32, i32, i32>> qs;

    static_distinct_offline() {}
    explicit static_distinct_offline(const std::vector<T> &v) {
        build(v);
    }

    void build(const std::vector<T> &v) {
        n = static_cast<i32>(v.size());
        qs.clear();

        const auto b = compress(v);

        nxt.assign(n, -1);
        first.assign(n, 0);

        std::vector<i32> last(n, -1);
        for (i32 i = 0; i < n; ++i) {
            if (last[b[i]] != -1)
                nxt[last[b[i]]] = i;
            else
                ++first[i];

            last[b[i]] = i;
        }
    }

    void query(i32 l, i32 r) {
        assert(0 <= l && l <= r && r <= n);

        const i32 q = static_cast<i32>(qs.size());
        qs.emplace_back(l, r, q);
    }

    std::vector<i32> solve() {
        std::vector<i32> out(qs.size());
        std::sort(qs.begin(), qs.end());

        fenwick_tree<monoid_add<i32>> ft(first);

        i32 i = 0;
        for (const auto &[l, r, j] : qs) {
            for (; i < l; ++i)
                if (nxt[i] != -1) ft.multiply(nxt[i], 1);

            out[j] = ft.prod(l, r);
        }

        return out;
    }
};

#endif // LIB_STATIC_DISTINCT_OFFLINE_HPP
