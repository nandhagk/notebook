#ifndef LIB_SPLAY_TREE_COMMUTATIVE_HPP
#define LIB_SPLAY_TREE_COMMUTATIVE_HPP 1

#include <cassert>

#include <lib/prelude.hpp>
#include <lib/splay_tree_base.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_monoid_t<Monoid> * = nullptr>
struct splay_tree_commutative_node {
    struct MA {
        using ValueT = bool;
    };

    using MX = Monoid;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    splay_tree_commutative_node *l, *r, *p;
    X val, sum;
    bool rev;
    i32 sz;

    explicit splay_tree_commutative_node(const X &x)
        : l{nullptr}, r{nullptr}, p{nullptr}, val{x}, sum{x}, rev{false}, sz{1} {}

    splay_tree_commutative_node()
        : splay_tree_commutative_node(MX::unit()) {}

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

    void toggle() {
        std::swap(l, r);
        rev ^= true;
    }

    void push() {
        if (rev) {
            if (l != nullptr) l->toggle();
            if (r != nullptr) r->toggle();
            rev = false;
        }
    }
};

template <typename Monoid>
using splay_tree_commutative = splay_tree_base<splay_tree_commutative_node<Monoid>>;

#endif // LIB_SPLAY_TREE_COMMUTATIVE_HPP
