#ifndef LIB_BASIC_LINK_CUT_TREE_HPP
#define LIB_BASIC_LINK_CUT_TREE_HPP 1

#include <cassert>

#include <lib/link_cut_tree_base.hpp>
#include <lib/prelude.hpp>

template <typename T>
struct basic_link_cut_tree_node {
    struct MA {
        using ValueT = bool;
    };

    struct MX {
        using X = T;
        using ValueT = X;

        static constexpr X unit() {
            return T();
        }
    };

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    basic_link_cut_tree_node *l, *r, *p;
    X val;
    bool rev;
    i32 sz;

    explicit basic_link_cut_tree_node(const X &x)
        : l{nullptr}, r{nullptr}, p{nullptr}, val{x}, rev{false}, sz{1} {}

    basic_link_cut_tree_node()
        : basic_link_cut_tree_node(MX::unit()) {}

    bool is_root() const {
        return !p || (p->l != this && p->r != this);
    }

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

template <typename T>
using basic_link_cut_tree = link_cut_tree_base<basic_link_cut_tree_node<T>>;

#endif // LIB_BASIC_LINK_CUT_TREE_HPP
