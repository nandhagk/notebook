#ifndef LIB_LYNDON_HPP
#define LIB_LYNDON_HPP 1

#include <string>
#include <vector>

#include <lib/lcp.hpp>
#include <lib/prelude.hpp>

// Lyndon factorization based on Duval's algorithm
// **NOT VERIFIED YET**
// Reference:
// [1] K. T. Chen, R. H. Fox, R. C. Lyndon,
//     "Free Differential Calculus, IV. The Quotient Groups of the Lower Central Series,"
//     Annals of Mathematics, 81-95, 1958.
// [2] J. P. Duval, "Factorizing words over an ordered alphabet,"
//     Journal of Algorithms, 4(4), 363-381, 1983.
// - https://cp-algorithms.com/string/lyndon_factorization.html
// - https://qiita.com/nakashi18/items/66882bd6e0127174267a
template <typename T>
std::vector<std::pair<i32, i32>> lyndon_factorization(const std::vector<T> &s) {
    const i32 n = static_cast<i32>(s.size());

    std::vector<std::pair<i32, i32>> ret;
    for (i32 l = 0; l < n;) {
        i32 i = l;
        i32 j = i + 1;

        while (j < n && s[i] <= s[j]) {
            i = (s[i] == s[j] ? i + 1 : l);
            ++j;
        }

        const i32 m = (j - l) / (j - i);
        for (i32 t = 0; t < m; t++) ret.emplace_back(l, j - i), l += j - i;
    }

    return ret;
}

inline std::vector<std::pair<i32, i32>> lyndon_factorization(const std::string &s) {
    const i32 n = i32(s.size());

    std::vector<i32> t(n);
    for (i32 i = 0; i < n; i++) t[i] = s[i];

    return lyndon_factorization(t);
}

// Compute the longest Lyndon prefix for each suffix s[i:N]
// (Our implementation is $O(N \log N)$)
// Example:
// - `teletelepathy` -> [1,4,1,2,1,4,1,2,1,4,1,2,1]
// Reference:
// [1] H. Bannai et al., "The "Runs" Theorem,"
// SIAM Journal on Computing, 46.5, 1501-1514, 2017.
template <typename T>
std::vector<i32> longest_lyndon_prefixes(const std::vector<T> &s, const lcp &rh) {
    const i32 n = static_cast<i32>(s.size());

    std::vector<std::pair<i32, i32>> st{{n, n}};
    std::vector<i32> ret(n);

    for (i32 i = n - 1, j = i; i >= 0; i--, j = i) {
        while (st.size() > 1) {
            const i32 iv = st.back().first;
            const i32 jv = st.back().second;
            const i32 l = rh.prod(i, iv);
            if (!(iv + l < n && s[i + l] < s[iv + l])) break;

            j = jv;
            st.pop_back();
        }

        st.emplace_back(i, j);
        ret[i] = j - i + 1;
    }

    return ret;
}

inline std::vector<i32> longest_lyndon_prefixes(const std::string &s, const lcp &rh) {
    const i32 n = i32(s.size());

    std::vector<i32> t(n);
    for (i32 i = 0; i < n; i++) t[i] = s[i];

    return longest_lyndon_prefixes(t, rh);
}

#endif // LIB_LYNDON_HPP
