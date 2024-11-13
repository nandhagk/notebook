#ifndef LIB_RANDOM_HPP
#define LIB_RANDOM_HPP 1

#include <chrono>
#include <random>

#include <lib/prelude.hpp>

static const u64 FIXED_RANDOM = std::chrono::steady_clock::now().time_since_epoch().count();
static std::mt19937_64 MT(FIXED_RANDOM);

#endif // LIB_RANDOM_HPP
