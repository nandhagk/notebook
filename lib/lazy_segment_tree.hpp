#ifndef LIB_LAZY_SEGMENT_TREE_HPP
#define LIB_LAZY_SEGMENT_TREE_HPP 1

#include <cassert>
#include <vector>

#include <lib/prelude.hpp>
#include <lib/monoids/reverse_monoid.hpp>
#include <lib/hld.hpp>

template <typename ActedMonoid>
struct lazy_segment_tree {
	using AM = ActedMonoid;

	using MX = typename AM::MX;
	using MA = typename AM::MA;

	using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	i32 n, log, size;
	std::vector<X> d;
	std::vector<A> z;

	lazy_segment_tree() {}
	explicit lazy_segment_tree(i32 m) { 
		build(m); 
	}

	explicit lazy_segment_tree(const std::vector<X>& v) {
		build(v); 
	}

	template <typename F>
	lazy_segment_tree(i32 m, F f) {
		build(m, f);
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

                log = 1;
                while ((1 << log) < n) ++log;

                size = 1 << log;
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

	template <typename F>
	i32 max_right(const F f, i32 l) {
		assert(0 <= l && l <= n);
		assert(f(MX::unit()));

		if (l == n) return n;

		l += size;
		for (i32 i = log; i >= 1; i--) push(l >> i);

		X sm = MX::unit();
		do {
			while (l % 2 == 0) l >>= 1;
			if (!f(MX::op(sm, d[l]))) {
				while (l < size) {
					push(l);
					l = (2 * l);
					if (f(MX::op(sm, d[l]))) { 
						sm = MX::op(sm, d[l++]); 
					}
				}

				return l - size;
			}
			sm = MX::op(sm, d[l++]);
		} while ((l & -l) != l);

		return n;
	}

	template <typename F>
	i32 min_left(const F f, i32 r) {
		assert(0 <= r && r <= n);
		assert(f(MX::unit()));

		if (r == 0) return 0;

		r += size;
		for (i32 i = log; i >= 1; i--) push((r - 1) >> i);

		X sm = MX::unit();
		do {
			r--;
			while (r > 1 && (r % 2)) r >>= 1;

			if (!f(MX::op(d[r], sm))) {
				while (r < size) {
					push(r);
					r = (2 * r + 1);
					if (f(MX::op(d[r], sm))) { 
						sm = MX::op(d[r--], sm); 
					}
				}

				return r + 1 - size;
			}
			sm = MX::op(d[r], sm);
		} while ((r & -r) != r);

		return 0;
	}

private:
	void apply_at(i32 k, A a) {
		const i64 sz = 1 << (log - topbit(k));

		d[k] = AM::act(d[k], a, sz);
		if (k < size) z[k] = MA::op(z[k], a);
	}

	void push(i32 k) {
		if (z[k] == MA::unit()) return;

		apply_at(2 * k, z[k]);
		apply_at(2 * k + 1, z[k]);

		z[k] = MA::unit();
	}
};

template <class ActedMonoid>
struct hld_lazy_segment_tree {
	using AM = ActedMonoid;
	
        using MX = typename AM::MX;
	using MA = typename AM::MA;

        using X = typename MX::ValueT;
	using A = typename MA::ValueT;

	const hld& h;

	struct RAM {
		using MX = monoid_reverse_monoid<hld_lazy_segment_tree::MX>;
		using MA = hld_lazy_segment_tree::MA;

		using X = typename MX::ValueT;
		using A = typename MA::ValueT;

		static constexpr X act(const X& x, const A& a, const i64 size) {
			return AM::act(x, a, size);
		}
	};

	lazy_segment_tree<ActedMonoid> st;
	lazy_segment_tree<RAM> rst;

        explicit hld_lazy_segment_tree(const hld &g): h(g) {
                build();
        }

        template <typename F>
        hld_lazy_segment_tree(const hld& g, F f): h(g) {
                build(f);
        }

        explicit hld_lazy_segment_tree(const hld& g, const std::vector<X> &v): h(g) {
                build(v);
        }

        void build() {
                build([](i32) -> X { return MX::unit(); });
        }

        void build(const std::vector<X> &v) {
                build([&](i32 u) -> X { return v[h.tour[u]]; });
        }

        template <typename F>
        void build(F f) {
		st.build(h.n, f);
		if constexpr (!MX::commutative) rst.build(h.n, f);
        }

        X get(i32 u) {
                return st.get(h.tin[u]);
        }

        void set(i32 u, const X &x) {
		st.set(h.tin[u], x);
		if constexpr (!MX::commutative) rst.set(h.tin[u], x);
        }

        void multiply(i32 u, const X &x) {
		st.multiply(h.tin[u], x);
		if constexpr (!MX::commutative) rst.multiply(h.tin[u], x);
        }

        X prod_path(i32 u, i32 v) {
		X x = MX::unit();
		for (const auto &[s, t] : h.decompose(u, v)) {
			x = MX::op(x, prod(s, t));
		}

		return x;
        }

	X prod_subtree(i32 u) {
		return st.prod(h.tin[u], h.tin[u] + h.sz[u]);
	}

	X prod_all() { 
		return st.prod_all(); 
	}

	X prod(i32 u, i32 v) {
		const i32 a = h.tin[u];
		const i32 b = h.tin[v];

		if (a <= b) return st.prod(a, b + 1);
		if constexpr (!MX::commutative) return rst.prod(b, a + 1);

		return st.prod(b, a + 1);
	}

	void apply_path(i32 u, i32 v, A a) {
		for (const auto &[s, t] : h.decompose(u, v)) {
			const auto &[x, y] = std::minmax(h.tin[s], h.tin[t]);

			st.apply(x, y + 1, a);
			if constexpr (!MX::commutative) rst.apply(x, y + 1, a);
		}
	}

	void apply_subtree(i32 u, A a) {
		const i32 x = h.tin[u];
		const i32 y = h.tin[u] + h.sz[u];

		st.apply(x, y, a);
		if constexpr (!MX::commutative) rst.apply(x, y, a);
	}
};

#endif // LIB_LAZY_SEGMENT_TREE_HPP
