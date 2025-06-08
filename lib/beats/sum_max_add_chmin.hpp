#ifndef LIB_BEATS_SUM_MAX_ADD_CHMIN_HPP
#define LIB_BEATS_SUM_MAX_ADD_CHMIN_HPP 1

#include <algorithm>

#include <lib/limits.hpp>
#include <lib/monoids/add_chmin.hpp>
#include <lib/prelude.hpp>

template <typename T>
struct beats_sum_max {
    struct X {
        T sum, max, maxc, max2;
        bool fail;
    };

    using ValueT = X;
    static constexpr X op(const X &x, const X &y) {
        if (x.max == -inf<T>) return y;
        if (y.max == -inf<T>) return x;

        X z;
        z.sum = x.sum + y.sum;
        z.max = std::max(x.max, y.max);
        z.maxc = (x.max == z.max ? x.maxc : 0) + (y.max == z.max ? y.maxc : 0);
        z.max2 = -inf<T>;

        if (z.max > x.max && x.max > z.max2) z.max2 = x.max;
        if (z.max > x.max2 && x.max2 > z.max2) z.max2 = x.max2;
        if (z.max > y.max && y.max > z.max2) z.max2 = y.max;
        if (z.max > y.max2 && y.max2 > z.max2) z.max2 = y.max2;

        z.fail = false;
        return z;
    }

    static constexpr X unit() {
        return {0, -inf<T>, 0, -inf<T>, 0};
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
struct beats_sum_max_add_chmin {
    using MX = beats_sum_max<T>;
    using MA = monoid_add_chmin<T>;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    static constexpr X act(const X &z, const A &a, i32 sz) {
        assert(!z.fail);
        if (z.max == -inf<T>) return z;

        X x = z;
        const auto &[add, mi] = a;

        x.sum += add * sz;
        x.max += add;
        x.max2 += add;

        if (mi == inf<T>) return x;

        T before_max = x.max;
        x.max = std::min(x.max, mi);
        if (x.maxc == sz) {
            x.max2 = x.max;
            x.sum = x.max * sz;
        } else if (x.max2 < x.max) {
            x.sum += (x.max - before_max) * x.maxc;
        } else {
            x.fail = true;
        }

        return x;
    }
};

#endif // LIB_BEATS_SUM_MAX_ADD_CHMIN_HPP
