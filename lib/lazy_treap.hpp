#ifndef LIB_LAZY_TREAP_HPP
#define LIB_LAZY_TREAP_HPP 1

#include <cassert>

#include <lib/prelude.hpp>
#include <lib/random.hpp>
#include <lib/treap_base.hpp>
#include <lib/type_traits.hpp>

template <typename ActedMonoid>
struct lazy_treap_node {
    using AM = ActedMonoid;

    using MX = typename AM::MX;
    using MA = typename AM::MA;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    lazy_treap_node *l, *r;
    X val, sum, mus;
    A lz;
    bool rev;
    i32 sz;
    u64 pr;

    explicit lazy_treap_node(const X &x)
        : l{nullptr}, r{nullptr}, val{x}, sum{x}, mus{x}, lz{MA::unit()}, rev{false}, sz{1}, pr{MT()} {}

    lazy_treap_node()
        : lazy_treap_node(MX::unit()) {}

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

    void all_apply(const A &a) {
        val = AM::act(val, a, 1);
        lz = MA::op(lz, a);

        sum = AM::act(sum, a, sz);
        mus = AM::act(mus, a, sz);

        if constexpr (has_fail_v<MX>) {
            if (MX::failed(sum)) {
                push();
                update();
            }
        }
    }

    void toggle() {
        std::swap(l, r);
        std::swap(sum, mus);
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
};

template <typename ActedMonoid>
using lazy_treap = treap_base<lazy_treap_node<ActedMonoid>>;

#endif // LIB_LAZY_TREAP_HPP
