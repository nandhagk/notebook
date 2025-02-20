#ifndef LIB_STATIC_MEX_HPP
#define LIB_STATIC_MEX_HPP

#include <cassert>
#include <vector>

#include <lib/monoids/min.hpp>
#include <lib/persistent_segment_tree.hpp>
#include <lib/prelude.hpp>

template <u32 MAXIDX>
struct static_mex {
    using S = persistent_segment_tree<monoid_min<i32>, MAXIDX>;

    i32 n;
    std::vector<S> st;

    static_mex() {}
    explicit static_mex(const std::vector<i32> &v) {
        build(v);
    }

    void build(const std::vector<i32> &v) {
        n = static_cast<i32>(v.size());

        st.resize(n + 1);
        st[0] = S(std::vector<i32>(n, -1));

        for (i32 r = 0; r < n; ++r)
            if (v[r] < n)
                st[r + 1] = st[r].set(v[r], r);
            else
                st[r + 1] = st[r];
    }

    i32 prod(i32 l, i32 r) const {
        assert(0 <= l && l <= r && r <= n);

        if (l == r) return 0;

        i32 lo = -1;
        i32 hi = n;

        while (hi - lo > 1) {
            const i32 m = (hi + lo) / 2;

            if (st[r].prod(0, m + 1) < l)
                hi = m;
            else
                lo = m;
        }

        return hi;
    }
};

#endif // LIB_STATIC_MEX_HPP
