#ifndef LIB_LICHAO_TREE_HPP
#define LIB_LICHAO_TREE_HPP 1

#include <algorithm>
#include <vector>

#include <lib/prelude.hpp>

template <typename F, bool MINIMIZE>
struct lichao_tree {
    using T = typename F::ValueT;

    i32 n, log, size;
    std::vector<i64> z;
    std::vector<F> d;

    lichao_tree() {}

    lichao_tree(const std::vector<i64> &y, const F &f) {
        build(y, f);
    }

    void build(const std::vector<i64> &y, const F &f) {
        z.reserve(y.size());
        for (const i64 x : y) z.push_back(x);

        std::sort(z.begin(), z.end());
        z.erase(std::unique(z.begin(), z.end()), z.end());

        n = static_cast<i32>(z.size());

        log = 1;
        while ((1 << log) < n) ++log;

        size = 1 << log;
        d.assign(size << 1, f);
    }

    template <std::enable_if<MINIMIZE> * = nullptr>
    void chmin_line(const F &f) {
        return add_line_at(1, f);
    }

    template <std::enable_if<!MINIMIZE> * = nullptr>
    void chmax_line(const F &f) {
        return add_line_at(1, f);
    }

    template <std::enable_if<MINIMIZE> * = nullptr>
    void chmin_segment(i64 xl, i64 xr, const F &f) {
        for (i32 l = idx(xl) + size, r = idx(xr) + size; l < r; l >>= 1, r >>= 1) {
            if (l & 1) add_line_at(l++, f);
            if (r & 1) add_line_at(--r, f);
        }
    }

    template <std::enable_if<!MINIMIZE> * = nullptr>
    void chmax_segment(i64 xl, i64 xr, const F &f) {
        for (i32 l = idx(xl) + size, r = idx(xr) + size; l < r; l >>= 1, r >>= 1) {
            if (l & 1) add_line_at(l++, f);
            if (r & 1) add_line_at(--r, f);
        }
    }

    std::pair<T, F> query(i64 x) {
        F f = d[0];
        T fx = f(x);
        for (i32 i = idx(x) + size; i > 0; i >>= 1) {
            F g = d[i];
            T gx = g(x);

            if (MINIMIZE ? gx < fx : gx > fx) {
                f = g;
                fx = gx;
            }
        }

        return {fx, f};
    }

private:
    void add_line_at(i32 i, F f) {
        i32 upper_bit = topbit(i);

        i32 l = (size >> upper_bit) * (i - (1 << upper_bit));
        i32 r = l + (size >> upper_bit);

        while (l < r) {
            F g = d[i];

            T fl = evaluate_inner(f, l);
            T fr = evaluate_inner(f, r - 1);
            T gl = evaluate_inner(g, l);
            T gr = evaluate_inner(g, r - 1);

            bool bl = (MINIMIZE ? fl < gl : fl > gl);
            bool br = (MINIMIZE ? fr < gr : fr > gr);

            if (bl && br) {
                d[i] = f;
                return;
            }

            if (!bl && !br) return;

            i32 m = (l + r) / 2;
            T fm = evaluate_inner(f, m);
            T gm = evaluate_inner(g, m);

            bool bm = (MINIMIZE ? fm < gm : fm > gm);
            if (bm) {
                d[i] = f;
                f = g;

                if (bl) {
                    i = 2 * i + 1;
                    l = m;
                } else {
                    i = 2 * i + 0;
                    r = m;
                }
            } else if (bl) {
                i = 2 * i + 0;
                r = m;
            } else {
                i = 2 * i + 1;
                l = m;
            }
        }
    }

    inline T evaluate_inner(F &f, i32 x) {
        return f(z[std::min(x, n - 1)]);
    }

    inline i32 idx(i64 x) {
        return static_cast<i32>(std::lower_bound(z.begin(), z.end(), x) - z.begin());
    }
};

#endif // LIB_LICHAO_TREE_HPP
