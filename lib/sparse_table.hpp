#ifndef LIB_SPARSE_TABLE_HPP
#define LIB_SPARSE_TABLE_HPP 1

#include <lib/prelude.hpp>
#include <lib/type_traits.hpp>

template <typename Monoid, is_monoid_t<Monoid>* = nullptr>
struct sparse_table {
};

#endif // LIB_SPARSE_TABLE_HPP
