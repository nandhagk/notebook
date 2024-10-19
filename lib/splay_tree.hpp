#ifndef LIB_SPLAY_TREE_HPP
#define LIB_SPLAY_TREE_HPP 1

#include <vector>
#include <cassert>
#include <lib/prelude.hpp>

template <typename Node>
struct splay_tree {
	Node *pool;
	const i32 n;
	i32 pid;
	using np = Node*;

	using X = typename Node::ValueT;
	using A = typename Node::OpT;

	std::vector<np> free;

	splay_tree(i32 m) : 
		n(m), pid(0) { pool = new Node[n]; }

	~splay_tree() { delete[] pool; }

	void free_subtree(np c) {
		if (!c) return;

		const auto dfs = [&](auto &&self, np d) -> void {
			if (d->l) self(self, d->l);
			if (d->r) self(self, d->r);
			free.emplace_back(d);
		};

		dfs(dfs, c);
	}

	void reset() {
		pid = 0;
		free.clear();
	}

	np new_root() { return nullptr; }

	np new_node(const X &x) {
		assert(!free.empty() || pid < n);

		np d;
		if (free.empty()) {
			d = &(pool[pid++]);
		} else {
			d = free.back();
			free.pop_back();
		}

		Node::new_node(d, x);
		return d;
	}

	np new_node(const std::vector<X> &dat) {
		const auto dfs = [&](auto &&self, i32 l, i32 r) -> np {
			if (l == r) return nullptr;
			if (r == l + 1) return new_node(dat[l]);

			const i32 m = (l + r) / 2;

			np l_root = self(self, l, m);
			np r_root = self(self, m + 1, r);
			np root = new_node(dat[m]);

			root->l = l_root, root->r = r_root;
			if (l_root) l_root->p = root;
			if (r_root) r_root->p = root;

			root->update();
			return root;
		};

		return dfs(dfs, 0, static_cast<i32>(dat.size()));
	}

	u32 get_size(np root) { return (root ? root->size : 0); }

	np merge(np l_root, np r_root) {
		if (!l_root) return r_root;
		if (!r_root) return l_root;
		assert((!l_root->p) && (!r_root->p));

		splay_kth(r_root, 0);
		r_root->l = l_root;
		l_root->p = r_root;
		r_root->update();
		return r_root;
	}

	np merge3(np a, np b, np c) { return merge(merge(a, b), c); }
	np merge4(np a, np b, np c, np d) { return merge(merge(merge(a, b), c), d); }

	std::pair<np, np> split(np root, u32 k) {
		assert(!root || !root->p);

		if (k == 0) return {nullptr, root};
		if (k == (root->size)) return {root, nullptr};

		splay_kth(root, k - 1);

		np right = root->r;
		root->r = nullptr, right->p = nullptr;
		root->update();

		return {root, right};
	}

	std::tuple<np, np, np> split3(np root, u32 l, u32 r) {
		np nm, nr;
		std::tie(root, nr) = split(root, r);
		std::tie(root, nm) = split(root, l);
		return {root, nm, nr};
	}

	std::tuple<np, np, np, np> split4(np root, u32 i, u32 j, u32 k) {
		np d;
		std::tie(root, d) = split(root, k);
		const auto [a, b, c] = split3(root, i, j);
		return {a, b, c, d};
	}

	void goto_between(np &root, u32 l, u32 r) {
		if (l == 0 && r == root->size) return;

		if (l == 0) {
			splay_kth(root, r);
			root = root->l;
			return;
		}

		if (r == root->size) {
			splay_kth(root, l - 1);
			root = root->r;
			return;
		}

		splay_kth(root, r);

		np rp = root;
		root = rp->l;
		root->p = nullptr;
		splay_kth(root, l - 1);

		root->p = rp;
		rp->l = root;
		rp->update();
		root = root->r;
	}

	std::vector<X> get_all(const np &root) {
		std::vector<X> res;
		const auto dfs = [&](auto &&self, np r) -> void {
			if (!r) return;
			r->prop();

			self(self, r->l);
			res.emplace_back(r->get());
			self(self, r->r);
		};

		dfs(dfs, root);
		return res;
	}

	X get(np &root, u32 k) {
		assert(root == nullptr || !root->p);

		splay_kth(root, k);
		return root->get();
	}

	void set(np &root, u32 k, const X &x) {
		assert(root != nullptr && !root->p);

		splay_kth(root, k);
		root->set(x);
	}

	void multiply(np &root, u32 k, const X &x) {
		assert(root != nullptr && !root->p);

		splay_kth(root, k);
		root->multiply(x);
	}

	X prod(np &root, u32 l, u32 r) {
		assert(root == nullptr || !root->p);

		using MX = typename Node::MX;
		if (l == r) return MX::unit();

		assert(l < r && r <= root->size);

		goto_between(root, l, r);
		X res = root->prod;
		splay(root, true);

		return res;
	}

