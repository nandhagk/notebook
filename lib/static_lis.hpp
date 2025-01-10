#ifndef LIB_STATIC_LIS_HPP
#define LIB_STATIC_LIS_HPP 1

#include <vector>

#include <lib/prelude.hpp>
#include <lib/wavelet_matrix.hpp>

using iter = typename std::vector<i32>::iterator;

static constexpr i32 NONE = -1;

inline std::vector<i32> inverse(const std::vector<i32> &p) {
    const i32 n = static_cast<i32>(p.size());
    std::vector<i32> q(n, NONE);
    for (i32 i = 0; i < n; ++i)
        if (p[i] != NONE) q[p[i]] = i;
    return q;
}

inline void unit_monge_dmul(const i32 n, iter stack, const iter a, const iter b) {
    if (n == 1) {
        stack[0] = 0;
        return;
    }

    const iter c_row = stack;
    stack += n;
    const iter c_col = stack;
    stack += n;

    const auto map = [=](const i32 len, const auto f, const auto g) {
        const iter a_h = stack + 0 * len;
        const iter a_m = stack + 1 * len;
        const iter b_h = stack + 2 * len;
        const iter b_m = stack + 3 * len;
        const auto split = [=](const iter v, iter v_h, iter v_m) {
            for (i32 i = 0; i < n; ++i) {
                if (f(v[i])) {
                    *v_h = g(v[i]);
                    ++v_h;
                    *v_m = i;
                    ++v_m;
                }
            }
        };
        split(a, a_h, a_m);
        split(b, b_h, b_m);
        const iter c = stack + 4 * len;
        unit_monge_dmul(len, c, a_h, b_h);
        for (i32 i = 0; i < len; ++i) {
            const i32 row = a_m[i];
            const i32 col = b_m[c[i]];
            c_row[row] = col;
            c_col[col] = row;
        }
    };

    const i32 mid = n / 2;
    map(mid, [mid](const i32 x) { return x < mid; }, [](const i32 x) { return x; });
    map(n - mid, [mid](const i32 x) { return x >= mid; }, [mid](const i32 x) { return x - mid; });

    struct d_itr {
        i32 delta;
        i32 col;
        d_itr()
            : delta(0), col(0) {}
    };

    i32 row = n;
    const auto right = [&](d_itr &it) {
        if (b[it.col] < mid) {
            if (c_col[it.col] >= row) it.delta += 1;
        } else {
            if (c_col[it.col] < row) it.delta += 1;
        }
        it.col += 1;
    };

    const auto up = [&](d_itr &it) {
        if (a[row] < mid) {
            if (c_row[row] >= it.col) it.delta -= 1;
        } else {
            if (c_row[row] < it.col) it.delta -= 1;
        }
    };

    d_itr neg, pos;
    while (row != 0) {
        while (pos.col != n) {
            d_itr temp = pos;
            right(temp);
            if (temp.delta == 0)
                pos = temp;
            else
                break;
        }

        row -= 1;
        up(neg);
        up(pos);
        while (neg.delta != 0) right(neg);
        if (neg.col > pos.col) c_row[row] = pos.col;
    }
}

inline std::vector<i32> subunit_monge_dmul(std::vector<i32> a, std::vector<i32> b) {
    const i32 n = static_cast<i32>(a.size());

    std::vector<i32> a_inv = inverse(a);
    std::vector<i32> b_inv = inverse(b);

    std::swap(b, b_inv);
    std::vector<i32> a_map, b_map;

    for (i32 i = n - 1; i >= 0; --i) {
        if (a[i] != NONE) {
            a_map.push_back(i);
            a[n - static_cast<i32>(a_map.size())] = a[i];
        }
    }

    std::reverse(a_map.begin(), a_map.end());

    {
        i32 cnt = 0;
        for (i32 i = 0; i < n; ++i) {
            if (a_inv[i] == NONE) {
                a[cnt] = i;
                cnt += 1;
            }
        }
    }

    for (i32 i = 0; i < n; ++i) {
        if (b[i] != NONE) {
            b[b_map.size()] = b[i];
            b_map.push_back(i);
        }
    }

    {
        i32 cnt = static_cast<i32>(b_map.size());
        for (i32 i = 0; i < n; ++i) {
            if (b_inv[i] == NONE) {
                b[cnt] = i;
                cnt += 1;
            }
        }
    }

    std::vector<i32> c([](i32 m) {
        i32 ret = 0;
        while (m > 1) {
            ret += 2 * m;
            m = (m + 1) / 2;
            ret += 4 * m;
        }
        ret += 1;
        return ret;
    }(n));
    unit_monge_dmul(n, c.begin(), a.begin(), b.begin());

    std::vector<i32> c_pad(n, NONE);
    for (i32 i = 0; i < static_cast<i32>(a_map.size()); ++i) {
        const i32 t = c[n - static_cast<i32>(a_map.size()) + i];
        if (t < static_cast<i32>(b_map.size())) c_pad[a_map[i]] = b_map[t];
    }

    return c_pad;
}

inline std::vector<i32> seaweed_doubling(const std::vector<i32> &p) {
    const i32 n = static_cast<i32>(p.size());
    if (n == 1) return std::vector<i32>({NONE});

    const i32 mid = n / 2;
    std::vector<i32> lo, hi;
    std::vector<i32> lo_map, hi_map;

    lo.reserve(n);
    hi.reserve(n);
    lo_map.reserve(n);
    hi_map.reserve(n);

    for (i32 i = 0; i < n; ++i) {
        const i32 e = p[i];
        if (e < mid) {
            lo.push_back(e);
            lo_map.push_back(i);
        } else {
            hi.push_back(e - mid);
            hi_map.push_back(i);
        }
    }

    lo = seaweed_doubling(lo);
    hi = seaweed_doubling(hi);
    std::vector<i32> lo_pad(n), hi_pad(n);
    std::iota(lo_pad.begin(), lo_pad.end(), 0);
    std::iota(hi_pad.begin(), hi_pad.end(), 0);

    for (i32 i = 0; i < mid; ++i) lo_pad[lo_map[i]] = lo[i] == NONE ? NONE : lo_map[lo[i]];
    for (i32 i = 0; mid + i < n; ++i) hi_pad[hi_map[i]] = hi[i] == NONE ? NONE : hi_map[hi[i]];

    return subunit_monge_dmul(std::move(lo_pad), std::move(hi_pad));
}

template <typename T>
struct static_lis {
    i32 n;
    wavelet_matrix<i32> wm;

    static_lis() {}
    explicit static_lis(const std::vector<T> &v) {
        build(v);
    }

    void build(const std::vector<T> &v) {
        n = static_cast<i32>(v.size());

        std::vector<i32> vi(n);
        std::iota(vi.begin(), vi.end(), 0);
        std::sort(vi.begin(), vi.end(), [&](i32 i, i32 j) { return v[i] == v[j] ? i > j : v[i] < v[j]; });

        std::vector<i32> p(n);
        for (i32 i = 0; i < n; ++i) p[vi[i]] = i;

        auto row = seaweed_doubling(p);
        for (i32 &e : row)
            if (e == NONE) e = n;

        wm.build(row);
    }

    i32 prod(i32 l, i32 r) const {
        assert(0 <= l && l <= r && r <= n);

        return r - l - wm.count(l, n, r);
    }
};

#endif // LIB_STATIC_LIS_HPP
