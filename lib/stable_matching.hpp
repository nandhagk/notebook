#ifndef LIB_STABLE_MATCHING_HPP
#define LIB_STABLE_MATCHING_HPP 1

#include <numeric>
#include <vector>

#include <lib/prelude.hpp>

inline std::vector<i32> stable_matching(const std::vector<std::vector<i32>> &a,
                                        const std::vector<std::vector<i32>> &b) {
    const i32 n = static_cast<i32>(a.size());

    std::vector<std::vector<i32>> b_priority(n, std::vector<i32>(n));
    for (i32 i = 0; i < n; ++i)
        for (i32 j = 0; j < n; ++j) b_priority[i][b[i][j]] = j;

    std::vector<i32> a_propose(n), b_match(n, -1), unmatched(n);
    std::iota(unmatched.begin(), unmatched.end(), 0);

    while (!unmatched.empty()) {
        const i32 l = unmatched.back();
        unmatched.pop_back();

        const i32 r = a[l][a_propose[l]];
        if (b_match[r] == -1) {
            b_match[r] = l;
        } else if (b_priority[r][l] < b_priority[r][b_match[r]]) {
            ++a_propose[b_match[r]];
            unmatched.push_back(b_match[r]);
            b_match[r] = l;
        } else {
            ++a_propose[l];
            unmatched.push_back(l);
        }
    }

    std::vector<i32> a_match(n);
    for (i32 i = 0; i < n; ++i) a_match[b_match[i]] = i;

    return a_match;
}

#endif // LIB_STABLE_MATCHING_HPP
