#ifndef LIB_MONOID_MISRA_GRIES_HPP
#define LIB_MONOID_MISRA_GRIES_HPP 1

#include <algorithm>
#include <utility>
#include <vector>

#include <lib/prelude.hpp>

template <typename T, usize K>
struct monoid_misra_gries {
	static_assert(K >= 2);

    using X = std::vector<std::pair<T, u32>>;
    using ValueT = X;

    static X op(const X &x, const X &y) {
        X z;
        z.reserve(2 * (K - 1));

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

		while (z.size() >= K) {
			const u32 c = std::min_element(z.begin(), z.end(), [](const auto &a, const auto &b) { return a.second < b.second; })->second;
            for (auto &p : z) p.second -= c;
			z.erase(std::remove_if(z.begin(), z.end(), [&](const auto &p) { return p.second <= 0; }), z.end());
        }

        return z;
    }

    static X unit() {
        return X{};
    }

    static X from_element(const T &t) {
        return X{{t, 1}};
    }

    static constexpr bool commutative = true;
};

#endif // LIB_MONOID_MISRA_GRIES_HPP
