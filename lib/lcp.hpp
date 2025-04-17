#ifndef LIB_LCP_HPP
#define LIB_LCP_HPP 1

#include <vector>

#include <lib/prelude.hpp>
#include <lib/suffix_array.hpp>
#include <lib/lcp_array.hpp>
#include <lib/monoids/min.hpp>
#include <lib/sparse_table.hpp>

struct lcp {
    lcp() {}

    explicit lcp(const std::string &s) {
        build(s);
    }

    void build(const std::string &s) {
        n = static_cast<i32>(s.size());

        const auto sa = suffix_array(s);
        const auto lc = lcp_array(s, sa);

        rnk.resize(n);
        for (i32 i = 0; i < n; ++i) rnk[sa[i]] = i;

        st.build(lc);
    }

    i32 prod(i32 l, i32 r) const {
        assert(0 <= l && l <= n);
        assert(0 <= r && r <= n);

        if (l == r) return n - l;
        if (l == n || r == n) return 0;

        l = rnk[l];
        r = rnk[r];

        if (l > r) std::swap(l, r);
        return st.prod(l, r);
    }

    i32 n;
    std::vector<i32> rnk;
    sparse_table<monoid_min<i32>> st;
};

#endif // LIB_LCP_HPP
