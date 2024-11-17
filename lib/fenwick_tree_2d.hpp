#ifndef LIB_FENWICK_TREE_2D_HPP
#define LIB_FENWICK_TREE_2D_HPP 1

#include <vector>
#include <cassert>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_abelian_group_t<Monoid>* = nullptr>
struct fenwick_tree_2d {
	using MX = Monoid;
	using X = typename MX::ValueT;

	i32 H, W;
	std::vector<X> d;

	fenwick_tree_2d() {}

	fenwick_tree_2d(i32 h, i32 w) {
		build(h, w);
	}

	explicit fenwick_tree_2d(const std::vector<std::vector<X>> &v) {
		build(v);
	}

	template <typename F>
	fenwick_tree_2d(i32 h, i32 w, F f) {
		build(h, w, f);
	}

	void build(i32 h, i32 w) {
		build(h, w, [](i32, i32) -> X { return MX::unit(); });
	}

	void build(const std::vector<std::vector<X>> &v) {
		const i32 h = static_cat<i32>(v.size());
		const i32 w = static_cat<i32>(v[0].size());

		build(h, w, [&](i32 i, i32 j) -> X { return v[i][j]; });
	}

	template <typename F>
	void build(i32 h, i32 w, F f) {
		H = h;
		W = w;

		d.assign(H * W, MX::unit());
		for (i32 i = 0; i < H; ++i) {
			for (i32 j = 0; j < W; ++j) {
				d[idx(i + 1, j + 1)] = f(i, j);
			}
		}

		for (i32 i = 1; i <= H; ++i) {
			for (i32 j = 1; j <= W; ++j) {
				const i32 k = j + (j & -j);
				if (k <= W) d[idx(i, k)] = MX::op(d[idx(i, k)], d[idx(i, j)]);
			}
		}

		for (i32 i = 1; i <= H; ++i) {
			for (i32 j = 1; j <= W; ++j) {
				const i32 k = i + (i & -i);
				if (k <= H) d[idx(k, j)] = MX::op(d[idx(k, j)], d[idx(i, j)]);
			}
		}
	}

	void multiply(i32 x, i32 y, X v) {
		assert(0 <= x && x < H);
		assert(0 <= y && y < W);

		for (++x; x <= H; x += (x & -x)) multiplyx(x, y, v);
	}

	void set(i32 x, i32 y, X v) {
		assert(0 <= x && x < H);
		assert(0 <= y && y < W);

		multiply(x, y, MX::op(MX::inv(get(x, y)), v));
	}

	X prod(i32 xl, i32 xr, i32 yl, i32 yr) const {
		assert(0 <= xl && xl <= xr && xr <= H);
		assert(0 <= yl && yl <= yr && yr <= W);

		X vl = MX::unit();
		X vr = MX::unit();

		for (; xl < xr; xr -= (xr & -xr)) vr = MX::op(vr, prodx(xr, yl, yr));
		for (; xr < xl; xl -= (xl & -xl)) vl = MX::op(vl, prodx(xl, yl, yr));

		return MX::op(MX::inv(vl), vr);
	}

	X prod(i32 xr, i32 yr) const {
		assert(0 <= xr && xr <= H);
		assert(0 <= yr && yr <= W);

		X r = MX::unit();
		for (; xr; xr -= (xr & -xr)) r = MX::op(r, prodx(xr, yr));
		return r;
	}

	X get(i32 x, i32 y) const {
		assert(0 <= x && x < H);
		assert(0 <= y && y < W);

		return prod(x, x + 1, y, y + 1);
	}

private:
	inline i32 idx(i32 i, i32 j) const {
		return (i - 1) * W + (j - 1);
	}

	void multiplyx(i32 x, i32 y, X v) {
		for (++y; y <= W; y += (y & -y)) d[idx(x, y)] = MX::op(d[idx(x, y)], v);
	}

	X prodx(i32 x, i32 yl, i32 yr) const {
		X vl = MX::unit();
		X vr = MX::unit();

		for (; yl < yr; yr -= (yr & -yr)) vr = MX::op(vr, d[idx(x, yr)]);
		for (; yr < yl; yl -= (yl & -yl)) vl = MX::op(vl, d[idx(x, yl)]);

		return MX::op(MX::inv(vl), vr);
	}

	X prodx(i32 x, i32 y) const {
		X r = MX::unit();
		for (; y; y -= (y & -y)) r = MX::op(r, d[idx(x, y)]);

		return r;
	}
};

#endif // LIB_FENWICK_TREE_2D_HPP
