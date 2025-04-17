#ifndef LIB_ENUMERATE_RUNS_HPP
#define LIB_ENUMERATE_RUNS_HPP 1

#include <vector>

#include <lib/prelude.hpp>
#include <lib/lyndon.hpp>

// Compute all runs in given string
// Complexity: $O(N \log N)$ in this implementation (Theoretically $O(N)$ achievable)
// N = 2e5 -> ~800 ms
// Reference:
// [1] H. Bannai et al., "The "Runs" Theorem,"
// SIAM Journal on Computing, 46.5, 1501-1514, 2017.
inline std::vector<std::tuple<i32, i32, i32>> enumerate_runs(std::string s) {
    if (s.empty()) return {};

    lcp rh(s);
    std::reverse(s.begin(), s.end());

    lcp rrev(s);
    std::reverse(s.begin(), s.end());

    auto t = s;
    
    const auto lo = *std::min_element(s.begin(), s.end());
    const auto hi = *std::max_element(s.begin(), s.end());
    for (auto &c : t) c = static_cast<char>(hi - (c - lo));

    const auto l1 = longest_lyndon_prefixes(s, rh);
    const auto l2 = longest_lyndon_prefixes(t, rh);

    const i32 n = static_cast<i32>(s.size());

    std::vector<std::tuple<i32, i32, i32>> ret;
    for (i32 i = 0; i < n; ++i) {
        i32 j = i + l1[i];
        i32 L = i - rrev.prod(n - i, n - j);
        i32 R = j + rh.prod(i, j);
        if (R - L >= (j - i) * 2) ret.emplace_back(j - i, L, R);

        if (l1[i] != l2[i]) {
            j = i + l2[i];
            L = i - rrev.prod(n - i, n - j);
            R = j + rh.prod(i, j);
            if (R - L >= (j  - i) * 2) ret.emplace_back(j - i, L, R);
        }
    }

    std::sort(ret.begin(), ret.end());
    ret.erase(std::unique(ret.begin(), ret.end()), ret.end());

    return ret;
}

#endif // LIB_ENUMERATE_RUNS_HPP
