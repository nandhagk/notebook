#ifndef LIB_DSU_HPP
#define LIB_DSU_HPP 1

#include <algorithm>
#include <cassert>
#include <vector>
#include <lib/prelude.hpp>

struct DSU {
public:
	DSU() : n(0) {}
	explicit DSU(i32 n_) : n(n_), parent_or_size(n, -1) {}

	i32 merge(i32 a, i32 b) {
		assert(0 <= a && a < n);
		assert(0 <= b && b < n);

		i32 x = leader(a), y = leader(b);

		if (x == y) return x;
		if (-parent_or_size[x] < -parent_or_size[y]) std::swap(x, y);

		parent_or_size[x] += parent_or_size[y];
		parent_or_size[y] = x;

		return x;
	}

	bool same(i32 a, i32 b) {
		assert(0 <= a && a < n);
		assert(0 <= b && b < n);

		return leader(a) == leader(b);
	}

	i32 leader(i32 a) {
		assert(0 <= a && a < n);

		if (parent_or_size[a] < 0) return a;

		return parent_or_size[a] = leader(parent_or_size[a]);
	}

	i32 size(i32 a) {
		assert(0 <= a && a < n);

		return -parent_or_size[leader(a)];
	}

	std::vector<std::vector<i32>> groups() {
		std::vector<i32> leader_buf(n), group_size(n);

		for (i32 i = 0; i < n; i++) {
			leader_buf[i] = leader(i);
			group_size[leader_buf[i]]++;
		}

		std::vector<std::vector<i32>> result(n);

		for (i32 i = 0; i < n; i++) {
			result[i].reserve(group_size[i]);
		}

		for (i32 i = 0; i < n; i++) {
			result[leader_buf[i]].push_back(i);
		}

		result.erase(
			std::remove_if(result.begin(), result.end(),
				[&](const std::vector<i32>& v) { return v.empty(); }),
			result.end());

		return result;
	}

private:
	i32 n;
	std::vector<i32> parent_or_size;
};

#endif // LIB_DSU_HPP
