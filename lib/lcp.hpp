#ifndef LIB_LCP_HPP
#define LIB_LCP_HPP 1

#include <vector>

#include <lib/lcp_array.hpp>
#include <lib/monoids/min.hpp>
#include <lib/prelude.hpp>
#include <lib/sparse_table.hpp>
#include <lib/suffix_array.hpp>

struct lcp {
    lcp() {}

    explicit lcp(const std::string &s) {
        build(s);
    }

    template <typename T>
    explicit lcp(const std::vector<T> &s) {
        build(s);
    }

    void build(const std::string &s) {
        const i32 p = static_cast<i32>(s.size());

        std::vector<i32> t(p);
        for (i32 i = 0; i < p; ++i) t[i] = s[i];

        build(t);
    }

    template <typename T>
    void build(const std::vector<T> &s) {
        n = static_cast<i32>(s.size());

        const auto sa = suffix_array(s);
        const auto lc = lcp_array(s, sa);

        rnk.resize(n);
        for (i32 i = 0; i < n; ++i) rnk[sa[i]] = i;

        st.build(lc);
    }

    i32 prod(i32 p, i32 q) const {
        assert(0 <= p && p <= n);
        assert(0 <= q && q <= n);

        if (p == q) return n - p;
        if (p == n || q == n) return 0;

        const auto &[l, r] = std::minmax(rnk[p], rnk[q]);
        return st.prod(l, r);
    }

    i32 n;
    std::vector<i32> rnk;
    sparse_table<monoid_min<i32>> st;
};

#endif // LIB_LCP_HPP
