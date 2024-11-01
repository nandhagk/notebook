#ifndef LIB_TWO_SAT_HPP
#define LIB_TWO_SAT_HPP 1

#include <vector>
#include <cassert>
#include <lib/prelude.hpp>
#include <lib/scc.hpp>

struct two_sat {
	i32 n;
	std::vector<bool> ans;
	std::vector<std::vector<i32>> g;

	two_sat(): two_sat(0) {}
	explicit two_sat(i32 m):
		n{m}, ans(n), g(2 * n) {}

	void add_clause(i32 i, bool p, i32 j, bool q) {
		assert(0 <= i && i < n);
		assert(0 <= j && j < n);

		g[2 * i + (p ? 0 : 1)].push_back(2 * j + (q ? 1 : 0));
		g[2 * j + (q ? 0 : 1)].push_back(2 * i + (p ? 1 : 0));
	}

	bool satisfiable() {
		const auto ids = scc(g);
		for (i32 i = 0; i < n; ++i) {
			if (ids[2 * i] == ids[2 * i + 1]) return false;
			ans[i] = ids[2 * i] < ids[2 * i + 1];
		}

		return true;
	}
};

#endif //  LIB_TWO_SAT_HPP
