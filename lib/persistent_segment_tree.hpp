#ifndef LIB_PERSISTENT_SEGMENT_TREE_HPP
#define LIB_PERSISTENT_SEGMENT_TREE_HPP 1

#include <type_traits>
#include <vector>

#include <lib/algebraic_traits.hpp>
#include <lib/prelude.hpp>

constexpr i32 bit_ceil_log(i32 n) {
    i32 x = 0;
    while ((1 << x) < (i32)(n)) x++;
    return x;
}

template <typename Monoid, u32 MAXIDX>
struct persistent_segment_tree {
    static_assert(is_monoid_v<Monoid>);

    using MX = Monoid;
    using X = typename MX::ValueT;

    using self_t = persistent_segment_tree<MX, MAXIDX>;

    static constexpr u32 log = bit_ceil_log(MAXIDX);
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
        X val;
        node()
            : lch(nullid), rch(nullid), val(MX::unit()) {}
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
    static i32 set(i32 id, u32, X x) {
        id = node_vector<dep>::copy_node(id);
        node_vector<dep>::V[id].val = x;
        return id;
    }

    template <u32 dep, u32 len, std::enable_if_t<dep != log> * = nullptr>
    static i32 set(i32 id, u32 k, X x) {
        id = node_vector<dep>::copy_node(id);
        node_ref<dep> v(id);
        i32 dir = (k >> (log - dep - 1)) & 1;
        if (dir == 0)
            (*v).lch = set<dep + 1, len / 2>((*v).lch, k, x);
        else
            (*v).rch = set<dep + 1, len / 2>((*v).rch, k, x);

        (*v).val = MX::op(node_vector<dep + 1>::get_val((*v).lch), node_vector<dep + 1>::get_val((*v).rch));
        return id;
    }

    template <u32 dep>
    static X get(i32 id, u32 k) {
        if (id == nullid) return MX::unit();
        node_ref<dep> v(id);
        if constexpr (dep == log) {
            return (*v).val;
        } else {
            i32 dir = (k >> (log - dep - 1)) & 1;
            if (dir == 0)
                return get<dep + 1>((*v).lch, k);
            else
                return get<dep + 1>((*v).rch, k);
        }
    }

    template <u32 dep, u32 len, std::enable_if_t<dep == log> * = nullptr>
    static X prod(i32 id, u32 l, u32 r, u32 L, u32 R) {
        if (r <= L || R <= l) return MX::unit();
        return node_vector<dep>::get_val(id);
    }

    template <u32 dep, u32 len, std::enable_if_t<dep != log> * = nullptr>
    static X prod(i32 id, u32 l, u32 r, u32 L, u32 R) {
        if (r <= L || R <= l) return MX::unit();
        if (l <= L && R <= r) return node_vector<dep>::get_val(id);
        if (id == nullid) return MX::unit();
        node_ref<dep> v(id);
        u32 M = (L + R) / 2;
        return MX::op(prod<dep + 1, len / 2>((*v).lch, l, r, L, M), prod<dep + 1, len / 2>((*v).rch, l, r, M, R));
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
    static i32 copy(i32 id1, i32 id2, u32 l, u32 r, u32 L, u32 R) {
        if (r <= L || R <= l) {
            id1 = node_vector<dep>::copy_node(id1);
            return id1;
        } else {
            id2 = node_vector<dep>::copy_node(id2);
            return id2;
        }
    }

    template <u32 dep, u32 len, std::enable_if_t<dep != log> * = nullptr>
    static i32 copy(i32 id1, i32 id2, u32 l, u32 r, u32 L, u32 R) {
        if (r <= L || R <= l) {
            id1 = node_vector<dep>::copy_node(id1);
            return id1;
        }
        if (l <= L && R <= r) {
            id2 = node_vector<dep>::copy_node(id2);
            return id2;
        }
        id1 = node_vector<dep>::copy_node(id1);
        node_ref<dep> v1(id1), v2(id2);
        u32 M = (L + R) / 2;
        if (std::max(L, l) < std::min(M, r)) {
            i32 lch2 = (id2 == nullid ? nullid : (*v2).lch);
            (*v1).lch = copy<dep + 1, len / 2>((*v1).lch, lch2, l, r, L, M);
        }

        if (std::max(M, l) < std::min(R, r)) {
            i32 rch2 = (id2 == nullid ? nullid : (*v2).rch);
            (*v1).rch = copy<dep + 1, len / 2>((*v1).rch, rch2, l, r, M, R);
        }

        (*v1).val = MX::op(node_vector<dep + 1>::get_val((*v1).lch), node_vector<dep + 1>::get_val((*v1).rch));
        return id1;
    }

    i32 rootid;
    persistent_segment_tree(i32 _rootid)
        : rootid(_rootid) {}

public:
    persistent_segment_tree()
        : rootid(nullid) {}
    explicit persistent_segment_tree(const std::vector<X> &V)
        : rootid(build<0>(V, 0, 1 << log)) {}

    self_t set(u32 k, X x) const {
        i32 id = set<0, (u32)1 << log>(rootid, k, x);
        return self_t(id);
    }

    X get(u32 k) const {
        return get<0>(rootid, k);
    }

    X prod(u32 l, u32 r) const {
        return prod<0, (u32)1 << log>(rootid, l, r, 0, (u32)1 << log);
    }

    self_t copy(self_t S, u32 l, u32 r) const {
        return copy<0, (u32)1 << log>(rootid, S.rootid, l, r, 0, (u32)1 << log);
    }
};

template <typename Monoid, u32 MAXIDX>
template <u32 dep>
std::vector<typename persistent_segment_tree<Monoid, MAXIDX>::template node<dep>>
    persistent_segment_tree<Monoid, MAXIDX>::node_vector<dep>::V;

#endif // LIB_PERSISTENT_SEGMENT_TREE_HPP
