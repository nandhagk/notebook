#ifndef LIB_LAZY_SEGMENT_TREE_HPP
#define LIB_LAZY_SEGMENT_TREE_HPP 1

#include <cassert>
#include <vector>
#include <bit>
#include <lib/prelude.hpp>

template <typename ActedMonoid>
struct LazySegmentTree {
	using AM = ActedMonoid;

	using MX = typename AM::MX;
	using MA = typename AM::MA;

	using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	i32 n, log, size;
	std::vector<X> d;
	std::vector<A> z;

	LazySegmentTree() {}
	LazySegmentTree(i32 m) { 
		Build(m); 
	}

	template <typename F>
	LazySegmentTree(i32 m, F f) {
		Build(m, f);
	}

	LazySegmentTree(const std::vector<X>& v) {
		Build(v); 
	}

	void Build(i32 m) {
		Build(m, [](i32) -> X { return MX::Unit(); });
	}

	void Build(const std::vector<X>& v) {
		Build(static_cast<i32>(v.size()), [&](i32 i) -> X { return v[i]; });
	}

	template <typename F>
	void Build(i32 m, F f) {
		n = m;
		size = std::bit_ceil(static_cast<u32>(n));
		log = lowbit(size);

		d.assign(size << 1, MX::Unit());
		z.assign(size, MA::Unit());

		for (i32 i = 0; i < n; ++i) d[i + size] = f(i);
		for (i32 i = size - 1; i >= 1; --i) Update(i);
	}

	void Update(i32 k) { 
		d[k] = MX::Op(d[2 * k], d[2 * k + 1]); 
	}

	void Set(i32 p, X x) {
		assert(0 <= p && p < n);

		p += size;
		for (i32 i = log; i >= 1; i--) Push(p >> i);

		d[p] = x;
		for (i32 i = 1; i <= log; i++) Update(p >> i);
	}

	void Multiply(i32 p, const X& x) {
		assert(0 <= p && p < n);

		p += size;
		for (i32 i = log; i >= 1; i--) Push(p >> i);

		d[p] = MX::Op(d[p], x);
		for (i32 i = 1; i <= log; i++) Update(p >> i);
	}

	X Get(i32 p) {
		assert(0 <= p && p < n);

		p += size;
		for (i32 i = log; i >= 1; i--) Push(p >> i);

		return d[p];
	}

	std::vector<X> GetAll() {
		for (i32 k = size - 1; k >= 1; --k) Push(k);
		return {d.begin() + size, d.begin() + size + n};
	}

	X Prod(i32 l, i32 r) {
		assert(0 <= l && l <= r && r <= n);

		if (l == r) return MX::Unit();
		l += size, r += size;

		for (i32 i = log; i >= 1; i--) {
			if (((l >> i) << i) != l) Push(l >> i);
			if (((r >> i) << i) != r) Push((r - 1) >> i);
		}

		X xl = MX::Unit(), xr = MX::Unit();
		while (l < r) {
			if (l & 1) xl = MX::Op(xl, d[l++]);
			if (r & 1) xr = MX::Op(d[--r], xr);

			l >>= 1;
			r >>= 1;
		}

		return MX::Op(xl, xr);
	}

	X ProdAll() { 
		return d[1]; 
	}

	void Apply(i32 l, i32 r, A a) {
		assert(0 <= l && l <= r && r <= n);

		if (l == r) return;
		l += size, r += size;

		for (i32 i = log; i >= 1; i--) {
			if (((l >> i) << i) != l) Push(l >> i);
			if (((r >> i) << i) != r) Push((r - 1) >> i);
		}

		i32 l2 = l, r2 = r;
		while (l < r) {
			if (l & 1) ApplyAt(l++, a);
			if (r & 1) ApplyAt(--r, a);

			l >>= 1;
			r >>= 1;
		}

		l = l2, r = r2;
		for (i32 i = 1; i <= log; i++) {
			if (((l >> i) << i) != l) Update(l >> i);
			if (((r >> i) << i) != r) Update((r - 1) >> i);
		}
	}

	template <typename F>
	i32 MaxRight(const F f, i32 l) {
		assert(0 <= l && l <= n);
		assert(f(MX::Unit()));

		if (l == n) return n;

		l += size;
		for (i32 i = log; i >= 1; i--) Push(l >> i);

		X sm = MX::Unit();
		do {
			while (l % 2 == 0) l >>= 1;
			if (!f(MX::Op(sm, d[l]))) {
				while (l < size) {
					Push(l);
					l = (2 * l);
					if (f(MX::Op(sm, d[l]))) { 
						sm = MX::Op(sm, d[l++]); 
					}
				}

				return l - size;
			}
			sm = MX::Op(sm, d[l++]);
		} while ((l & -l) != l);

		return n;
	}

	template <typename F>
	i32 MinLeft(const F f, i32 r) {
		assert(0 <= r && r <= n);
		assert(f(MX::Unit()));

		if (r == 0) return 0;

		r += size;
		for (i32 i = log; i >= 1; i--) Push((r - 1) >> i);

		X sm = MX::Unit();
		do {
			r--;
			while (r > 1 && (r % 2)) r >>= 1;

			if (!f(MX::Op(d[r], sm))) {
				while (r < size) {
					Push(r);
					r = (2 * r + 1);
					if (f(MX::Op(d[r], sm))) { 
						sm = MX::Op(d[r--], sm); 
					}
				}

				return r + 1 - size;
			}
			sm = MX::Op(d[r], sm);
		} while ((r & -r) != r);

		return 0;
	}

private:
	void ApplyAt(i32 k, A a) {
		const i64 sz = 1 << (log - topbit(k));

		d[k] = AM::Act(d[k], a, sz);
		if (k < size) z[k] = MA::Op(z[k], a);
	}

	void Push(i32 k) {
		if (z[k] == MA::Unit()) return;

		ApplyAt(2 * k, z[k]);
		ApplyAt(2 * k + 1, z[k]);

		z[k] = MA::Unit();
	}
};

#endif // LIB_LAZY_SEGMENT_TREE_HPP
