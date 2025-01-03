#ifndef LIB_STATIC_DISTINCT_OFFLINE_HPP
#define LIB_STATIC_DISTINCT_OFFLINE_HPP 1

#include <algorithm>
#include <numeric>
#include <vector>

#include <lib/fenwick_tree.hpp>
#include <lib/monoids/add.hpp>
#include <lib/prelude.hpp>

template <typename T>
struct static_distinct_offline {
    i32 n, q;
    std::vector<i32> nxt, first;
    std::vector<std::tuple<i32, i32, i32>> qs;

    static_distinct_offline() {}
    explicit static_distinct_offline(const std::vector<T> &v) { build(v); }

    void build(const std::vector<T> &v) {
        n = static_cast<i32>(v.size());
        q = 0;

        std::vector<i32> vi(n);
        std::iota(vi.begin(), vi.end(), 0);
        std::sort(vi.begin(), vi.end(), [&](const i32 i, const i32 j) { return v[i] == v[j] ? i < j : v[i] < v[j]; });

        std::vector<T> rv;
        rv.reserve(n);

        std::vector<i32> b(n);
        for (const i32 i : vi) {
            if (rv.empty() || rv.back() != v[i]) rv.push_back(v[i]);
            b[i] = static_cast<i32>(rv.size()) - 1;
        }

        nxt.assign(n, -1);
        first.reserve(n);

        std::vector<i32> last(n, -1);
        for (i32 i = 0; i < n; ++i) {
            if (last[b[i]] != -1) nxt[last[b[i]]] = i;
            else
                first.push_back(i);

            last[b[i]] = i;
        }
    }

    void query(i32 l, i32 r) { qs.emplace_back(l, r, q++); }

    std::vector<i32> solve() {
        std::vector<i32> out(q);
        std::sort(qs.begin(), qs.end());

        fenwick_tree<monoid_add<i32>> ft(n);
        for (const i32 i : first) ft.multiply(i, 1);

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
