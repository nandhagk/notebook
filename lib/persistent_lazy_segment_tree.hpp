#ifndef LIB_PERSISTENT_LAZY_SEGMENT_TREE_HPP
#define LIB_PERSISTENT_LAZY_SEGMENT_TREE_HPP 1

#include <type_traits>
#include <vector>

#include <lib/prelude.hpp>

constexpr i32 bit_ceil_log_(i32 n) {
    i32 x = 0;
    while ((1 << x) < (i32)(n)) x++;
    return x;
}

template <typename ActedMonoid, u32 MAXIDX>
struct persistent_lazy_segment_tree {
    using AM = ActedMonoid;

    using MX = typename AM::MX;
    using MA = typename AM::MA;

    using X = typename MX::ValueT;
    using A = typename MA::ValueT;

    using self_t = persistent_lazy_segment_tree<AM, MAXIDX>;

    static constexpr u32 log = bit_ceil_log_(MAXIDX);
    static constexpr i32 nullid = -1;

    template <u32 dep, typename = void>
    struct node {};

    template <u32 dep>
    struct node<dep, std::enable_if_t<dep == log>> {
        X val;
        node()
            : val(MX::unit()) {}
    };

    template <u32 dep>
    struct node<dep, std::enable_if_t<dep != log>> {
        i32 lch, rch;
        A lzl, lzr;
        X val;
        node()
            : lch(nullid), rch(nullid), lzl(MA::unit()), lzr(MA::unit()), val(MX::unit()) {}
    };

    template <u32 dep>
    struct node_vector {
        static std::vector<node<dep>> V;
        static i32 copy_node(i32 id = nullid) {
            if (id == nullid) {
                id = static_cast<i32>(V.size());
                V.push_back(node<dep>());
                return id;
            } else {
                V.push_back(V[id]);
                id = static_cast<i32>(V.size()) - 1;
                return id;
            }
        }
        static X get_val(i32 id) {
            if (id == nullid) return MX::unit();
            return V[id].val;
        }
    };

    template <u32 dep>
    struct node_ref {
        i32 id;
        node_ref(i32 _id)
            : id(_id) {}
        node<dep> &operator*() {
            return node_vector<dep>::V[id];
        }
    };

    template <u32 dep, u32 len, std::enable_if_t<dep == log> * = nullptr>
    static void all_apply(i32 id, A lz) {
        node_ref<dep> v(id);
        (*v).val = AM::act((*v).val, lz, len);
    }

    template <u32 dep, u32 len, std::enable_if_t<dep != log> * = nullptr>
    static void all_apply(i32 id, A lz) {
        node_ref<dep> v(id);
        (*v).val = AM::act((*v).val, lz, len);
        (*v).lzl = MA::op((*v).lzl, lz);
        (*v).lzr = MA::op((*v).lzr, lz);
    }

    template <u32 dep, u32 len, std::enable_if_t<dep == log> * = nullptr>
    static i32 set(i32 id, u32, X x, A) {
        id = node_vector<dep>::copy_node(id);
        node_vector<dep>::V[id].val = x;
        return id;
    }

    template <u32 dep, u32 len, std::enable_if_t<dep != log> * = nullptr>
    static i32 set(i32 id, u32 k, X x, A lz) {
        id = node_vector<dep>::copy_node(id);
        node_ref<dep> v(id);
        all_apply<dep, len>(id, lz);
        i32 dir = (k >> (log - dep - 1)) & 1;
        if (dir == 0) {
            (*v).lch = set<dep + 1, len / 2>((*v).lch, k, x, MA::op((*v).lzl, lz));
            (*v).lzl = MA::unit();
            X val_r = AM::act(node_vector<dep + 1>::get_val((*v).rch), MA::op((*v).lzr, lz), len / 2);
            (*v).val = MX::op(node_vector<dep + 1>::get_val((*v).lch), val_r);
        } else {
            (*v).rch = set<dep + 1, len / 2>((*v).rch, k, x, MA::op((*v).lzr, lz));
            (*v).lzr = MA::unit();
            X val_l = AM::act(node_vector<dep + 1>::get_val((*v).lch), MA::op((*v).lzl, lz), len / 2);
            (*v).val = MX::op(val_l, node_vector<dep + 1>::get_val((*v).rch));
        }
        return id;
    }

