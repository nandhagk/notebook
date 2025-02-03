#ifndef LIB_BEATS_SUM_MIN_MAX_HPP
#define LIB_BEATS_SUM_MIN_MAX_HPP 1

#include <algorithm>

#include <lib/monoids/add_chmin_chmax.hpp>
#include <lib/prelude.hpp>

template <typename T>
struct sum_min_max {
    struct X {
        T sum, min, max, minc, maxc, min2, max2;
        bool fail;
    };

    using ValueT = X;
    static constexpr X op(const X &x, const X &y) {
        if (x.min > x.max) return y;
        if (y.min > y.max) return x;

        X z;
        z.sum = x.sum + y.sum;
        z.min = std::min(x.min, y.min);
        z.max = std::max(x.max, y.max);

        z.minc = (x.min == z.min ? x.minc : 0) + (y.min == z.min ? y.minc : 0);
        z.maxc = (x.max == z.max ? x.maxc : 0) + (y.max == z.max ? y.maxc : 0);

        z.min2 = z.max;
        if (z.min < x.min && x.min < z.min2) z.min2 = x.min;
        if (z.min < x.min2 && x.min2 < z.min2) z.min2 = x.min2;
        if (z.min < y.min && y.min < z.min2) z.min2 = y.min;
        if (z.min < y.min2 && y.min2 < z.min2) z.min2 = y.min2;

        z.max2 = z.min;
        if (z.max > x.max && x.max > z.max2) z.max2 = x.max;
        if (z.max > x.max2 && x.max2 > z.max2) z.max2 = x.max2;
        if (z.max > y.max && y.max > z.max2) z.max2 = y.max;
        if (z.max > y.max2 && y.max2 > z.max2) z.max2 = y.max2;

        z.fail = false;
        return z;
    }

    static constexpr X unit() {
        return {0, inf<T>, -inf<T>, 0, 0, inf<T>, -inf<T>, 0};
    }

    static constexpr X from_element(T x) {
        return {x, x, x, 1, 1, x, x, 0};
    }

    static constexpr bool failed(const X &x) {
        return x.fail;
    }

    static constexpr bool commutative = true;
};

template <typename T>
struct beats_sum_min_max {
    using MX = sum_min_max<T>;
    using MA = monoid_add_chmin_chmax<T>;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    static constexpr X act(const X &z, const A &a, i32 sz) {
        assert(!z.fail);
        if (z.min > z.max) return z;

        const auto &[add, mi, ma] = a;

        X x = z;
        x.sum += add * sz;
        x.min += add;
        x.max += add;
        x.min2 += add;
        x.max2 += add;

        if (mi == inf<T> && ma == -inf<T>) return x;

        T before_min = x.min;
        T before_max = x.max;

        x.min = std::min(x.min, mi);
        x.min = std::max(x.min, ma);

        x.max = std::min(x.max, mi);
        x.max = std::max(x.max, ma);

        if (x.min == x.max) {
            x.sum = x.max * sz;
            x.max2 = x.min2 = x.max;
            x.maxc = x.minc = sz;
        } else if (x.max2 <= x.min) {
            x.max2 = x.min;
            x.min2 = x.max;
            x.minc = sz - x.maxc;
            x.sum = x.max * x.maxc + x.min * x.minc;
        } else if (x.min2 >= x.max) {
            x.max2 = x.min;
            x.min2 = x.max;
            x.maxc = sz - x.minc;
            x.sum = x.max * x.maxc + x.min * x.minc;
        } else if (x.min < x.min2 && x.max > x.max2) {
            x.sum += (x.min - before_min) * x.minc + (x.max - before_max) * x.maxc;
        } else {
            x.fail = true;
        }

        return x;
    }
};

#endif // LIB_BEATS_SUM_MIN_MAX_HPP
