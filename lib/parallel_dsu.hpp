#ifndef LIB_PARALLEL_DSU_HPP
#define LIB_PARALLEL_DSU_HPP 1

#include <vector>
#include <lib/prelude.hpp>
#include <lib/dsu.hpp>

// same(L1,R1,L2,R2) みたいなことは出来ないと思う(必要なら rolling hash かな)
struct parallel_dsu {
	i32 n, log, ccs;
	std::vector<dsu> ufs;

	parallel_dsu(i32 m) : n(m), ccs(m) {
		log = 1;
		while ((1 << log) < m) ++log;

		ufs.resize(log);
		for (i32 i = 0; i < log; ++i) {
			ufs[i].build(n - (1 << i) + 1);
		}
	}

	i32 operator[](i32 i) { 
		return ufs[0][i]; 
	}

	// f(r1,r2) : 成分 r2 を r1 にマージ
	template <typename F>
	void merge(i32 L1, i32 R1, i32 L2, i32 R2, F f) {
		assert(R1 - L1 == R2 - L2);

		i32 m = R1 - L1;
		if (m == 0) return;
		if (m == 1) return merge_inner(0, L1, L2, f);

		i32 k = topbit(m - 1);
		merge_inner(k, L1, L2, f);
		merge_inner(k, R1 - (1 << k), R2 - (1 << k), f);
	}

	// f(r1,r2) : 成分 r2 を r1 にマージ
	template <typename F>
	void merge(i32 i, i32 j, F f) {
		merge_inner(0, i, j, f);
	}

	void merge(i32 L1, i32 R1, i32 L2, i32 R2) {
		merge(L1, R1, L2, R2, [&](i32, i32) -> void {});
	}

	void merge(i32 i, i32 j) {
		merge(i, j, [&](i32, i32) -> void {});
	}

	template <typename F>
	void merge_inner(i32 k, i32 L1, i32 L2, const F& f) {
		if (k == 0) {
			i32 a = ufs[0][L1], b = ufs[0][L2];
			if (a == b) return;

			ufs[0].merge(a, b);
			i32 c = ufs[0][a];
			--ccs;
			return f(c, a ^ b ^ c);
		}

		if (!ufs[k].merge(L1, L2)) return;
		merge_inner(k - 1, L1, L2, f);
		merge_inner(k - 1, L1 + (1 << (k - 1)), L2 + (1 << (k - 1)), f);
	}
};


#endif // LIB_PARALLEL_DSU_HPP 1
