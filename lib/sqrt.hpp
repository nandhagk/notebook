#ifndef LIB_SQRT_HPP
#define LIB_SQRT_HPP 1

#include <optional>

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>
#include <lib/arbitrary_montgomery_modint.hpp>
#include <lib/random.hpp>

template <typename Z>
std::optional<Z> sqrt(Z a) {
        if (a == 0) return 0;
        if (Z::mod() == 2) return 1;

        if (a.pow((Z::mod() - 1) / 2) != 1) return std::nullopt;
        if (Z::mod() % 4 == 3) return a.pow(Z::mod() / 4 + 1);

        i64 q = Z::mod() - 1, n = 0;
        for (; q % 2 == 0; q >>= 1, ++n);

        Z z;
        for (z = MT(); z.pow((Z::mod() - 1) / 2) != Z::mod() - 1; z = MT());

        Z r = a.pow((q + 1) / 2);
        for (Z t = a.pow(q), c = z.pow(q); n > 1; c *= c, --n) {
                if (t.pow(static_cast<i64>(1) << (n - 2)) == 1) continue;

                r *= c;
                t *= c * c;
        }

        return r;
}

template <typename U, typename T, is_unsigned_integral_t<U>* = nullptr, is_integral_t<T>* = nullptr>
std::optional<T> sqrt(T k, U m) {
        if (k % m == 0) return 0;
        if (m == 2) return 1;

        using Z = arbitrary_montgomery_modint_base<U, -1>;
        Z::set_mod(m);

        if (const auto s = sqrt(Z(k)); s.has_value()) return T(s->val());
        return std::nullopt;
}

#endif // LIB_SQRT_HPP

