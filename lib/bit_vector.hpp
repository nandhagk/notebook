#ifndef LIB_BIT_VECTOR_HPP
#define LIB_BIT_VECTOR_HPP 1

#include <vector>

#include <lib/prelude.hpp>

struct bit_vector {
    i32 n;
    std::vector<std::pair<u64, u32>> d;

    explicit bit_vector(i32 m) : n(m), d((n + 127) >> 6) {}

    void set(i32 i) {
        d[i >> 6].first |= static_cast<u64>(1) << (i & 63);
    }

    void reset() {
        d.assign((n + 127) >> 6, {0, 0});
    }

    void build() {
        for (i32 i = 0; i < static_cast<i32>(d.size()) - 1; ++i) d[i + 1].second = d[i].second + popcnt(d[i].first);
    }

    bool operator[](i32 i) const {
        return d[i >> 6].first >> (i & 63) & 1;
    }

    i32 rank1(i32 r) const {
        const auto &[a, b] = d[r >> 6];
        return b + popcnt(a & ((static_cast<u64>(1) << (r & 63)) - 1));
    }

    i32 rank0(i32 r) const {
        return r - rank1(r);
    }

    i32 rank1(i32 l, i32 r) const {
        return rank1(r) - rank1(l);
    }

    i32 rank0(i32 l, i32 r) const {
        return rank0(r) - rank0(l);
    }
};

#endif // LIB_BIT_VECTOR_HPP
