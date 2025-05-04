#ifndef LIB_IS_TREE_ISOMORPHIC_HPP
#define LIB_IS_TREE_ISOMORPHIC_HPP 1

#include <map>

#include <lib/centroids.hpp>
#include <lib/prelude.hpp>
#include <lib/rooted_tree_isomorphism.hpp>

template <typename Map, typename Tree>
inline bool is_tree_isomorphic_rooted(Map &k, const Tree &g1, const Tree &g2, i32 r1, i32 r2) {
    if (g1.size() != g2.size()) return false;

    const auto t1 = rooted_tree_isomorphism(k, g1, r1);
    const auto t2 = rooted_tree_isomorphism(k, g2, r2);

    return t1[r1] == t2[r2];
}

template <typename Tree>
inline bool is_tree_isomorphic_rooted(const Tree &g1, const Tree &g2, i32 r1, i32 r2) {
    std::map<std::vector<i32>, i32> k;
    return is_tree_isomorphic_rooted(k, g1, g2, r1, r2);
}

template <typename Map, typename Tree>
inline bool is_tree_isomorphic_rooted(Map &k, const Tree &g1, const Tree &g2) {
    return is_tree_isomorphic_rooted(k, g1, g2, 0, 0);
}

template <typename Tree>
inline bool is_tree_isomorphic_rooted(const Tree &g1, const Tree &g2) {
    return is_tree_isomorphic_rooted(g1, g2, 0, 0);
}

template <typename Map, typename Tree>
inline bool is_tree_isomorphic_unrooted(Map &k, const Tree &g1, const Tree &g2, const std::vector<i32> &c1,
                                        const std::vector<i32> &c2) {
    if (c1.size() != c2.size()) return false;

    const auto t2 = rooted_tree_isomorphism(k, g2, c2[0]);

    auto t1 = rooted_tree_isomorphism(k, g1, c1[0]);
    if (t1[c1[0]] == t2[c2[0]]) return true;
    if (c1.size() == 1) return false;

    t1 = rooted_tree_isomorphism(k, g1, c1[1]);
    return t1[c1[1]] == t2[c2[0]];
}

template <typename Tree>
inline bool is_tree_isomorphic_unrooted(const Tree &g1, const Tree &g2, const std::vector<i32> &c1,
                                        const std::vector<i32> &c2) {
    std::map<std::vector<i32>, i32> k;
    return is_tree_isomorphic_unrooted(k, g1, g2, c1, c2);
}

template <typename Map, typename Tree>
inline bool is_tree_isomorphic_unrooted(Map &k, const Tree &g1, const Tree &g2) {
    const auto c1 = centroids(g1);
    const auto c2 = centroids(g2);

    return is_tree_isomorphic_unrooted(k, g1, g2, c1, c2);
}

template <typename Tree>
inline bool is_tree_isomorphic_unrooted(const Tree &g1, const Tree &g2) {
    std::map<std::vector<i32>, i32> k;
    return is_tree_isomorphic_unrooted(k, g1, g2);
}

#endif // LIB_IS_TREE_ISOMORPHIC_HPP
