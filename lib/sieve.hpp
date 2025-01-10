#ifndef LIB_SIEVE_HPP
#define LIB_SIEVE_HPP 1

#include <cassert>
#include <numeric>
#include <vector>

#include <lib/prelude.hpp>

struct sieve {
    i32 n;
    std::vector<i32> lp, pr;

    sieve() {}
    explicit sieve(i32 m) {
        build(m);
    }

    void build(i32 m) {
        n = m;
        pr.clear();
        lp.resize(n + 1);

        for (i32 i = 2; i <= n; ++i) {
            if (lp[i] == 0) {
                lp[i] = i;
                pr.push_back(i);
            }

            for (i32 j = 0; i * pr[j] <= n; ++j) {
                lp[i * pr[j]] = pr[j];
                if (pr[j] == lp[i]) break;
            }
        }
    }

    std::vector<std::pair<i32, i32>> factorize(i32 k) const {
        assert(k <= n);

        std::vector<std::pair<i32, i32>> f;
        while (k != 1) {
            i32 p = lp[k];

            i32 c{};
            do {
                k /= p;
                ++c;
            } while (k % p == 0);

            f.emplace_back(p, c);
        }

        return f;
    }

    i32 totient(i32 k) const {
        assert(k <= n);

        for (const auto &[p, _] : factorize(k)) k -= k / p;
        return k;
    }

    std::vector<i32> totient() const {
        std::vector<i32> to(n + 1);
        std::iota(to.begin(), to.end(), 0);

        for (const i32 p : pr)
            for (i32 j = p; j <= n; j += p) to[j] -= to[j] / p;

        return to;
    }
};

#endif // LIB_SIEVE_HPP
