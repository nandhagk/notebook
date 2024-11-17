#ifndef LIB_CONVOLUTION_HPP
#define LIB_CONVOLUTION_HPP 1

#include <vector>
#include <array>
#include <cassert>

#include <lib/prelude.hpp>
#include <lib/static_montgomery_modint.hpp>

template <typename Z>
void ntt(std::vector<Z> &a, bool inv) {
	static std::array<Z, 30> dw{}, idw{};
	if (dw[0] == 0) {
		Z root = 2;
		while (root.pow((Z::mod() - 1) / 2) == 1) root += 1;

		for (i32 i = 0; i < 30; ++i) {
			dw[i] = -root.pow((Z::mod() - 1) >> (i + 2));
			idw[i] = dw[i].inv();
		}
	}

	const i32 n = static_cast<i32>(a.size());
	assert((n & (n - 1)) == 0);

	if (!inv) {
		for (i32 m = n; m >>= 1;) {
			Z w = 1;
			for (i32 s{}, k{}; s < n; s += 2 * m) {
				for (i32 i = s, j = s + m; i < s + m; ++i, ++j) {
					Z x = a[i], y = a[j] * w;
					a[i] = x + y;
					a[j] = x - y;
				}

				w *= dw[lowbit(++k)];
			}
		}
	} else {
		for (i32 m = 1; m < n; m <<= 1) {
			Z w = 1;
			for (i32 s{}, k{}; s < n; s += 2 * m) {
				for (i32 i = s, j = s + m; i < s + m; ++i, ++j) {
					Z x = a[i], y = a[j];
					a[i] = x + y;
					a[j] = (x - y) * w;
				}

				w *= idw[lowbit(++k)];
			}
		}

		Z r = Z(n).inv();
		for (auto &&e : a) e *= r;
	}
}

template <typename Z>
std::vector<Z> convolution(std::vector<Z> l, std::vector<Z> r) {
	if (l.empty() || r.empty()) return {};

	const i32 n = static_cast<i32>(l.size());
	const i32 m = static_cast<i32>(r.size());
	const i32 sz = 1 << std::__lg(2 * (n + m - 1) - 1);

	l.resize(sz);
	ntt(l, false);

	r.resize(sz);
	ntt(r, false);

	for (i32 i = 0; i < sz; ++i) l[i] *= r[i];

	ntt(l, true);
	l.resize(n + m - 1);

	return l;
}

template <typename Z>
std::vector<Z> arbitrary_convolution(std::vector<Z> l, std::vector<Z> r) {
	static constexpr i64 MOD1 = 754974721;  // 2^24
        static constexpr i64 MOD2 = 167772161;  // 2^25
        static constexpr i64 MOD3 = 469762049;  // 2^26
	
        static constexpr i64 M1M2 = MOD1 * MOD2;
        static constexpr i64 INV_M1_MOD2 = inv_gcd(MOD1, MOD2).second;
        static constexpr i64 INV_M1M2_MOD3 = inv_gcd(M1M2, MOD3).second;

	using Z1 = static_montgomery_modint_32<MOD1>;
	using Z2 = static_montgomery_modint_32<MOD2>;
	using Z3 = static_montgomery_modint_32<MOD3>;

	const i32 n = static_cast<i32>(l.size());
	const i32 m = static_cast<i32>(r.size());

        std::vector<u32> a2(n), b2(m);
        for (i32 i = 0; i < n; ++i) a2[i] = l[i].val();
        for (i32 i = 0; i < m; ++i) b2[i] = r[i].val();

        const auto c1 = convolution(std::vector<Z1>(a2.begin(), a2.end()), std::vector<Z1>(b2.begin(), b2.end()));
        const auto c2 = convolution(std::vector<Z2>(a2.begin(), a2.end()), std::vector<Z2>(b2.begin(), b2.end()));
        const auto c3 = convolution(std::vector<Z3>(a2.begin(), a2.end()), std::vector<Z3>(b2.begin(), b2.end()));
        const i64 m1m2 = Z(M1M2).val();

        std::vector<Z> c(n + m - 1);
        for (i32 i = 0; i < n + m - 1; ++i) {
		i64 x1 = c1[i].val();
		i64 y1 = c2[i].val();
		i64 z1 = c3[i].val();

		i64 x2 = (Z2(y1 - x1) * INV_M1_MOD2).val();
		i64 x3 = (Z3(z1 - x1 - x2 * MOD1) * INV_M1M2_MOD3).val();

		c[i] = x1 + x2 * MOD1 + x3 * m1m2;
        }

        return c;
}

