#ifndef LIB_MEX_HPP
#define LIB_MEX_HPP 1

#include <vector>

#include <lib/prelude.hpp>

inline i32 mex(const std::vector<i32> &v) {
    const i32 n = static_cast<i32>(v.size());

    std::vector<bool> xs(n + 1);
    for (const i32 x : v)
        if (x < n) xs[x] = true;

    i32 mex = 0;
    while (xs[mex]) ++mex;

    return mex;
}

#endif // LIB_MEX_HPP
