#ifndef LIB_CONVOLUTION_2_64_HPP
#define LIB_CONVOLUTION_2_64_HPP 1 

#include <vector>

#include <lib/prelude.hpp>
#include <lib/static_montgomery_modint.hpp>
#include <lib/convolution.hpp>

inline std::vector<u64> convolution_2_64(std::vector<u64> l, std::vector<u64> r) {
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

	static constexpr i64 INV_M1_MOD2 = inv(M1_MOD2, MOD2);
	static constexpr i64 INV_M1M2_MOD3 = inv(M1M2_MOD3, MOD3);
	static constexpr i64 INV_M1M2M3_MOD4 = inv(M1M2M3_MOD4, MOD4);
	static constexpr i64 INV_M1M2M3M4_MOD5 = inv(M1M2M3M4_MOD5, MOD5);
	static constexpr i64 INV_M1M2M3M4M5_MOD6 = inv(M1M2M3M4M5_MOD6, MOD6);

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

#endif // LIB_CONVOLUTION_2_64_HPP
