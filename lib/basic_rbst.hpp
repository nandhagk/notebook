#ifndef LIB_BASIC_RBST_HPP
#define LIB_BASIC_RBST_HPP 1

#include <cassert>

#include <lib/prelude.hpp>
#include <lib/rbst_base.hpp>

template <typename T>
struct basic_rbst_node {
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

    basic_rbst_node *l, *r;
    X val;
    bool rev;
    i32 sz;

    explicit basic_rbst_node(const X &x)
        : l{nullptr}, r{nullptr}, val{x}, rev{false}, sz{1} {}

    basic_rbst_node()
        : basic_rbst_node(MX::unit()) {}

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
using basic_rbst = rbst_base<basic_rbst_node<T>>;

#endif // LIB_BASIC_RBST_HPP
