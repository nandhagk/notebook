#ifndef LIB_LINEAR_ADD_RANGE_MIN_HPP
#define LIB_LINEAR_ADD_RANGE_MIN_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>

template <typename T, typename TN, typename TN2>
struct linear_add_range_min {
    struct point {
        i32 x;
        T y;
        static TN cross(const point &a, const point &b, const point &c) {
            return (TN)(b.y - a.y) * (c.x - a.x) - (TN)(c.y - a.y) * (b.x - a.x);
        }
    };

    struct node {
        point lbr, rbr;
        T lza, lzb;

        node(i32 x, T y)
            : lbr{x, y}, rbr{x, y}, lza(0), lzb(0) {}

        node()
            : lza(0), lzb(0) {}
    };

public:
    linear_add_range_min() {}

    explicit linear_add_range_min(i32 m) {
        build(m);
    }

    explicit linear_add_range_min(const std::vector<T> &v) {
        build(v);
    }

    template <typename F>
    linear_add_range_min(i32 m, F f) {
        build(m, f);
    }

    void build(i32 m) {
        build(m, [](i32) -> T { return T(0); });
    }

    void build(const std::vector<T> &v) {
        build(static_cast<i32>(v.size()), [&](i32 i) -> T { return v[i]; });
    }

    template <typename F>
    void build(i32 m, F f) {
        n = m;

        log = 1;
        while ((1 << log) < n) ++log;

        size = 1 << log;

        nd = std::vector<node>(2 * size);
        correct = std::vector<bool>(2 * size, true);

        for (i32 i = 0; i < size; ++i) nd[size + i] = node(i, (i < n ? f(i) : 0));
        for (i32 i = size - 1; i >= 1; --i) pull(i);
    }
 
    T get(i32 p) {
        assert(0 <= p && p < n);

        p += size;

        T a = 0, b = 0;
        for (i32 i = log; i >= 1; --i) {
            a += nd[p].lza;
            b += nd[p].lzb;
        }

        return nd[p].lbr.y + (p - size) * a + b;
    }

    T prod(i32 l, i32 r) {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return inf<T>;

        l += size;
        r += size;

        for (i32 i = log; i >= 1; --i) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r - 1) >> i);
        }

        T res = inf<T>;
        for (; l < r; l >>= 1, r >>= 1) {
            if (l & 1) res = std::min(res, min_subtree(l++));
            if (r & 1) res = std::min(res, min_subtree(--r));
        }

        return res;
    }

    T prod_assume_add(i32 l, i32 r, T a, T b) {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return inf<T>;

        l += size;
        r += size;

        for (i32 i = log; i >= 1; --i) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r - 1) >> i);
        }

        T res = inf<T>;
        for (; l < r; l >>= 1, r >>= 1) {
            if (l & 1) res = std::min(res, min_subtree(l++, a, b));
            if (r & 1) res = std::min(res, min_subtree(--r, a, b));
        }

        return res;
    }

    void apply(i32 l, i32 r, T a, T b) {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return;

        l += size;
        r += size;

        for (i32 p = l, q = r; p < q; p >>= 1, q >>= 1) {
            if (p & 1) all_apply(p++, a, b);
            if (q & 1) all_apply(--q, a, b);
        }

        for (i32 i = 1; i <= log; ++i) {
            if (((l >> i) << i) != l) correct[l >> i] = false;
            if (((r >> i) << i) != r) correct[(r - 1) >> i] = false;
        }
    }

private:
    i32 n, size, log;
    std::vector<node> nd;
    std::vector<bool> correct;

    void all_apply(i32 k, T a, T b) {
        nd[k].lbr.y += a * nd[k].lbr.x + b;
        nd[k].rbr.y += a * nd[k].rbr.x + b;
        if (k < size) nd[k].lza += a, nd[k].lzb += b;
    }

    void push(i32 k) {
        all_apply(2 * k, nd[k].lza, nd[k].lzb);
        all_apply(2 * k + 1, nd[k].lza, nd[k].lzb);
        nd[k].lza = nd[k].lzb = 0;
    }

    i32 leftmost(i32 k) const {
        i32 msb = topbit(k);
        return (k - (1 << msb)) << (log - msb);
    }

    void pull(i32 k) {
        assert(k < size);

        i32 l = k * 2;
        i32 r = k * 2 + 1;

        push(k);

        if (!correct[l]) pull(l);
        if (!correct[r]) pull(r);

        i32 splitx = leftmost(r);
        T lza = 0, lzb = 0, lzA = 0, lzB = 0;
        point a = nd[l].lbr, b = nd[l].rbr, c = nd[r].lbr, d = nd[r].rbr;

#define movel(f)                                                                                                       \
    {                                                                                                                  \
        lza += nd[l].lza, lzb += nd[l].lzb;                                                                            \
        l = l * 2 + f;                                                                                                 \
        a = nd[l].lbr, b = nd[l].rbr;                                                                                  \
        a.y += lza * a.x + lzb;                                                                                        \
        b.y += lza * b.x + lzb;                                                                                        \
    }

#define mover(f)                                                                                                       \
    {                                                                                                                  \
        lzA += nd[r].lza, lzB += nd[r].lzb;                                                                            \
        r = r * 2 + f;                                                                                                 \
        c = nd[r].lbr, d = nd[r].rbr;                                                                                  \
        c.y += lzA * c.x + lzB;                                                                                        \
        d.y += lzA * d.x + lzB;                                                                                        \
    }

        while ((l < size) || (r < size)) {
            TN s1 = point::cross(a, b, c);
            if (l < size && s1 > 0) {
                movel(0);
            } else if (r < size && point::cross(b, c, d) > 0) {
                mover(1);
            } else if (l >= size) {
                mover(0);
            } else if (r >= size) {
                movel(1);
            } else {
                TN2 s2 = point::cross(b, a, d);

                // clang-format off
                if (s1 + s2 == 0 || (TN2)s1 * (d.x - splitx) < s2 * (splitx - c.x)) {
                    movel(1);
                } else {
                    mover(0);
                }

                // clang-format on
            }
        }

        nd[k].lbr = a;
        nd[k].rbr = c;
        correct[k] = true;

#undef movel
#undef mover
    }

    T min_subtree(i32 k, T a = 0, T b = 0) {
        if (!correct[k]) pull(k);

        while (k < size) {
            bool f = (nd[k].lbr.y - nd[k].rbr.y) > a * (nd[k].rbr.x - nd[k].lbr.x);
            a += nd[k].lza;
            b += nd[k].lzb;
            k = k * 2 + f;
        }

        return nd[k].lbr.y + a * nd[k].lbr.x + b;
    }
};

#endif // LIB_LINEAR_ADD_RANGE_MIN_HPP
