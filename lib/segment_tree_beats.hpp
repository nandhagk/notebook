#ifndef LIB_SEGMENT_TREE_BEATS_HPP
#define LIB_SEGMENT_TREE_BEATS_HPP 1

#include <cassert>
#include <vector>
#include <bit>
#include <lib/prelude.hpp>

template <typename ActedMonoid>
struct segment_tree_beats {
	using AM = ActedMonoid;

	using MX = typename AM::MX;
	using MA = typename AM::MA;

	using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	i32 n, log, size;
	std::vector<X> d;
	std::vector<A> z;

	segment_tree_beats() {}
	segment_tree_beats(i32 m) { 
		build(m); 
	}

	template <typename F>
	segment_tree_beats(i32 m, F f) {
		build(m, f);
	}

	segment_tree_beats(const std::vector<X>& v) {
		build(v); 
	}

	void build(i32 m) {
		build(m, [](i32) -> X { return MX::unit(); });
	}

	void build(const std::vector<X>& v) {
		build(static_cast<i32>(v.size()), [&](i32 i) -> X { return v[i]; });
	}

	template <typename F>
	void build(i32 m, F f) {
		n = m;
		size = std::bit_ceil(static_cast<u32>(n));
		log = lowbit(size);

		d.assign(size << 1, MX::unit());
		z.assign(size, MA::unit());

		for (i32 i = 0; i < n; ++i) d[i + size] = f(i);
		for (i32 i = size - 1; i >= 1; --i) update(i);
	}

	void update(i32 k) { 
		d[k] = MX::op(d[2 * k], d[2 * k + 1]); 
	}

	void set(i32 p, X x) {
		assert(0 <= p && p < n);

		p += size;
		for (i32 i = log; i >= 1; i--) push(p >> i);

		d[p] = x;
		for (i32 i = 1; i <= log; i++) update(p >> i);
	}

	void multiply(i32 p, const X& x) {
		assert(0 <= p && p < n);

		p += size;
		for (i32 i = log; i >= 1; i--) push(p >> i);

		d[p] = MX::op(d[p], x);
		for (i32 i = 1; i <= log; i++) update(p >> i);
	}

	X get(i32 p) {
		assert(0 <= p && p < n);

		p += size;
		for (i32 i = log; i >= 1; i--) push(p >> i);

		return d[p];
	}

	std::vector<X> get_all() {
		for (i32 k = size - 1; k >= 1; --k) push(k);
		return {d.begin() + size, d.begin() + size + n};
	}

	X prod(i32 l, i32 r) {
		assert(0 <= l && l <= r && r <= n);

		if (l == r) return MX::unit();
		l += size, r += size;

		for (i32 i = log; i >= 1; i--) {
			if (((l >> i) << i) != l) push(l >> i);
			if (((r >> i) << i) != r) push((r - 1) >> i);
		}

		X xl = MX::unit(), xr = MX::unit();
		while (l < r) {
			if (l & 1) xl = MX::op(xl, d[l++]);
			if (r & 1) xr = MX::op(d[--r], xr);

			l >>= 1;
			r >>= 1;
		}

		return MX::op(xl, xr);
	}

	X prod_all() { 
		return d[1]; 
	}

	void apply(i32 l, i32 r, A a) {
		assert(0 <= l && l <= r && r <= n);

		if (l == r) return;
		l += size, r += size;

		for (i32 i = log; i >= 1; i--) {
			if (((l >> i) << i) != l) push(l >> i);
			if (((r >> i) << i) != r) push((r - 1) >> i);
		}

		i32 l2 = l, r2 = r;
		while (l < r) {
			if (l & 1) apply_at(l++, a);
			if (r & 1) apply_at(--r, a);

			l >>= 1;
			r >>= 1;
		}

		l = l2, r = r2;
		for (i32 i = 1; i <= log; i++) {
			if (((l >> i) << i) != l) update(l >> i);
			if (((r >> i) << i) != r) update((r - 1) >> i);
		}
	}

private:
	void apply_at(i32 k, A a) {
		const i64 sz = 1 << (log - topbit(k));

		d[k] = AM::act(d[k], a, sz);
		if (k < size) {
			z[k] = MA::op(z[k], a);
			if (d[k].fail) {
				push(k);
				update(k);
			}
		}
	}

	void push(i32 k) {
		if (z[k] == MA::unit()) return;

		apply_at(2 * k, z[k]);
		apply_at(2 * k + 1, z[k]);

		z[k] = MA::unit();
	}
};

#endif // LIB_SEGMENT_TREE_BEATS_HPP
