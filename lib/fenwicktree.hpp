#ifndef LIB_FENWICKTREE_HPP
#define LIB_FENWICKTREE_HPP 1

#include <vector>
#include <cassert>

namespace lib {

template<class T>
struct fenwick_tree {
public:
	explicit fenwick_tree(const int n_) : fenwick_tree(std::vector<T>(n_)) {}
	explicit fenwick_tree(const std::vector<T> &v) : n{static_cast<int>(v.size())} {
		d = std::vector<T>(n);

		for (auto i = 0; i < n; ++i) {
			d[i] += v[i];
			if (const auto r = i | (i + 1); r < n) d[r] += d[i];
		}
	}

	void add(int p, T x) {
		assert(0 <= p && p < n);

		++p;
		while (p <= n) {
			d[p - 1] += x;
			p += p & -p;
		}
	}

	T sum(int r) const {
		assert(r <= n);

		T s = 0;
		while (r > 0) {
			s += d[r - 1];
			r -= r & -r;
		}

		return s;
	}

	T sum(int l, int r) const {
		assert(0 <= l && l <= r && r <= n);

		return sum(r) - sum(l);
	}

private:
	int n;
	std::vector<T> d;
};

} // namespace lib

#endif // LIB_FENWICKTREE_HPP
