#ifndef LIB_LICHAO_TREE_HPP
#define LIB_LICHAO_TREE_HPP 1

#include <algorithm>
#include <vector>

#include <lib/prelude.hpp>

template <typename T>
struct line {
    using ValueT = T;

    T a, b;

    line()
        : line(0, 0) {}

    explicit line(T d)
        : line(0, d) {}

    line(T c, T d)
        : a{c}, b{d} {}

    T eval(T x) const {
        return a * x + b;
    }

    T operator()(T x) const {
        return eval(x);
    }
};

template <typename L, bool MIN = true>
struct lichao_tree {
    using T = typename L::ValueT;

    i32 n, log, size;
    std::vector<T> x;
    std::vector<L> d;

    lichao_tree() {}

    explicit lichao_tree(const std::vector<T> &p, const L &f) {
        build(p, f);
    }

    void build(const std::vector<T> &p, const L &f) {
        x = p;

        std::sort(x.begin(), x.end());
        x.erase(std::unique(x.begin(), x.end()), x.end());
        x.shrink_to_fit();

        n = static_cast<i32>(x.size());

        log = 1;
        while ((1 << log) < n) ++log;

        size = 1 << log;
        d.assign(size << 1, f);
    }

    void add_line(const L &f) {
        add_line_at(1, f);
    }

    void add_segment(T xl, T xr, const L &f) {
        for (i32 l = idx(xl) + size, r = idx(xr) + size; l < r; l >>= 1, r >>= 1) {
            if (l & 1) add_line_at(l++, f);
            if (r & 1) add_line_at(--r, f);
        }
    }

    std::pair<T, L> query(T z) const {
        L f = d[0];
        T a = f(z);

        for (i32 i = idx(z) + size; i; i >>= 1) {
            L g = d[i];
            T b = g(z);

            if ((MIN && b < a) || (!MIN && b > a)) {
                f = g;
                a = b;
            }
        }

        return {a, f};
    }

private:
    inline i32 idx(T z) const {
        return static_cast<i32>(std::lower_bound(x.begin(), x.end(), z) - x.begin());
    }

    inline T eval(const L &f, i32 i) const {
        return f(x[std::min(i, n - 1)]);
    }

    void add_line_at(i32 i, L f) {
        const i32 k = topbit(i);

        i32 l = (size >> k) * (i - (1 << k));
        i32 r = l + (size >> k);

        while (l < r) {
            const L g = d[i];

            const T fl = eval(f, l);
            const T fr = eval(f, r - 1);
            const T gl = eval(g, l);
            const T gr = eval(g, r - 1);

            const bool bl = MIN ? fl < gl : fl > gl;
            const bool br = MIN ? fr < gr : fr > gr;

            if (bl && br) {
                d[i] = f;
                return;
            }

            if (!bl && !br) return;

            const i32 m = (l + r) / 2;

            const T fm = eval(f, m);
            const T gm = eval(g, m);

            const bool bm = MIN ? fm < gm : fm > gm;
            if (bm) {
                d[i] = f;
                f = g;
            }

            if (bm ^ bl) {
                i = (i << 1) | 0;
                r = m;
            } else {
                i = (i << 1) | 1;
                l = m;
            }
        }
    }
};

#endif // LIB_LICHAO_TREE_HPP
