#ifndef LIB_CONVOLUTION_HPP
#define LIB_CONVOLUTION_HPP 1

#include <array>
#include <vector>
#include <lib/prelude.hpp>
#include <lib/modint.hpp>

template <class mint,
          i32 g = primitive_root<mint::mod()>,
          is_static_modint_t<mint>* = nullptr>
struct fft_info {
	static constexpr i32 rank2 = std::countr_zero(static_cast<u32>(mint::mod() - 1));

	std::array<mint, rank2 + 1> root;   // root[i]^(2^i) == 1
	std::array<mint, rank2 + 1> iroot;  // root[i] * iroot[i] == 1

	std::array<mint, std::max(0, rank2 - 2 + 1)> rate2;
	std::array<mint, std::max(0, rank2 - 2 + 1)> irate2;

	std::array<mint, std::max(0, rank2 - 3 + 1)> rate3;
	std::array<mint, std::max(0, rank2 - 3 + 1)> irate3;

	fft_info() {
		root[rank2] = mint(g).pow((mint::mod() - 1) >> rank2);
		iroot[rank2] = root[rank2].inv();

		for (i32 i = rank2 - 1; i >= 0; i--) {
			root[i] = root[i + 1] * root[i + 1];
			iroot[i] = iroot[i + 1] * iroot[i + 1];
		}

		{
			mint prod = 1, iprod = 1;
			for (i32 i = 0; i <= rank2 - 2; i++) {
				rate2[i] = root[i + 2] * prod;
				irate2[i] = iroot[i + 2] * iprod;
				prod *= iroot[i + 2];
				iprod *= root[i + 2];
			}
		}

		{
			mint prod = 1, iprod = 1;
			for (i32 i = 0; i <= rank2 - 3; i++) {
				rate3[i] = root[i + 3] * prod;
				irate3[i] = iroot[i + 3] * iprod;
				prod *= iroot[i + 3];
				iprod *= root[i + 3];
			}
		}
	}
};

template <class mint, is_static_modint_t<mint>* = nullptr>
void butterfly(std::vector<mint>& a) {
	i32 n = i32(a.size());
	i32 h = std::countr_zero((u32)n);

	static const fft_info<mint> info;

	i32 len = 0;  // a[i, i+(n>>len), i+2*(n>>len), ..] is transformed
	while (len < h) {
		if (h - len == 1) {
			i32 p = 1 << (h - len - 1);
			mint rot = 1;

			for (i32 s = 0; s < (1 << len); s++) {
				i32 offset = s << (h - len);

				for (i32 i = 0; i < p; i++) {
					auto l = a[i + offset];
					auto r = a[i + offset + p] * rot;
					a[i + offset] = l + r;
					a[i + offset + p] = l - r;
				}

				if (s + 1 != (1 << len)) rot *= info.rate2[std::countr_zero(~(u32)(s))];
			}

			len++;
		} else {
			// 4-base
			i32 p = 1 << (h - len - 2);
			mint rot = 1, imag = info.root[2];

			for (i32 s = 0; s < (1 << len); s++) {
				mint rot2 = rot * rot;
				mint rot3 = rot2 * rot;
				i32 offset = s << (h - len);

				for (i32 i = 0; i < p; i++) {
					auto mod2 = 1ULL * mint::mod() * mint::mod();

					auto a0 = 1ULL * a[i + offset].val();
					auto a1 = 1ULL * a[i + offset + p].val() * rot.val();
					auto a2 = 1ULL * a[i + offset + 2 * p].val() * rot2.val();
					auto a3 = 1ULL * a[i + offset + 3 * p].val() * rot3.val();

					auto a1na3imag = 1ULL * mint(a1 + mod2 - a3).val() * imag.val();
					auto na2 = mod2 - a2;

					a[i + offset] = a0 + a2 + a1 + a3;
					a[i + offset + 1 * p] = a0 + a2 + (2 * mod2 - (a1 + a3));
					a[i + offset + 2 * p] = a0 + na2 + a1na3imag;
					a[i + offset + 3 * p] = a0 + na2 + (mod2 - a1na3imag);
				}

				if (s + 1 != (1 << len)) rot *= info.rate3[std::countr_zero(~(u32)(s))];
			}

			len += 2;
		}
	}
}