inline std::vector<u64> convolution_64(std::vector<u64> l, std::vector<u64> r) {
	const i32 n = static_cast<i32>(l.size());
	const i32 m = static_cast<i32>(r.size());

	static constexpr i64 MOD1 = 754974721;  // 2^24
	static constexpr i64 MOD2 = 167772161;  // 2^25
	static constexpr i64 MOD3 = 469762049;  // 2^26
	static constexpr i64 MOD4 = 377487361;  // 2^23
	static constexpr i64 MOD5 = 595591169;  // 2^23
	static constexpr i64 MOD6 = 645922817;  // 2^23

	static constexpr i64 M1_MOD2 = MOD1 % MOD2;
	static constexpr i64 M1_MOD3 = MOD1 % MOD3;
	static constexpr i64 M1_MOD4 = MOD1 % MOD4;
	static constexpr i64 M1_MOD5 = MOD1 % MOD5;
	static constexpr i64 M1_MOD6 = MOD1 % MOD6;
	static constexpr i64 M1M2_MOD3 = M1_MOD3 * MOD2 % MOD3;
	static constexpr i64 M1M2_MOD4 = M1_MOD4 * MOD2 % MOD4;
	static constexpr i64 M1M2_MOD5 = M1_MOD5 * MOD2 % MOD5;
	static constexpr i64 M1M2_MOD6 = M1_MOD6 * MOD2 % MOD6;
	static constexpr i64 M1M2M3_MOD4 = M1M2_MOD4 * MOD3 % MOD4;
	static constexpr i64 M1M2M3_MOD5 = M1M2_MOD5 * MOD3 % MOD5;
	static constexpr i64 M1M2M3_MOD6 = M1M2_MOD6 * MOD3 % MOD6;
	static constexpr i64 M1M2M3M4_MOD5 = M1M2M3_MOD5 * MOD4 % MOD5;
	static constexpr i64 M1M2M3M4_MOD6 = M1M2M3_MOD6 * MOD4 % MOD6;
	static constexpr i64 M1M2M3M4M5_MOD6 = M1M2M3M4_MOD6 * MOD5 % MOD6;

	static constexpr i64 INV_M1_MOD2 = inv_gcd(M1_MOD2, MOD2).second;
	static constexpr i64 INV_M1M2_MOD3 = inv_gcd(M1M2_MOD3, MOD3).second;
	static constexpr i64 INV_M1M2M3_MOD4 = inv_gcd(M1M2M3_MOD4, MOD4).second;
	static constexpr i64 INV_M1M2M3M4_MOD5 = inv_gcd(M1M2M3M4_MOD5, MOD5).second;
	static constexpr i64 INV_M1M2M3M4M5_MOD6 = inv_gcd(M1M2M3M4M5_MOD6, MOD6).second;

	static constexpr u64 M1 = MOD1;
	static constexpr u64 M1M2 = M1 * MOD2;
	static constexpr u64 M1M2M3 = M1M2 * MOD3;
	static constexpr u64 M1M2M3M4 = M1M2M3 * MOD4;
	static constexpr u64 M1M2M3M4M5 = M1M2M3M4 * MOD5;

	using Z1 = static_montgomery_modint_32<MOD1>;
	using Z2 = static_montgomery_modint_32<MOD2>;
	using Z3 = static_montgomery_modint_32<MOD3>;
	using Z4 = static_montgomery_modint_32<MOD4>;
	using Z5 = static_montgomery_modint_32<MOD5>;
	using Z6 = static_montgomery_modint_32<MOD6>;

	const auto c1 = convolution(std::vector<Z1>(l.begin(), l.end()), std::vector<Z1>(r.begin(), r.end()));
	const auto c2 = convolution(std::vector<Z2>(l.begin(), l.end()), std::vector<Z2>(r.begin(), r.end()));
	const auto c3 = convolution(std::vector<Z3>(l.begin(), l.end()), std::vector<Z3>(r.begin(), r.end()));
	const auto c4 = convolution(std::vector<Z4>(l.begin(), l.end()), std::vector<Z4>(r.begin(), r.end()));
	const auto c5 = convolution(std::vector<Z5>(l.begin(), l.end()), std::vector<Z5>(r.begin(), r.end()));
	const auto c6 = convolution(std::vector<Z6>(l.begin(), l.end()), std::vector<Z6>(r.begin(), r.end()));

	std::vector<u64> c(n + m - 1);
	for (int i = 0; i < n + m - 1; ++i) {
		// Garner's Algorithm
		i64 x1 = c1[i].val();
		i64 y1 = c2[i].val();
		i64 z1 = c3[i].val();
		i64 w1 = c4[i].val();
		i64 s1 = c5[i].val();
		i64 t1 = c6[i].val();

		i64 x2 = (Z2(y1 - x1) * INV_M1_MOD2).val();
		i64 x3 = (Z3(z1 - x1 - x2 * M1_MOD3) * INV_M1M2_MOD3).val();
		i64 x4 = (Z4(w1 - x1 - x2 * M1_MOD4 - x3 * M1M2_MOD4) * INV_M1M2M3_MOD4).val();
		i64 x5 = (Z5(s1 - x1 - x2 * M1_MOD5 - x3 * M1M2_MOD5 - x4 * M1M2M3_MOD5) * INV_M1M2M3M4_MOD5).val();
		i64 x6 = (Z6(t1 - x1 - x2 * M1_MOD6 - x3 * M1M2_MOD6 - x4 * M1M2M3_MOD6 - x5 * M1M2M3M4_MOD6) * INV_M1M2M3M4M5_MOD6).val();

		c[i] = x1 + x2 * M1 + x3 * M1M2 + x4 * M1M2M3 + x5 * M1M2M3M4 + x6 * M1M2M3M4M5;
	}

	return c;
}

#endif // LIB_CONVOLUTION_HPP
