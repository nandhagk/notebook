#ifndef LIB_STATIC_INVERSIONS_HPP
#define LIB_STATIC_INVERSIONS_HPP 1

#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>
#include <vector>

#include <lib/prelude.hpp>

template <typename T>
struct static_inversions {
private:
    struct bucket_type {
        std::vector<i32> rank;
        std::vector<i32> sorted_ind;
        std::vector<i64> prefix_inv;
        std::vector<i64> block_inv;
        std::vector<i32> freq;
    };

    i32 n, bsize;
    std::vector<bucket_type> buckets;

public:
    static_inversions() {}
    explicit static_inversions(const std::vector<T> &v) { build(v); }

    void build(const std::vector<T> &v) {
        n = static_cast<i32>(v.size());

        std::vector<i32> vi(n);
        std::iota(vi.begin(), vi.end(), 0);
        std::sort(vi.begin(), vi.end(), [&](i32 i, i32 j) { return v[i] == v[j] ? i < j : v[i] < v[j]; });

        std::vector<i32> rank(n);
        for (i32 i = 0; i < n; ++i) rank[vi[i]] = i;

        bsize = static_cast<i32>(std::sqrt(n)) + 1;
        buckets = std::vector<bucket_type>(n / bsize + 1);

        std::vector<i32> freq(n, 0);
        for (i32 r = 0; r != static_cast<i32>(buckets.size()); ++r) {
            bucket_type &b = buckets[r];

            b.rank = std::vector<i32>(rank.begin() + r * bsize, rank.begin() + std::min(n, (r + 1) * bsize));
            const i32 m = static_cast<i32>(b.rank.size());

            b.sorted_ind.resize(m);
            std::iota(b.sorted_ind.begin(), b.sorted_ind.end(), static_cast<i32>(0));
            std::sort(b.sorted_ind.begin(), b.sorted_ind.end(),
                      [&b](const i32 i, const i32 j) { return b.rank[i] < b.rank[j]; });

            b.freq.resize(n + 1);
            b.freq[0] = 0;
            for (i32 i = 0; i != n; ++i) b.freq[i + 1] = b.freq[i] + freq[i];
            for (const i32 e : b.rank) ++freq[e];

            b.prefix_inv.resize(m);
            i64 inv = 0;
            for (i32 i = 0; i != m; ++i) {
                b.prefix_inv[i] = inv;
                for (i32 j = 0; j != i; ++j)
                    if (b.rank[j] > b.rank[i]) ++inv;
            }

            if (r + 1 == static_cast<i32>(buckets.size())) {
                b.prefix_inv.push_back(inv);
            } else {
                buckets[r + 1].block_inv = std::vector<i64>(r + 1);
                buckets[r + 1].block_inv[r] = inv;
                for (i32 l = r; l != 0;) {
                    --l;
                    const bucket_type &a = buckets[l];
                    i32 ri = 0;
                    for (const i32 i : a.sorted_ind) {
                        while (ri != static_cast<i32>(b.sorted_ind.size()) && b.rank[b.sorted_ind[ri]] < a.rank[i])
                            ++ri;
                        inv += ri;
                    }
                    buckets[r + 1].block_inv[l] = b.block_inv[l] + inv;
                }
            }
        }
    }

    i64 prod(i32 l, i32 r) const {
        assert(0 <= l && l <= r && r <= n);

        const i32 fq = l / bsize;
        const i32 fr = l % bsize;
        const i32 lq = r / bsize;
        const i32 lr = r % bsize;

        const bucket_type &fb = buckets[fq];
        const bucket_type &lb = buckets[lq];

        if (fq == lq) {
            i64 ret = fb.prefix_inv[lr] - fb.prefix_inv[fr];
            i32 sum = 0;
            for (const i32 i : fb.sorted_ind)
                if (i < fr) ret -= sum;
                else if (i < lr)
                    ++sum;
            return ret;
        } else {
            i64 ret = lb.block_inv[fq];
            for (i32 i = 0; i != fr; ++i) ret -= lb.freq[fb.rank[i]] - fb.freq[fb.rank[i]];

            ret += static_cast<i64>(fr) * (fr - 1) / 2;
            ret -= fb.prefix_inv[fr];
            ret += lb.prefix_inv[lr];
            ret += static_cast<i64>(lq - fq) * bsize * lr;

            for (i32 i = 0; i != lr; ++i) ret -= lb.freq[lb.rank[i]] - fb.freq[lb.rank[i]];

            i32 sum = 0;
            auto itr = lb.sorted_ind.begin();
            const auto end = lb.sorted_ind.end();
            for (const i32 i : fb.sorted_ind) {
                while (itr != end && lb.rank[*itr] < fb.rank[i]) {
                    if (*itr < lr) ++sum;
                    ++itr;
                }
                if (i < fr) ret -= sum;
            }

            return ret;
        }
    }
};

#endif // LIB_STATIC_INVERSIONS_HPP
