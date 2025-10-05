#ifndef LIB_MONOID_MAJORITY_HPP
#define LIB_MONOID_MAJORITY_HPP 1

#include <algorithm>
#include <utility>
#include <vector>

#include <lib/prelude.hpp>

template <typename T, usize K>
struct monoid_majority {
    using X = std::vector<std::pair<T, u32>>;
    using ValueT = X;

    static constexpr X op(const X &x, const X &y) {
        X z;
        z.reserve(2 * K);

        auto it = x.begin();
        auto ti = y.begin();

        while (it != x.end() && ti != y.end()) {
            const auto &[u, v] = *it;
            const auto &[p, q] = *ti;

            if (u == p) {
                z.emplace_back(u, v + q);
                ++it;
                ++ti;
            } else if (u < p) {
                z.emplace_back(u, v);
                ++it;
            } else {
                z.emplace_back(p, q);
                ++ti;
            }
        }

        for (; it != x.end(); ++it) z.emplace_back(*it);
        for (; ti != y.end(); ++ti) z.emplace_back(*ti);

        if (z.size() > K) {
            for (auto &&[t, c] : z) --c;
            z.erase(std::remove_if(z.begin(), z.end(), [&](const auto &p) { return p.second <= 0; }), z.end());
            z.shrink_to_fit();
        }

        return z;
    }

    static constexpr X unit() {
        return X{};
    }

    static constexpr X from_element(const T &t) {
        return X{{t, 1}};
    }

    static constexpr bool commutative = true;
};

#endif // LIB_MONOID_MAJORITY_HPP
