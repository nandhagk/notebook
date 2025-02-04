#ifndef LIB_SPLAY_TREE_HPP
#define LIB_SPLAY_TREE_HPP 1

#include <cassert>

#include <lib/prelude.hpp>
#include <lib/splay_tree_base.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_monoid_t<Monoid> * = nullptr>
struct splay_tree_node {
    struct MA {
        using ValueT = bool;
    };

    using MX = Monoid;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    splay_tree_node *l, *r;
    X val, sum, mus;
    bool rev;
    i32 sz;

    explicit splay_tree_node(const X &x)
        : l{nullptr}, r{nullptr}, val{x}, sum{x}, mus{x}, rev{false}, sz{1} {}

    splay_tree_node()
        : splay_tree_node(MX::unit()) {}

    void update() {
        sz = 1;
        mus = sum = val;

        if (l != nullptr) {
            sz += l->sz;
            sum = MX::op(l->sum, sum);
            mus = MX::op(mus, l->mus);
        }

        if (r != nullptr) {
            sz += r->sz;
            sum = MX::op(sum, r->sum);
            mus = MX::op(r->mus, mus);
        }
    }

    void toggle() {
        std::swap(l, r);
        std::swap(sum, mus);
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
using splay_tree = splay_tree_base<splay_tree_node<Monoid>>;

#endif // LIB_SPLAY_TREE_HPP
