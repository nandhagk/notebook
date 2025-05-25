#ifndef LIB_COMPRESS_HPP
#define LIB_COMPRESS_HPP 1

#include <algorithm>
#include <numeric>
#include <vector>

#include <lib/prelude.hpp>

template <typename T>
std::vector<i32> compress(const std::vector<T> &v) {
    const i32 n = static_cast<i32>(v.size());

    std::vector<i32> vi(n);
    std::iota(vi.begin(), vi.end(), 0);
    std::sort(vi.begin(), vi.end(), [&](i32 i, i32 j) { return v[i] == v[j] ? i < j : v[i] < v[j]; });

    std::vector<i32> rv;
    rv.reserve(n);

    std::vector<i32> b(n);

    for (const i32 i : vi) {
        if (rv.empty() || rv.back() != v[i]) rv.push_back(v[i]);
        b[i] = static_cast<i32>(rv.size()) - 1;
    }

    return b;
}

#endif // LIB_COMPRESS_HPP
