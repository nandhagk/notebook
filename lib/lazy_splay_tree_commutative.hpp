#ifndef LIB_LAZY_SPLAY_TREE_COMMUTATIVE_HPP
#define LIB_LAZY_SPLAY_TREE_COMMUTATIVE_HPP 1

#include <cassert>

#include <lib/prelude.hpp>
#include <lib/splay_tree_base.hpp>
#include <lib/type_traits.hpp>

template <typename ActedMonoid>
struct lazy_splay_tree_commutative_node {
    using AM = ActedMonoid;

    using MX = typename AM::MX;
    using MA = typename AM::MA;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    lazy_splay_tree_commutative_node *l, *r, *p;
    X val, sum;
    A lz;
    bool rev;
    i32 sz;

    explicit lazy_splay_tree_commutative_node(const X &x)
        : l{nullptr}, r{nullptr}, p{nullptr}, val{x}, sum{x}, lz{MA::unit()}, rev{false}, sz{1} {}

    lazy_splay_tree_commutative_node()
        : lazy_splay_tree_commutative_node(MX::unit()) {}

    void update() {
        sz = 1;
        sum = val;

        if (l != nullptr) {
            sz += l->sz;
            sum = MX::op(l->sum, sum);
        }

        if (r != nullptr) {
            sz += r->sz;
            sum = MX::op(sum, r->sum);
        }
    }

    void all_apply(const A &a) {
        val = AM::act(val, a, 1);
        lz = MA::op(lz, a);

        sum = AM::act(sum, a, sz);

        if constexpr (has_fail_v<MX>) {
            if (MX::failed(sum)) {
                push();
                update();
            }
        }
    }

    void toggle() {
        std::swap(l, r);
        rev ^= true;
    }

    void push() {
        if (lz != MA::unit()) {
            if (l != nullptr) l->all_apply(lz);
            if (r != nullptr) r->all_apply(lz);
            lz = MA::unit();
        }

        if (rev) {
            if (l != nullptr) l->toggle();
            if (r != nullptr) r->toggle();
            rev = false;
        }
    }

    static constexpr X unit() {
        return MX::unit();
    }
};

template <typename ActedMonoid>
using lazy_splay_tree_commutative = splay_tree_base<lazy_splay_tree_commutative_node<ActedMonoid>>;

#endif // LIB_LAZY_SPLAY_TREE_COMMUTATIVE_HPP