    template <u32 dep>
    static X get(i32 id, u32 k, A lz) {
        if (id == nullid) return AM::act(MX::unit(), lz, 1);
        node_ref<dep> v(id);
        if constexpr (dep == log) {
            return AM::act((*v).val, lz, 1);
        } else {
            i32 dir = (k >> (log - dep - 1)) & 1;
            if (dir == 0)
                return get<dep + 1>((*v).lch, k, MA::op((*v).lzl, lz));
            else
                return get<dep + 1>((*v).rch, k, MA::op((*v).lzr, lz));
        }
    }

    template <u32 dep, u32 len, std::enable_if_t<dep == log> * = nullptr>
    static i32 apply(i32 id, u32 l, u32 r, u32 L, u32 R, A lz, A lz2) {
        id = node_vector<dep>::copy_node(id);
        if (r <= L || R <= l)
            all_apply<dep, len>(id, lz);
        else
            all_apply<dep, len>(id, MA::op(lz, lz2));
        return id;
    }

    template <u32 dep, u32 len, std::enable_if_t<dep != log> * = nullptr>
    static i32 apply(i32 id, u32 l, u32 r, u32 L, u32 R, A lz, A lz2) {
        id = node_vector<dep>::copy_node(id);
        all_apply<dep, len>(id, lz);
        if (r <= L || R <= l) return id;
        if (l <= L && R <= r) {
            all_apply<dep, len>(id, lz2);
            return id;
        }
        node_ref<dep> v(id);
        u32 M = (L + R) / 2;
        X suml = MX::unit(), sumr = MX::unit();
        if (std::max(L, l) < std::min(M, r)) {
            (*v).lch = apply<dep + 1, len / 2>((*v).lch, l, r, L, M, (*v).lzl, lz2);
            (*v).lzl = MA::unit();
            suml = node_vector<dep + 1>::get_val((*v).lch);
        } else {
            suml = AM::act(node_vector<dep + 1>::get_val((*v).lch), (*v).lzl, len / 2);
        }
        if (std::max(M, l) < std::min(R, r)) {
            (*v).rch = apply<dep + 1, len / 2>((*v).rch, l, r, M, R, (*v).lzr, lz2);
            (*v).lzr = MA::unit();
            sumr = node_vector<dep + 1>::get_val((*v).rch);
        } else {
            sumr = AM::act(node_vector<dep + 1>::get_val((*v).rch), (*v).lzr, len / 2);
        }
        (*v).val = MX::op(suml, sumr);
        return id;
    }

    template <u32 dep, u32 len, std::enable_if_t<dep == log> * = nullptr>
    static X prod(i32 id, u32 l, u32 r, u32 L, u32 R, A lz) {
        if (r <= L || R <= l) return MX::unit();
        return AM::act(node_vector<dep>::get_val(id), lz, len);
    }

    template <u32 dep, u32 len, std::enable_if_t<dep != log> * = nullptr>
    static X prod(i32 id, u32 l, u32 r, u32 L, u32 R, A lz) {
        if (r <= L || R <= l) return MX::unit();
        if (l <= L && R <= r) return AM::act(node_vector<dep>::get_val(id), lz, len);
        if (id == nullid) return AM::act(MX::unit(), lz, std::min(R, r) - std::max(L, l));
        node_ref<dep> v(id);
        u32 M = (L + R) / 2;
        return MX::op(prod<dep + 1, len / 2>((*v).lch, l, r, L, M, MA::op((*v).lzl, lz)),
                      prod<dep + 1, len / 2>((*v).rch, l, r, M, R, MA::op((*v).lzr, lz)));
    }

    template <u32 dep, std::enable_if_t<dep == log> * = nullptr>
    i32 build(const std::vector<X> &V, u32 L, u32) {
        if (L >= V.size()) return nullid;
        i32 id = node_vector<dep>::copy_node();
        node_vector<dep>::V[id].val = V[L];
        return id;
    }

    template <u32 dep, std::enable_if_t<dep != log> * = nullptr>
    i32 build(const std::vector<X> &V, u32 L, u32 R) {
        if (L >= V.size()) return nullid;
        i32 id = node_vector<dep>::copy_node();
        node_ref<dep> v(id);
        u32 M = (L + R) / 2;
        (*v).lch = build<dep + 1>(V, L, M);
        (*v).rch = build<dep + 1>(V, M, R);
        (*v).val = MX::op(node_vector<dep + 1>::get_val((*v).lch), node_vector<dep + 1>::get_val((*v).rch));
        return id;
    }