template <class mint, is_static_modint_t<mint>* = nullptr>
void butterfly_inv(std::vector<mint>& a) {
	i32 n = i32(a.size());
	i32 h = std::countr_zero((u32)n);

	static const fft_info<mint> info;

	i32 len = h;  // a[i, i+(n>>len), i+2*(n>>len), ..] is transformed
	while (len) {
		if (len == 1) {
			i32 p = 1 << (h - len);
			mint irot = 1;

			for (i32 s = 0; s < (1 << (len - 1)); s++) {
				i32 offset = s << (h - len + 1);

				for (i32 i = 0; i < p; i++) {
					auto l = a[i + offset];
					auto r = a[i + offset + p];
					a[i + offset] = l + r;
					a[i + offset + p] = (u64)(mint::mod() + l.val() - r.val()) * irot.val();
				}

				if (s + 1 != (1 << (len - 1))) irot *= info.irate2[std::countr_zero(~(u32)(s))];
			}

			len--;
		} else {
			// 4-base
			i32 p = 1 << (h - len);
			mint irot = 1, iimag = info.iroot[2];

			for (i32 s = 0; s < (1 << (len - 2)); s++) {
				mint irot2 = irot * irot;
				mint irot3 = irot2 * irot;
				i32 offset = s << (h - len + 2);

				for (i32 i = 0; i < p; i++) {
					auto a0 = 1ULL * a[i + offset + 0 * p].val();
					auto a1 = 1ULL * a[i + offset + 1 * p].val();
					auto a2 = 1ULL * a[i + offset + 2 * p].val();
					auto a3 = 1ULL * a[i + offset + 3 * p].val();

					auto a2na3iimag = 1ULL * mint((mint::mod() + a2 - a3) * iimag.val()).val();

					a[i + offset] = a0 + a1 + a2 + a3;
					a[i + offset + 1 * p] = (a0 + (mint::mod() - a1) + a2na3iimag) * irot.val();
					a[i + offset + 2 * p] = (a0 + a1 + (mint::mod() - a2) + (mint::mod() - a3)) * irot2.val();
					a[i + offset + 3 * p] = (a0 + (mint::mod() - a1) + (mint::mod() - a2na3iimag)) * irot3.val();
				}

				if (s + 1 != (1 << (len - 2))) irot *= info.irate3[std::countr_zero(~(u32)(s))];
			}

			len -= 2;
		}
	}
}

template <class mint, is_static_modint_t<mint>* = nullptr>
std::vector<mint> convolution_naive(const std::vector<mint>& a,
	    const std::vector<mint>& b) {
	i32 n = i32(a.size()), m = i32(b.size());
	std::vector<mint> ans(n + m - 1);

	if (n < m) {
		for (i32 j = 0; j < m; j++) {
			for (i32 i = 0; i < n; i++) {
				ans[i + j] += a[i] * b[j];
			}
		}
	} else {
		for (i32 i = 0; i < n; i++) {
			for (i32 j = 0; j < m; j++) {
				ans[i + j] += a[i] * b[j];
			}
		}
	}

	return ans;
}

template <class mint, is_static_modint_t<mint>* = nullptr>
std::vector<mint> convolution_fft(std::vector<mint> a, std::vector<mint> b) {
	i32 n = i32(a.size()), m = i32(b.size());
	i32 z = (i32)std::bit_ceil((u32)(n + m - 1));

	a.resize(z);
	butterfly(a);
	b.resize(z);
	butterfly(b);

	for (i32 i = 0; i < z; i++) {
		a[i] *= b[i];
	}

	butterfly_inv(a);
	a.resize(n + m - 1);

	mint iz = mint(z).inv();
	for (i32 i = 0; i < n + m - 1; i++) a[i] *= iz;

	return a;
}

template <class mint, is_static_modint_t<mint>* = nullptr>
std::vector<mint> convolution(std::vector<mint>&& a, std::vector<mint>&& b) {
	i32 n = i32(a.size()), m = i32(b.size());
	if (!n || !m) return {};

	i32 z = (i32)std::bit_ceil((u32)(n + m - 1));
	assert((mint::mod() - 1) % z == 0);

	if (std::min(n, m) <= 60) return convolution_naive(std::move(a), std::move(b));
	return convolution_fft(std::move(a), std::move(b));
}

template <class mint, is_static_modint_t<mint>* = nullptr>
std::vector<mint> convolution(const std::vector<mint>& a, const std::vector<mint>& b) {
	i32 n = i32(a.size()), m = i32(b.size());
	if (!n || !m) return {};

	i32 z = (i32)std::bit_ceil((u32)(n + m - 1));
	assert((mint::mod() - 1) % z == 0);

	if (std::min(n, m) <= 60) return convolution_naive(a, b);
	return convolution_fft(a, b);
}