	X prod(np &root) {
		assert(root == nullptr || !root->p);

		using MX = typename Node::MX;
		return (root ? root->prod : MX::unit());
	}

	void apply(np &root, u32 l, u32 r, const A &a) {
		if (l == r) return;
		assert(l < r && r <= root->size);

		goto_between(root, l, r);
		root->apply(a);
		splay(root, true);
	}

	void apply(np &root, const A &a) {
		if (!root) return;
		root->apply(a);
	}

	void reverse(np &root, u32 l, u32 r) {
		assert(root == nullptr || !root->p);

		if (l == r) return;
		assert(l < r && r <= root->size);

		goto_between(root, l, r);
		root->reverse();
		splay(root, true);
	}

	void reverse(np root) {
		if (!root) return;
		root->reverse();
	}

	void rotate(Node *d) {
		Node *pp, *p, *c;
		p = d->p;
		pp = p->p;

		if (p->l == d) {
			c = d->r;
			d->r = p;
			p->l = c;
		} else {
			c = d->l;
			d->l = p;
			p->r = c;
		}

		if (pp && pp->l == p) pp->l = d;
		if (pp && pp->r == p) pp->r = d;

		d->p = pp;
		p->p = d;

		if (c) c->p = p;
	}

	void prop_from_root(np c) {
		if (!c->p) {
			c->prop();
			return;
		}

		prop_from_root(c->p);
		c->prop();
	}

	void splay(Node *me, bool prop_from_root_done) {
		if (!prop_from_root_done) prop_from_root(me);
		me->prop();
		while (me->p) {
			np p = me->p;
			np pp = p->p;
			if (!pp) {
				rotate(me);
				p->update();
				break;
			}

			bool same = (p->l == me && pp->l == p) || (p->r == me && pp->r == p);
			if (same) rotate(p), rotate(me);
			if (!same) rotate(me), rotate(me);
			pp->update(), p->update();
		}
		me->update();
	}

	void splay_kth(np &root, u32 k) {
		assert(k < (root->size));
		while (1) {
			root->prop();
			u32 sl = (root->l ? root->l->size : 0);
			if (k == sl) break;
			if (k < sl) {
				root = root->l;
			} else {
				k -= sl + 1;
				root = root->r;
			}
		}

		splay(root, true);
	}

	template <typename F>
	std::pair<np, np> split_max_right(np root, F check) {
		if (!root) return {nullptr, nullptr};
		assert(!root->p);
		np c = find_max_right(root, check);
		if (!c) {
			splay(root, true);
			return {nullptr, root};
		}

		splay(c, true);
		np right = c->r;
		if (!right) return {c, nullptr};

		right->p = nullptr;
		c->r = nullptr;
		c->update();
		return {c, right};
	}

	template <typename F>
	std::pair<np, np> split_max_right_cnt(np root, F check) {
		if (!root) return {nullptr, nullptr};
		assert(!root->p);
		np c = find_max_right_cnt(root, check);
		if (!c) {
			splay(root, true);
			return {nullptr, root};
		}

		splay(c, true);
		np right = c->r;
		if (!right) return {c, nullptr};

		right->p = nullptr;
		c->r = nullptr;
		c->update();
		return {c, right};
	}

	template <typename F>
	std::pair<np, np> split_max_right_prod(np root, F check) {
		if (!root) return {nullptr, nullptr};
		assert(!root->p);
		np c = find_max_right_prod(root, check);
		if (!c) {
			splay(root, true);
			return {nullptr, root};
		}

		splay(c, true);
		np right = c->r;
		if (!right) return {c, nullptr};

		right->p = nullptr;
		c->r = nullptr;
		c->update();
		return {c, right};
	}

	template <typename F>
	np find_max_right(np root, const F &check) {
		np last_ok = nullptr, last = nullptr;
		while (root) {
			last = root;
			root->prop();
			if (check(root->x)) {
				last_ok = root;
				root = root->r;
			} else {
				root = root->l;
			}
		}

		splay(last, true);
		return last_ok;
	}

	template <typename F>
	np find_max_right_cnt(np root, const F &check) {
		np last_ok = nullptr, last = nullptr;
		i64 m = 0;
		while (root) {
			last = root;
			root->prop();
			i64 ns = (root->l ? root->l->size : 0);
			if (check(root->x, m + ns + 1)) {
				last_ok = root;
				m += ns + 1;
				root = root->r;
			} else {
				root = root->l;
			}
		}

		splay(last, true);
		return last_ok;
	}