    template <u32 dep, u32 len, std::enable_if_t<dep == log> * = nullptr>
    static i32 copy(i32 id1, i32 id2, u32 l, u32 r, u32 L, u32 R, A lz1, A lz2) {
        if (r <= L || R <= l) {
            id1 = node_vector<dep>::copy_node(id1);
            all_apply<dep, len>(id1, lz1);
            return id1;
        } else {
            id2 = node_vector<dep>::copy_node(id2);
            all_apply<dep, len>(id2, lz2);
            return id2;
        }
    }

    template <u32 dep, u32 len, std::enable_if_t<dep != log> * = nullptr>
    static i32 copy(i32 id1, i32 id2, u32 l, u32 r, u32 L, u32 R, A lz1, A lz2) {
        if (r <= L || R <= l) {
            id1 = node_vector<dep>::copy_node(id1);
            all_apply<dep, len>(id1, lz1);
            return id1;
        }
        if (l <= L && R <= r) {
            id2 = node_vector<dep>::copy_node(id2);
            all_apply<dep, len>(id2, lz2);
            return id2;
        }
        id1 = node_vector<dep>::copy_node(id1);
        all_apply<dep, len>(id1, lz1);
        node_ref<dep> v1(id1), v2(id2);
        u32 M = (L + R) / 2;
        X suml = MX::unit(), sumr = MX::unit();
        if (std::max(L, l) < std::min(M, r)) {
            i32 lch2 = (id2 == nullid ? nullid : (*v2).lch);
            A llz2 = (id2 == nullid ? lz2 : MA::op((*v2).lzl, lz2));
            (*v1).lch = copy<dep + 1, len / 2>((*v1).lch, lch2, l, r, L, M, (*v1).lzl, llz2);
            (*v1).lzl = MA::unit();
            suml = node_vector<dep + 1>::get_val((*v1).lch);
        } else {
            suml = AM::act(node_vector<dep + 1>::get_val((*v1).lch), (*v1).lzl, len / 2);
        }
        if (std::max(M, l) < std::min(R, r)) {
            i32 rch2 = (id2 == nullid ? nullid : (*v2).rch);
            A rlz2 = (id2 == nullid ? lz2 : MA::op((*v2).lzr, lz2));
            (*v1).rch = copy<dep + 1, len / 2>((*v1).rch, rch2, l, r, M, R, (*v1).lzr, rlz2);
            (*v1).lzr = MA::unit();
            sumr = node_vector<dep + 1>::get_val((*v1).rch);
        } else {
            sumr = AM::act(node_vector<dep + 1>::get_val((*v1).rch), (*v1).lzr, len / 2);
        }
        (*v1).val = MX::op(suml, sumr);
        return id1;
    }

    i32 rootid;
    persistent_lazy_segment_tree(i32 _rootid)
        : rootid(_rootid) {}

public:
    persistent_lazy_segment_tree()
        : rootid(nullid) {}
    explicit persistent_lazy_segment_tree(const std::vector<X> &V)
        : rootid(build<0>(V, 0, 1 << log)) {}

    self_t set(u32 k, X x) const {
        i32 id = set<0, (u32)1 << log>(rootid, k, x, MA::unit());
        return self_t(id);
    }

    X get(u32 k) const {
        return get<0>(rootid, k, MA::unit());
    }

    self_t apply(u32 l, u32 r, A lz) const {
        i32 id = apply<0, (u32)1 << log>(rootid, l, r, 0, (u32)1 << log, MA::unit(), lz);
        return self_t(id);
    }

    X prod(u32 l, u32 r) const {
        return prod<0, (u32)1 << log>(rootid, l, r, 0, (u32)1 << log, MA::unit());
    }

    self_t copy(self_t S, u32 l, u32 r) const {
        return copy<0, (u32)1 << log>(rootid, S.rootid, l, r, 0, (u32)1 << log, MA::unit(), MA::unit());
    }
};

template <typename ActedMonoid, u32 MAXIDX>
template <u32 dep>
std::vector<typename persistent_lazy_segment_tree<ActedMonoid, MAXIDX>::template node<dep>>
    persistent_lazy_segment_tree<ActedMonoid, MAXIDX>::node_vector<dep>::V;

#endif // LIB_PERSISTENT_LAZY_SEGMENT_TREE_HPP
