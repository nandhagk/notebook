#ifndef LIB_LINK_CUT_TREE_HPP
#define LIB_LINK_CUT_TREE_HPP 1

#include <cassert>

#include <lib/algebraic_traits.hpp>
#include <lib/link_cut_tree_base.hpp>
#include <lib/prelude.hpp>

template <typename Monoid, is_monoid_t<Monoid> * = nullptr>
struct link_cut_tree_node {
    struct MA {
        using ValueT = bool;
    };

    using MX = Monoid;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    link_cut_tree_node *l, *r, *p;
    X val, sum;
    bool rev;
    i32 sz;

    explicit link_cut_tree_node(const X &x)
        : l{nullptr}, r{nullptr}, p{nullptr}, val{x}, sum{x}, rev{false}, sz{1} {}

    link_cut_tree_node()
        : link_cut_tree_node(MX::unit()) {}

    bool is_root() const {
        return !p || (p->l != this && p->r != this);
    }

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
        if constexpr (has_rev_v<MX>) sum = MX::rev(sum);
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
using link_cut_tree = link_cut_tree_base<link_cut_tree_node<Monoid>>;

#endif // LIB_LINK_CUT_TREE_HPP
