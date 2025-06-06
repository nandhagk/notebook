#ifndef LIB_SUFFIX_ARRAY_HPP
#define LIB_SUFFIX_ARRAY_HPP

#include <algorithm>
#include <cassert>
#include <numeric>
#include <string>
#include <vector>

#include <lib/prelude.hpp>

inline std::vector<i32> sa_naive(const std::vector<i32> &s) {
    const i32 n = static_cast<i32>(s.size());

    std::vector<i32> sa(n);
    std::iota(sa.begin(), sa.end(), 0);
    std::sort(sa.begin(), sa.end(), [&](i32 l, i32 r) {
        if (l == r) return false;

        while (l < n && r < n) {
            if (s[l] != s[r]) return s[l] < s[r];
            ++l;
            ++r;
        }

        return l == n;
    });

    return sa;
}

inline std::vector<i32> sa_doubling(const std::vector<i32> &s) {
    const i32 n = static_cast<i32>(s.size());

    std::vector<i32> sa(n), rnk = s, tmp(n);
    std::iota(sa.begin(), sa.end(), 0);
    for (i32 k = 1; k < n; k *= 2) {
        const auto cmp = [&](i32 x, i32 y) {
            if (rnk[x] != rnk[y]) return rnk[x] < rnk[y];
            i32 rx = x + k < n ? rnk[x + k] : -1;
            i32 ry = y + k < n ? rnk[y + k] : -1;
            return rx < ry;
        };

        std::sort(sa.begin(), sa.end(), cmp);
        tmp[sa[0]] = 0;
        for (i32 i = 1; i < n; ++i) tmp[sa[i]] = tmp[sa[i - 1]] + (cmp(sa[i - 1], sa[i]) ? 1 : 0);

        std::swap(tmp, rnk);
    }

    return sa;
}

// SA-IS, linear-time suffix array construction
// Reference:
// G. Nong, S. Zhang, and W. H. Chan,
// Two Efficient Algorithms for Linear Time Suffix Array
// Construction
template <i32 THRESHOLD_NAIVE = 10, i32 THRESHOLD_DOUBLING = 40>
std::vector<i32> sa_is(const std::vector<i32> &s, int upper) {
    const i32 n = static_cast<i32>(s.size());
    if (n == 0) return {};
    if (n == 1) return {0};
    if (n == 2) {
        if (s[0] < s[1])
            return {0, 1};
        else
            return {1, 0};
    }

    if (n < THRESHOLD_NAIVE) return sa_naive(s);
    if (n < THRESHOLD_DOUBLING) return sa_doubling(s);

    std::vector<i32> sa(n);
    std::vector<bool> ls(n);
    for (i32 i = n - 2; i >= 0; --i) ls[i] = (s[i] == s[i + 1]) ? ls[i + 1] : (s[i] < s[i + 1]);

    std::vector<i32> sum_l(upper + 1), sum_s(upper + 1);
    for (i32 i = 0; i < n; ++i)
        if (!ls[i])
            ++sum_s[s[i]];
        else
            ++sum_l[s[i] + 1];

    for (i32 i = 0; i <= upper; ++i) {
        sum_s[i] += sum_l[i];
        if (i < upper) sum_l[i + 1] += sum_s[i];
    }

    const auto induce = [&](const std::vector<i32> &lms) {
        std::fill(sa.begin(), sa.end(), -1);

        std::vector<i32> buf(upper + 1);
        std::copy(sum_s.begin(), sum_s.end(), buf.begin());
        for (const i32 d : lms) {
            if (d == n) continue;
            sa[buf[s[d]]++] = d;
        }

        std::copy(sum_l.begin(), sum_l.end(), buf.begin());
        sa[buf[s[n - 1]]++] = n - 1;
        for (i32 i = 0; i < n; ++i) {
            i32 v = sa[i];
            if (v >= 1 && !ls[v - 1]) sa[buf[s[v - 1]]++] = v - 1;
        }

        std::copy(sum_l.begin(), sum_l.end(), buf.begin());
        for (i32 i = n - 1; i >= 0; --i) {
            i32 v = sa[i];
            if (v >= 1 && ls[v - 1]) sa[--buf[s[v - 1] + 1]] = v - 1;
        }
    };

    std::vector<i32> lms_map(n + 1, -1);
    i32 m = 0;
    for (i32 i = 1; i < n; ++i)
        if (!ls[i - 1] && ls[i]) lms_map[i] = m++;

    std::vector<i32> lms;
    lms.reserve(m);
    for (i32 i = 1; i < n; ++i)
        if (!ls[i - 1] && ls[i]) lms.push_back(i);

    induce(lms);
    if (m) {
        std::vector<i32> sorted_lms;
        sorted_lms.reserve(m);
        for (const i32 v : sa)
            if (lms_map[v] != -1) sorted_lms.push_back(v);

        std::vector<i32> rec_s(m);
        i32 rec_upper = 0;
        rec_s[lms_map[sorted_lms[0]]] = 0;

        for (i32 i = 1; i < m; ++i) {
            i32 l = sorted_lms[i - 1], r = sorted_lms[i];
            i32 end_l = (lms_map[l] + 1 < m) ? lms[lms_map[l] + 1] : n;
            i32 end_r = (lms_map[r] + 1 < m) ? lms[lms_map[r] + 1] : n;

            bool same = true;
            if (end_l - l != end_r - r) {
                same = false;
            } else {
                while (l < end_l) {
                    if (s[l] != s[r]) break;

                    ++l;
                    ++r;
                }

                if (l == n || s[l] != s[r]) same = false;
            }

            if (!same) rec_upper++;
            rec_s[lms_map[sorted_lms[i]]] = rec_upper;
        }

        const auto rec_sa = sa_is<THRESHOLD_NAIVE, THRESHOLD_DOUBLING>(rec_s, rec_upper);
        for (i32 i = 0; i < m; ++i) sorted_lms[i] = lms[rec_sa[i]];

        induce(sorted_lms);
    }

    return sa;
}

inline std::vector<i32> suffix_array(const std::vector<i32> &s, i32 upper) {
    assert(0 <= upper);
    for (const i32 d : s) assert(0 <= d && d <= upper);

    const auto sa = sa_is(s, upper);
    return sa;
}

template <typename T>
std::vector<i32> suffix_array(const std::vector<T> &s) {
    const i32 n = static_cast<i32>(s.size());

    std::vector<i32> idx(n);
    iota(idx.begin(), idx.end(), 0);
    sort(idx.begin(), idx.end(), [&](i32 l, i32 r) { return s[l] < s[r]; });

    std::vector<i32> t(n);
    i32 now = 0;
    for (i32 i = 0; i < n; ++i) {
        if (i && s[idx[i - 1]] != s[idx[i]]) now++;
        t[idx[i]] = now;
    }

    return sa_is(t, now);
}

inline std::vector<i32> suffix_array(const std::string &s) {
    const i32 n = static_cast<i32>(s.size());

    std::vector<i32> t(n);
    for (i32 i = 0; i < n; ++i) t[i] = s[i];

    return sa_is(t, 255);
}

#endif // LIB_SUFFIX_ARRAY_HPP
