#ifndef LIB_PRIME_COUNT_HPP
#define LIB_PRIME_COUNT_HPP 1

#include <cmath>
#include <vector>

#include <lib/prelude.hpp>

inline i64 prime_count(const i64 N) {
    if (N <= 1) return 0;
    if (N == 2) return 1;

    i32 v = i32(std::sqrt(N));
    while (i64(v) * v < N) ++v;
    while (i64(v) * v > N) --v;

    i32 s = (v + 1) / 2;
    std::vector<i32> smalls(s);
    for (i32 i = 1; i < s; i++) smalls[i] = i;

    std::vector<i32> roughs(s);
    for (i32 i = 0; i < s; i++) roughs[i] = 2 * i + 1;

    std::vector<i64> larges(s);
    for (i32 i = 0; i < s; i++) larges[i] = (N / (2 * i + 1) - 1) / 2;

    std::vector<bool> skip(v + 1);
    const auto divide = [](i64 n, i64 d) -> i32 { return i32((f64)n / d); };
    const auto half = [](i32 n) -> i32 { return (n - 1) >> 1; };

    i32 pc = 0;
    for (i32 p = 3; p <= v; p += 2) {
        if (!skip[p]) {
            i32 q = p * p;
            if ((i64)q * q > N) break;

            skip[p] = true;
            for (i32 i = q; i <= v; i += 2 * p) skip[i] = true;

            i32 ns = 0;
            for (i32 k = 0; k < s; k++) {
                i32 i = roughs[k];
                if (skip[i]) continue;

                i64 d = (i64)i * p;
                larges[ns] = larges[k] - (d <= v ? larges[smalls[d >> 1] - pc] : smalls[half(divide(N, d))]) + pc;
                roughs[ns++] = i;
            }

            s = ns;
            for (i32 i = half(v), j = ((v / p) - 1) | 1; j >= p; j -= 2) {
                i32 c = smalls[j >> 1] - pc;
                for (i32 e = (j * p) >> 1; i >= e; i--) smalls[i] -= c;
            }

            pc++;
        }
    }

    larges[0] += (i64)(s + 2 * (pc - 1)) * (s - 1) / 2;
    for (i32 k = 1; k < s; k++) larges[0] -= larges[k];
    for (i32 l = 1; l < s; l++) {
        i64 q = roughs[l];
        i64 M = N / q;
        i32 e = smalls[half(M / q)] - pc;
        if (e < l + 1) break;

        i64 t = 0;
        for (i32 k = l + 1; k <= e; k++) t += smalls[half(divide(M, roughs[k]))];

        larges[0] += t - (i64)(e - l) * (pc + l - 1);
    }

    return larges[0] + 1;
}

#endif // LIB_PRIME_COUNT_HPP
