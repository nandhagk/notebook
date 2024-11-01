#ifndef LIB_DSU_HPP
#define LIB_DSU_HPP 1

#include <algorithm>
#include <cassert>
#include <vector>
#include <lib/prelude.hpp>

struct dsu {
public:
	i32 n, ccs;
	std::vector<i32> d;

	dsu(): dsu(0) {}
	explicit dsu(i32 m) {
		build(m);
	}

	void build(i32 m) {
		n = ccs = m;
		d.assign(n, -1);
	}

	void reset() {
		build(n);
	}

	i32 operator[](i32 x) {
		while (d[x] >= 0) {
			i32 p = d[d[x]];
			if (p < 0) return d[x];

			x = d[x] = p;
		}

		return x;
	}

	i32 size(i32 x) {
		x = (*this)[x];
		return -d[x];
	}

	bool merge(i32 x, i32 y) {
		x = (*this)[x];
		y = (*this)[y];

		if (x == y) return false;
		if (-d[x] < -d[y]) std::swap(x, y);

		d[x] += d[y];
		d[y] = x;
		--ccs;

		return true;
	}
};

#endif // LIB_DSU_HPP
