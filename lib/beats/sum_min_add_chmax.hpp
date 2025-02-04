#ifndef LIB_BEATS_SUM_MIN_ADD_CHMAX_HPP
#define LIB_BEATS_SUM_MIN_ADD_CHMAX_HPP 1

#include <algorithm>

#include <lib/monoids/add_chmax.hpp>
#include <lib/prelude.hpp>

template <typename T>
struct beats_sum_min {
    struct X {
        T sum, min, minc, min2;
        bool fail;
    };

    using ValueT = X;
    static constexpr X op(const X &x, const X &y) {
        if (x.min == inf<T>) return y;
        if (y.min == inf<T>) return x;

        X z;
        z.sum = x.sum + y.sum;
        z.min = std::min(x.min, y.min);
        z.minc = (x.min == z.min ? x.minc : 0) + (y.min == z.min ? y.minc : 0);
        z.min2 = inf<T>;

        if (z.min < x.min && x.min < z.min2) z.min2 = x.min;
        if (z.min < x.min2 && x.min2 < z.min2) z.min2 = x.min2;
        if (z.min < y.min && y.min < z.min2) z.min2 = y.min;
        if (z.min < y.min2 && y.min2 < z.min2) z.min2 = y.min2;

        z.fail = false;
        return z;
    }

    static constexpr X unit() {
        return {0, inf<T>, 0, inf<T>, 0};
    }

    static constexpr X from_element(T x) {
        return {x, x, 1, x, 0};
    }

    static constexpr bool failed(const X &x) {
        return x.fail;
    }

    static constexpr bool commutative = true;
};

template <typename T>
struct beats_sum_min_add_chmax {
    using MX = beats_sum_min<T>;
    using MA = monoid_add_chmax<T>;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    static constexpr X act(const X &z, const A &a, i32 sz) {
        assert(!z.fail);
        if (z.min == inf<T>) return z;

        X x = z;
        const auto &[add, ma] = a;

        x.sum += add * sz;
        x.min += add;
        x.min2 += add;

        if (ma == -inf<T>) return x;

        T before_max = x.min;
        x.min = std::max(x.min, ma);
        if (x.minc == sz) {
            x.min2 = x.min;
            x.sum = x.min * sz;
        } else if (x.min2 < x.min) {
            x.sum += (x.min - before_max) * x.minc;
        } else {
            x.fail = true;
        }

        return x;
    }
};

#endif // LIB_BEATS_SUM_MIN_ADD_CHMAX_HPP
