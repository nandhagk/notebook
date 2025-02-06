#ifndef LIB_TREAP_HPP
#define LIB_TREAP_HPP 1

#include <cassert>

#include <lib/prelude.hpp>
#include <lib/random.hpp>
#include <lib/treap_base.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_monoid_t<Monoid> * = nullptr>
struct treap_node {
    struct MA {
        using ValueT = bool;
    };

    using MX = Monoid;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    treap_node *l, *r;
    X val, sum, mus;
    bool rev;
    i32 sz;
    u64 pr;

    explicit treap_node(const X &x)
        : l{nullptr}, r{nullptr}, val{x}, sum{x}, mus{x}, rev{false}, sz{1}, pr{MT()} {}

    treap_node()
        : treap_node(MX::unit()) {}

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
using treap = treap_base<treap_node<Monoid>>;

#endif // LIB_TREAP_HPP
