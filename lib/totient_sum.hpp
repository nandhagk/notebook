#ifndef LIB_TOTIENT_SUM_HPP
#define LIB_TOTIENT_SUM_HPP 1

#include <cmath>
#include <vector>
#include <algorithm>

#include <lib/prelude.hpp>

inline i128 totient_sum(const i64 N) {
    i32 v = i32(std::sqrt(N));
    while (i64(v) * v < N) ++v;
    while (i64(v) * v > N) --v;

    std::vector<i32> primes;
    std::vector<i64> s0(v + 1), s1(v + 1), l0(v + 1);
    std::vector<i128> l1(v + 1);

    // phi
    const auto f = [&](i32 p, i32 e) -> i64 {
        i64 ret = p - 1;
        while (e > 1) --e, ret *= p;
        return ret;
    };

    const auto divide = [](i64 n, i64 d) -> i64 { return i64(f64(n) / d); };

    // sum f(p)
    for (i32 i = 1; i <= v; ++i) s0[i] = i - 1, s1[i] = i64(i) * (i + 1) / 2 - 1;
    for (i32 i = 1; i <= v; ++i) l0[i] = N / i - 1, l1[i] = i128(N / i) * (N / i + 1) / 2 - 1;
    for (i32 p = 2; p <= v; ++p) {
        if (s0[p] > s0[p - 1]) {
            primes.push_back(p);
            i64 q = i64(p) * p, M = N / p, t0 = s0[p - 1], t1 = s1[p - 1];
            i64 t = v / p, u = std::min<i64>(v, N / q);
            for (i64 i = 1; i <= t; ++i) l0[i] -= (l0[i * p] - t0), l1[i] -= (l1[i * p] - t1) * p;
            for (i64 i = t + 1; i <= u; ++i) l0[i] -= (s0[divide(M, i)] - t0), l1[i] -= (s1[divide(M, i)] - t1) * p;
            for (i32 i = v; i >= q; --i) s0[i] -= (s0[divide(i, p)] - t0), s1[i] -= (s1[divide(i, p)] - t1) * p;
        }
    }
    for (i32 i = 1; i <= v; ++i) s1[i] -= s0[i];
    for (i32 i = 1; i <= v; ++i) l1[i] -= l0[i];

    // sum g(n) (g(p^e) := f(p)^e)
    for (auto it = primes.rbegin(); it != primes.rend(); ++it) {
        i32 p = *it;
        i64 q = i64(p) * p, M = N / p, s = s1[p - 1];
        i64 t = v / p, u = std::min<i64>(v, N / q);
        for (i64 i = q; i <= v; ++i) s1[i] += (s1[divide(i, p)] - s) * f(p, 1);
        for (i64 i = u; i > t; --i) l1[i] += (s1[divide(M, i)] - s) * f(p, 1);
        for (i64 i = t; i >= 1; --i) l1[i] += (l1[i * p] - s) * f(p, 1);
    }

    for (i32 i = 1; i <= v; ++i) s1[i] += 1;
    for (i32 i = 1; i <= v; ++i) l1[i] += 1;

    // sum f(n)
    const auto dfs = [&](auto &&self, i64 n, usize beg, i64 coeff) -> i128 {
        if (!coeff) return 0;

        i128 ret = i128(coeff) * (n > v ? l1[divide(N, n)] : s1[n]);
        for (usize i = beg; i < primes.size(); ++i) {
            i32 p = primes[i];
            i64 q = i64(p) * p;
            if (q > n) break;

            i64 nn = divide(n, q);
            for (i32 e = 2; nn > 0; nn = divide(nn, p), ++e)
                ret += self(self, nn, i + 1, coeff * (f(p, e) - f(p, 1) * f(p, e - 1)));
        }

        return ret;
    };

    return dfs(dfs, N, 0, 1);
}

#endif // LIB_TOTIENT_SUM_HPP