template <u32 mod = 998244353,
	  class T,
	  std::enable_if_t<is_integral<T>::value>* = nullptr>
std::vector<T> convolution(const std::vector<T>& a, const std::vector<T>& b) {
	i32 n = i32(a.size()), m = i32(b.size());
	if (!n || !m) return {};

	using mint = static_modint<mod>;

	i32 z = (i32)std::bit_ceil((u32)(n + m - 1));
	assert((mint::mod() - 1) % z == 0);

	std::vector<mint> a2(n), b2(m);
	for (i32 i = 0; i < n; i++) {
		a2[i] = mint(a[i]);
	}

	for (i32 i = 0; i < m; i++) {
		b2[i] = mint(b[i]);
	}

	auto c2 = convolution(std::move(a2), std::move(b2));
	std::vector<T> c(n + m - 1);
	for (i32 i = 0; i < n + m - 1; i++) {
		c[i] = c2[i].val();
	}

	return c;
}

inline std::vector<i64> convolution_ll(const std::vector<i64>& a, const std::vector<i64>& b) {
	i32 n = i32(a.size()), m = i32(b.size());
	if (!n || !m) return {};

	static constexpr u64 MOD1 = 754974721;  // 2^24
	static constexpr u64 MOD2 = 167772161;  // 2^25
	static constexpr u64 MOD3 = 469762049;  // 2^26
	static constexpr u64 M2M3 = MOD2 * MOD3;
	static constexpr u64 M1M3 = MOD1 * MOD3;
	static constexpr u64 M1M2 = MOD1 * MOD2;
	static constexpr u64 M1M2M3 = MOD1 * MOD2 * MOD3;

	static constexpr u64 i1 =
	inv_gcd(MOD2 * MOD3, MOD1).second;
	static constexpr u64 i2 =
	inv_gcd(MOD1 * MOD3, MOD2).second;
	static constexpr u64 i3 =
	inv_gcd(MOD1 * MOD2, MOD3).second;

	static constexpr int MAX_AB_BIT = 24;
	static_assert(MOD1 % (1ull << MAX_AB_BIT) == 1, "MOD1 isn't enough to support an array length of 2^24.");
	static_assert(MOD2 % (1ull << MAX_AB_BIT) == 1, "MOD2 isn't enough to support an array length of 2^24.");
	static_assert(MOD3 % (1ull << MAX_AB_BIT) == 1, "MOD3 isn't enough to support an array length of 2^24.");
	assert(n + m - 1 <= (1 << MAX_AB_BIT));

	auto c1 = convolution<MOD1>(a, b);
	auto c2 = convolution<MOD2>(a, b);
	auto c3 = convolution<MOD3>(a, b);

	std::vector<i64> c(n + m - 1);
	for (i32 i = 0; i < n + m - 1; i++) {
		u64 x = 0;
		x += (c1[i] * i1) % MOD1 * M2M3;
		x += (c2[i] * i2) % MOD2 * M1M3;
		x += (c3[i] * i3) % MOD3 * M1M2;
		// B = 2^63, -B <= x, r(real value) < B
		// (x, x - M, x - 2M, or x - 3M) = r (mod 2B)
		// r = c1[i] (mod MOD1)
		// focus on MOD1
		// r = x, x - M', x - 2M', x - 3M' (M' = M % 2^64) (mod 2B)
		// r = x,
		//     x - M' + (0 or 2B),
		//     x - 2M' + (0, 2B or 4B),
		//     x - 3M' + (0, 2B, 4B or 6B) (without mod!)
		// (r - x) = 0, (0)
		//           - M' + (0 or 2B), (1)
		//           -2M' + (0 or 2B or 4B), (2)
		//           -3M' + (0 or 2B or 4B or 6B) (3) (mod MOD1)
		// we checked that
		//   ((1) mod MOD1) mod 5 = 2
		//   ((2) mod MOD1) mod 5 = 3
		//   ((3) mod MOD1) mod 5 = 4
		i64 diff = c1[i] - safe_mod((i64)(x), (i64)(MOD1));
		if (diff < 0) diff += MOD1;

		static constexpr u64 offset[5] = {0, 0, M1M2M3, 2 * M1M2M3, 3 * M1M2M3};
		x -= offset[diff % 5];
		c[i] = x;
	}

	return c;
}

#endif // LIB_CONVOLUTION_HPP
