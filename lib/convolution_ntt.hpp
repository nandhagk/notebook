#ifndef LIB_CONVOLUTION_NTT_HPP
#define LIB_CONVOLUTION_NTT_HPP 1

#include <lib/bits.hpp>
#include <lib/ntt.hpp>

template <typename Z>
std::vector<Z> convolution_ntt(std::vector<Z> l, std::vector<Z> r) {
    if (l.empty() || r.empty()) return {};

    const i32 n = static_cast<i32>(l.size());
    const i32 m = static_cast<i32>(r.size());
    const i32 sz = 1 << topbit(2 * (n + m - 1) - 1);

    l.resize(sz);
    ntt(l, false);

    r.resize(sz);
    ntt(r, false);

    for (i32 i = 0; i < sz; ++i) l[i] *= r[i];

    ntt(l, true);
    l.resize(n + m - 1);

    return l;
}

#endif // LIB_CONVOLUTION_NTT_HPP
