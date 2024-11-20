#ifndef LIB_SIEVE_HPP
#define LIB_SIEVE_HPP 1

#include <vector>
#include <cassert>
#include <numeric>

#include <lib/prelude.hpp>

struct sieve {
        i64 n;
        std::vector<i64> lp, pr;

        sieve() {}

        explicit sieve(i64 m) {
                build(m);
        }

        void build(i64 m) {
                n = m;
                lp.resize(n + 1);

                for (i64 i = 2; i <= n; ++i) {
                        if (lp[i] == 0) {
                                lp[i] = i;
                                pr.push_back(i);
                        }

                        for (i64 j = 0; i * pr[j] <= n; ++j) {
                                lp[i * pr[j]] = pr[j];
                                if (pr[j] == lp[i]) break;
                        }
                }
        }

        std::vector<std::pair<i64, i32>> factorize(i64 k) const {
                assert(k <= n);

                std::vector<std::pair<i64, i32>> f;
                while (k != 1) {
                        i64 p = lp[k];

                        i32 c{};
                        do { 
                                k /= p;
                                ++c;
                        } while (k % p == 0); 

                        f.emplace_back(p, c);
                }

                return f;
        }

        i64 totient(i64 k) const {
                for (const auto &[p, _] : factorize(k)) k -= k / p;
                return k;
        }

        std::vector<i64> totient() const {
                std::vector<i64> to(n + 1);
                std::iota(to.begin(), to.end(), 0);

                for (const i64 p : pr) {
                        for (i64 j = p; j <= n; j += p) {
                                to[j] -= to[j] / p;
                        }
                }

                return to;
        }
};

#endif // LIB_SIEVE_HPP
