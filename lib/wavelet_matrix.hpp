#ifndef LIB_WAVELET_MATRIX_HPP
#define LIB_WAVELET_MATRIX_HPP 1

#include <limits>
#include <vector>
#include <algorithm>
#include <cassert>

#include <lib/prelude.hpp>
#include <lib/modint.hpp>

struct bit_vector {
	static constexpr usize wordsize = std::numeric_limits<usize>::digits;

	struct node_type {
		usize bit;
		usize sum;

		node_type() : bit(0), sum(0) {}
	};

	std::vector<node_type> v;

	bit_vector() = default;
	explicit bit_vector(const std::vector<bool> &a) : v(a.size() / wordsize + 1) {
		const usize s = a.size();
		const usize t = v.size();

		for (usize i = 0; i != s; i += 1) v[i / wordsize].bit |= static_cast<usize>(a[i] ? 1 : 0) << i % wordsize;
		for (usize i = 1; i != t; i += 1) v[i].sum = v[i - 1].sum + popcnt(v[i - 1].bit);
	}

	usize rank0(const usize index) const { return index - rank1(index); }
	usize rank1(const usize index) const {
		return v[index / wordsize].sum + popcnt(v[index / wordsize].bit & ~(~static_cast<usize>(0) << index % wordsize));
	}
};

template <class T, is_unsigned_int_t<T>* = nullptr> 
struct wavelet_matrix {
	static bool test(const T x, const usize k) {
		return (x & static_cast<T>(1) << k) != 0;
	}

	static void set(T &x, const usize k) {
		x |= static_cast<T>(1) << k;
	}

	usize n;
	std::vector<bit_vector> mat;

	wavelet_matrix() = default;
	explicit wavelet_matrix(const usize bit_length, std::vector<T> a) : 
		n(a.size()), mat(bit_length, bit_vector()) {
		for (usize p = bit_length; p != 0;) {
			--p;

			std::vector<bool> t(n);
			for (usize i = 0; i != n; i += 1) t[i] = test(a[i], p);
			mat[p] = bit_vector(t);

			std::vector<T> v0, v1;
			for (const usize e : a) (test(e, p) ? v1 : v0).push_back(e);

			const auto it = std::copy(v0.cbegin(), v0.cend(), a.begin());
			std::copy(v1.cbegin(), v1.cend(), it);
		}
	}

	T quantile(usize l, usize r, usize k) const {
		assert(l <= r && r <= n);
		assert(k < r - l);

		T ret = 0;
		for (usize p = mat.size(); p != 0;) {
			p -= 1;
			const bit_vector &v = mat[p];
			const usize z = v.rank0(r) - v.rank0(l);
			if (k < z) {
				l = v.rank0(l);
				r = v.rank0(r);
			} else {
				set(ret, p);
				k -= z;
				const usize b = v.rank0(n);
				l = b + v.rank1(l);
				r = b + v.rank1(r);
			}
		}

		return ret;
	}

	usize count(usize l, usize r, T ub) const {
		assert(l <= r && r <= n);

		if (ub >= T(1) << mat.size()) return r - l;

		usize ret = 0;
		for (usize p = mat.size(); p != 0;) {
                        p -= 1;
			const bit_vector &v = mat[p];
			const usize z = v.rank0(r) - v.rank0(l);
			if (~ub >> p & 1) {
				l = v.rank0(l);
				r = v.rank0(r);
			} else {
				ret += z;
				const usize b = v.rank0(n);
				l = b + v.rank1(l);
				r = b + v.rank1(r);
			}
		}

		return ret;
	}

	usize count(usize l, usize r, T lb, T ub) const {
		return count(l, r, ub) - count(l, r, lb);
	}
};

#endif // LIB_WAVELET_MATRIX_HPP

