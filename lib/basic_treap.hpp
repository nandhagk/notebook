#ifndef LIB_BASIC_TREAP_HPP
#define LIB_BASIC_TREAP_HPP 1

#include <cassert>

#include <lib/prelude.hpp>
#include <lib/random.hpp>
#include <lib/treap_base.hpp>

template <typename T>
struct basic_treap_node {
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

    basic_treap_node *l, *r;
    X val;
    bool rev;
    i32 sz;
    u64 pr;

    explicit basic_treap_node(const X &x)
        : l{nullptr}, r{nullptr}, val{x}, rev{false}, sz{1}, pr{MT()} {}

    basic_treap_node()
        : basic_treap_node(MX::unit()) {}

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
using basic_treap = treap_base<basic_treap_node<T>>;

#endif // LIB_BASIC_TREAP_HPP
