#ifndef LIB_BASIC_SPLAY_TREE_HPP
#define LIB_BASIC_SPLAY_TREE_HPP 1

#include <cassert>

#include <lib/prelude.hpp>
#include <lib/splay_tree_base.hpp>

template <typename T>
struct basic_splay_tree_node {
    struct MX {
        using X = T;
        using ValueT = X;

        static constexpr X unit() {
            return T();
        }
    };

    struct MA {
        using ValueT = bool;
    };

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    basic_splay_tree_node *l, *r;
    X val;
    bool rev;
    i32 sz;

    explicit basic_splay_tree_node(const X &x)
        : l{nullptr}, r{nullptr}, val{x}, rev{false}, sz{1} {}

    basic_splay_tree_node()
        : basic_splay_tree_node(MX::unit()) {}

    void update() {
        sz = 1;
        if (l != nullptr) sz += l->sz;
        if (r != nullptr) sz += r->sz;
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
using basic_splay_tree = splay_tree_base<basic_splay_tree_node<Monoid>>;

#endif // LIB_BASIC_SPLAY_TREE_HPP
