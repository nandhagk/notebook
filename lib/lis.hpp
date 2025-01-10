#ifndef LIB_LIS_HPP
#define LIB_LIS_HPP 1

#include <algorithm>
#include <vector>

#include <lib/prelude.hpp>

template <typename T>
inline std::vector<i32> lis(const std::vector<T> &a) {
    const i32 n = static_cast<i32>(a.size());

    std::vector<std::pair<T, i32>> dp;
    dp.reserve(n);

    std::vector<i32> prv(n, -1);
    for (i32 i = 0; i < n; ++i) {
        auto it = std::lower_bound(dp.begin(), dp.end(), std::make_pair(a[i], -i));

        if (it != dp.begin()) prv[i] = -std::prev(it)->second;
        if (it == dp.end()) dp.emplace_back(a[i], -i);
        else
            *it = {a[i], -i};
    }

    std::vector<i32> seq;
    seq.reserve(n);

    for (i32 i = -dp.rbegin()->second; i >= 0; i = prv[i]) seq.push_back(i);
    std::reverse(seq.begin(), seq.end());

    return seq;
}

#endif // LIB_LIS_HPP
