#ifndef LIB_MONOID_AFFINE_SZ_HPP
#define LIB_MONOID_AFFINE_SZ_HPP 1

#include <tuple>

#include <lib/prelude.hpp>
#include <lib/monoids/affine.hpp>

template <class T>
struct monoid_affine_sz {
        using X = std::tuple<T, T, i32>;
        using ValueT = X;

        static constexpr X op(const X &x, const X &y) {
                const auto [a, b, c] = x;
                const auto [p, q, r] = y;
                return {a * p, b * p + q, c + r};
        }

        static constexpr X inv(const T &x) {
                auto [a, b, c] = x;
                a = T(1) / a;
                return {a, a * (-b), c};
        }

        static constexpr T eval(const X &f, T x) {
                const auto [a, b, _] = f;
                return a * x + b;
        }

        static constexpr X unit() { 
                return {T(1), T(0), 0}; 
        }

        static constexpr X pow(const X& x, i64 n) {
                using M = monoid_affine<T>;
                using Z = typename M::X;

                const auto [a, b, c] = x;
                Z y = {a, b};

                Z z = M::unit();
                while (n) {
                        if (n & 1) z = M::op(z, y);
                        y = M::op(y, y);
                        n >>= 1;
                }

                const auto [p, q] = z;
                return {p, q, c};
        }

        static constexpr X from_element(const T& a, const T& b) {
                return {a, b, 1};
        }

        static constexpr bool commutative = false;
};

#endif // LIB_MONOID_AFFINE_SZ_HPP