	template <typename F>
	np find_max_right_prod(np root, const F &check) {
		using MX = typename Node::MX;
		X prod = MX::unit();
		np last_ok = nullptr, last = nullptr;
		while (root) {
			last = root;
			root->prop();

			np tmp = root->r;
			root->r = nullptr;
			root->update();

			X lprod = MX::op(prod, root->prod);
			root->r = tmp;
			root->update();

			if (check(lprod)) {
				prod = lprod;
				last_ok = root;
				root = root->r;
			} else {
				root = root->l;
			}
		}

		splay(last, true);
		return last_ok;
	}
};

template <typename S>
struct node_basic {
	using ValueT = S;
	using OpT = i32;
	using np = node_basic*;

	np p, l, r;
	bool rev;
	S x;
	u32 size;

	static void new_node(np n, const S &x) {
		n->p = n->l = n->r = nullptr;
		n->x = x, n->size = 1, n->rev = 0;
	}

	void update() {
		size = 1;
		if (l) { size += l->size; }
		if (r) { size += r->size; }
	}

	void prop() {
		if (rev) {
			if (l) { l->rev ^= 1, std::swap(l->l, l->r); }
			if (r) { r->rev ^= 1, std::swap(r->l, r->r); }
			rev = 0;
		}
	}

	S get() { return x; }

	void set(const S &xx) {
		x = xx;
		update();
	}

	void reverse() {
		std::swap(l, r);
		rev ^= 1;
	}
};

template <typename Monoid>
struct node_monoid {
	using MX = Monoid;
	using X = typename MX::ValueT;

	using ValueT = X;
	using OpT = i32;

	using np = node_monoid*;

	np p, l, r;
	X x, prod, rev_prod;
	u32 size;
	bool rev;

	static void new_node(np n, const X &x) {
		n->p = n->l = n->r = nullptr;
		n->x = n->prod = n->rev_prod = x;
		n->size = 1;
		n->rev = 0;
	}

	void update() {
		size = 1;
		prod = rev_prod = x;
		if (l) {
			size += l->size;
			prod = Monoid::op(l->prod, prod);
			rev_prod = Monoid::op(rev_prod, l->rev_prod);
		}

		if (r) {
			size += r->size;
			prod = Monoid::op(prod, r->prod);
			rev_prod = Monoid::op(r->rev_prod, rev_prod);
		}
	}

	void prop() {
		if (rev) {
			if (l) {
				l->rev ^= 1;
				std::swap(l->l, l->r);
				std::swap(l->prod, l->rev_prod);
			}

			if (r) {
				r->rev ^= 1;
				std::swap(r->l, r->r);
				std::swap(r->prod, r->rev_prod);
			}

			rev = 0;
		}
	}

	X get() { return x; }

	void set(const X &xx) {
		x = xx;
		update();
	}

	void multiply(const X &xx) {
		x = Monoid::op(x, xx);
		update();
	}

	void reverse() {
		std::swap(prod, rev_prod);
		std::swap(l, r);
		rev ^= 1;
	}
};

template <typename ActedMonoid>
struct node_acted_monoid {
	using MA = typename ActedMonoid::MA;
	using MX = typename ActedMonoid::MX;
	using A = typename MA::ValueT;
	using X = typename MX::ValueT;
	using ValueT = X;
	using OpT = A;
	using np = node_acted_monoid*;

	np p, l, r;
	X x, prod;
	A lazy;
	u32 size;
	bool rev;

	static void new_node(np n, const X &x) {
		n->p = n->l = n->r = nullptr;
		n->x = n->prod = x;
		n->lazy = MA::unit();
		n->size = 1;
		n->rev = 0;
	}

	void update() {
		size = 1;
		prod = x;
		if (l) {
			size += l->size;
			prod = MX::op(l->prod, prod);
		}

		if (r) {
			size += r->size;
			prod = MX::op(prod, r->prod);
		}
	}

	void prop() {
		if (lazy != MA::unit()) {
			if (l) { l->apply(lazy); }
			if (r) { r->apply(lazy); }
			lazy = MA::unit();
		}

		if (rev) {
			if (l) { l->reverse(); }
			if (r) { r->reverse(); }
			rev = 0;
		}
	}

	X get() { return x; }

	void set(const X &xx) {
		x = xx;
		update();
	}

	void multiply(const X &xx) {
		x = MX::op(x, xx);
		update();
	}

	void apply(const A &a) {
		x = ActedMonoid::act(x, a, 1);
		prod = ActedMonoid::act(prod, a, size);
		lazy = MA::op(lazy, a);
	}

	void reverse() {
		std::swap(l, r);
		rev ^= 1;
	}
};

template <typename ActedMonoid>
using splay_tree_acted_monoid = splay_tree<node_acted_monoid<ActedMonoid>>;

template <typename S>
using splay_tree_basic = splay_tree<node_basic<S>>;

template <typename Monoid>
using splay_tree_monoid = splay_tree<node_monoid<Monoid>>;

#endif // LIB_SPLAY_TREE_HPP
