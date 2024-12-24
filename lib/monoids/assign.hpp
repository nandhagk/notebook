#ifndef LIB_MONOID_ASSIGN_HPP
#define LIB_MONOID_ASSIGN_HPP 1

#include <optional>

template <typename T> struct monoid_assign {
    using X = std::optional<T>;
    using ValueT = X;

    static constexpr X op(X x, X y) { return y == std::nullopt ? x : y; }

    static constexpr X unit() { return std::nullopt; }

    static constexpr bool commutative = false;
};

#endif // LIB_MONOID_ASSIGN_HPP
