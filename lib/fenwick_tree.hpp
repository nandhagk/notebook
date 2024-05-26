#ifndef LIB_FENWICK_TREE_HPP
#define LIB_FENWICK_TREE_HPP 1

#include <vector>
#include <cassert>
#include <lib/prelude.hpp>

template<class T>
struct FenwickTree {
public:
	explicit FenwickTree(const i32 n_) : FenwickTree(std::vector<T>(n_)) {}

	explicit FenwickTree(const std::vector<T> &v):
		n(static_cast<i32>(v.size())),
		d(n)
	{
		for (auto i = 0; i < n; ++i) {
			d[i] += v[i];
			if (const auto r = i | (i + 1); r < n) d[r] += d[i];
		}
	}

	void add(i32 p, T x) {
		assert(0 <= p && p < n);

		++p;
		while (p <= n) {
			d[p - 1] += x;
			p += p & -p;
		}
	}

	T sum(i32 r) const {
		assert(r <= n);

		T s = 0;
		while (r > 0) {
			s += d[r - 1];
			r -= r & -r;
		}

		return s;
	}

	T sum(i32 l, i32 r) const {
		assert(0 <= l && l <= r && r <= n);

		return sum(r) - sum(l);
	}

private:
	i32 n;
	std::vector<T> d;
};

#endif // LIB_FENWICK_TREE_HPP
