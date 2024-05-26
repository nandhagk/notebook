#ifndef LIB_LAZY_SEGMENT_TREE_HPP
#define LIB_LAZY_SEGMENT_TREE_HPP 1

#include <algorithm>
#include <vector>
#include <functional>
#include <cassert>
#include <lib/prelude.hpp>

template<class S, auto op>
concept IsLazySegmentTreeOp = std::is_convertible<decltype(op), std::function<S(S, S)>>::value;

template <class S, auto e>
concept IsLazySegmentTreeE = std::is_convertible<decltype(e), std::function<S()>>::value;

template<class S, class F, auto mapping>
concept IsLazySegmentTreeMapping = std::is_convertible<decltype(mapping), std::function<F(F, S)>>::value;

template<class F, auto composition>
concept IsLazySegmentTreeComposition = std::is_convertible<decltype(composition), std::function<F(F, F)>>::value;

template<class F, auto id>
concept IsLazySegmentTreeID = std::is_convertible<decltype(id), std::function<F()>>::value;

template <class S, class G>
concept IsLazySegmentTreeG = std::is_convertible<G, std::function<bool(S)>>::value;

template <class S, auto op, auto e, class F, auto mapping, auto composition, auto id>
	requires IsLazySegmentTreeOp<S, op>
			&& IsLazySegmentTreeE<S, e>
			&& IsLazySegmentTreeMapping<S, F, mapping>
			&& IsLazySegmentTreeComposition<F, composition>
			&& IsLazySegmentTreeID<F, id>
struct LazySegmentTree {
public:
	LazySegmentTree() : LazySegmentTree(0) {}

	explicit LazySegmentTree(i32 n_) : LazySegmentTree(std::vector<S>(n_, e())) {}
	explicit LazySegmentTree(const std::vector<S>& v) :
		n(static_cast<int>(v.size())),
		size(std::bit_ceil(static_cast<u32>(n))),
		log(std::countr_zero(static_cast<u32>(size))),
		d(2 * size, e()),
		lz(size, id())
	{
		for (i32 i = 0; i < n; i++) d[size + i] = v[i];
		for (i32 i = size - 1; i >= 1; i--) {
			update(i);
		}
	}

	void set(i32 p, S x) {
		assert(0 <= p && p < n);

		p += size;
		for (i32 i = log; i >= 1; i--) push(p >> i);

		d[p] = x;
		for (i32 i = 1; i <= log; i++) update(p >> i);
	}

	S get(i32 p) {
		assert(0 <= p && p < n);

		p += size;
		for (i32 i = log; i >= 1; i--) push(p >> i);

		return d[p];
	}

	S prod(i32 l, i32 r) {
		assert(0 <= l && l <= r && r <= n);

		if (l == r) return e();

		l += size;
		r += size;

		for (i32 i = log; i >= 1; i--) {
			if (((l >> i) << i) != l) push(l >> i);
			if (((r >> i) << i) != r) push((r - 1) >> i);
		}

		S sml = e(), smr = e();
		while (l < r) {
			if (l & 1) sml = op(sml, d[l++]);
			if (r & 1) smr = op(d[--r], smr);
			l >>= 1;
			r >>= 1;
		}

		return op(sml, smr);
	}

	S all_prod() {
		return d[1];
	}

	void apply(i32 p, F f) {
		assert(0 <= p && p < n);

		p += size;
		for (i32 i = log; i >= 1; i--) push(p >> i);

		d[p] = mapping(f, d[p]);
		for (i32 i = 1; i <= log; i++) update(p >> i);
	}

	void apply(i32 l, i32 r, F f) {
		assert(0 <= l && l <= r && r <= n);

		if (l == r) return;

		l += size;
		r += size;

		for (i32 i = log; i >= 1; i--) {
			if (((l >> i) << i) != l) push(l >> i);
			if (((r >> i) << i) != r) push((r - 1) >> i);
		}

		{
			i32 l2 = l, r2 = r;
			while (l < r) {
				if (l & 1) all_apply(l++, f);
				if (r & 1) all_apply(--r, f);
				l >>= 1;
				r >>= 1;
			}

			l = l2;
			r = r2;
		}

		for (i32 i = 1; i <= log; i++) {
			if (((l >> i) << i) != l) update(l >> i);
			if (((r >> i) << i) != r) update((r - 1) >> i);
		}
	}

	template <class G>
		requires IsLazySegmentTreeG<S, G>
	i32 max_right(i32 l, G g) {
		assert(0 <= l && l <= n);
		assert(g(e()));

		if (l == n) return n;

		l += size;
		for (i32 i = log; i >= 1; i--) push(l >> i);

		S sm = e();
		do {
			while (l % 2 == 0) l >>= 1;

			if (!g(op(sm, d[l]))) {
				while (l < size) {
					push(l);
					l = (2 * l);
					if (g(op(sm, d[l]))) {
						sm = op(sm, d[l]);
						l++;
					}
				}

				return l - size;
			}

			sm = op(sm, d[l]);
			l++;
		} while ((l & -l) != l);

		return n;
	}

	template <class G>
		requires IsLazySegmentTreeG<S, G>
	i32 min_left(i32 r, G g) {
		assert(0 <= r && r <= n);
		assert(g(e()));

		if (r == 0) return 0;

		r += size;
		for (i32 i = log; i >= 1; i--) push((r - 1) >> i);

		S sm = e();
		do {
			r--;
			while (r > 1 && (r % 2)) r >>= 1;

			if (!g(op(d[r], sm))) {
				while (r < size) {
					push(r);
					r = (2 * r + 1);
					if (g(op(d[r], sm))) {
						sm = op(d[r], sm);
						r--;
					}
				}

				return r + 1 - size;
			}

			sm = op(d[r], sm);
		} while ((r & -r) != r);

		return 0;
	}

private:
	i32 n, size, log;
	std::vector<S> d;
	std::vector<F> lz;

	void update(i32 k) {
		d[k] = op(d[2 * k], d[2 * k + 1]);
	}

	void all_apply(i32 k, F f) {
		d[k] = mapping(f, d[k]);
		if (k < size) lz[k] = composition(f, lz[k]);
	}

	void push(i32 k) {
		all_apply(2 * k, lz[k]);
		all_apply(2 * k + 1, lz[k]);
		lz[k] = id();
	}
};

#endif // LIB_LAZY_SEGMENT_TREE_HPP
