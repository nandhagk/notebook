#ifndef LIB_PBDS_HPP
#define LIB_PBDS_HPP 1

#include <chrono>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

#include <lib/prelude.hpp>

static const u64 FIXED_RANDOM = std::chrono::steady_clock::now().time_since_epoch().count();

template <class T, class D = void>
struct hash {};

template <class T>
inline void hash_combine(u64& seed, const T& v) {
	hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b97f4a7c15 + (seed << 12) + (seed >> 4);
};

template <class T>
struct hash<T, typename std::enable_if<std::is_integral<T>::value>::type> {
	u64 operator()(T _x) const {
		u64 x = _x;
		x += 0x9e3779b97f4a7c15 + FIXED_RANDOM;
		x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
		x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
		return x ^ (x >> 31);
	}
};

template <class T>
struct hash<T, std::void_t<decltype(std::begin(std::declval<T>()))>> {
	u64 operator()(const T& a) const {
		u64 value = FIXED_RANDOM;
		for (const auto& x : a) hash_combine(value, x);
		return value;
	}
};

template <class... T>
struct hash<std::tuple<T...>> {
	u64 operator()(const std::tuple<T...>& a) const {
		u64 value = FIXED_RANDOM;
		std::apply([&value](T const&... args) { (hash_combine(value, args), ...); }, a);
		return value;
	}
};

template <class T, class U>
struct hash<std::pair<T, U>> {
	u64 operator()(const std::pair<T, U>& a) const {
		u64 value = FIXED_RANDOM;
		hash_combine(value, a.first);
		hash_combine(value, a.second);
		return value;
	}
};

template <class Key, class Value, class Hash = hash<Key>>
using HashMap = __gnu_pbds::gp_hash_table<Key,
						Value,
						Hash,
						std::equal_to<Key>,
						__gnu_pbds::direct_mask_range_hashing<>,
						__gnu_pbds::linear_probe_fn<>,
						__gnu_pbds::hash_standard_resize_policy<__gnu_pbds::hash_exponential_size_policy<>,
											__gnu_pbds::hash_load_check_resize_trigger<>,
											true>>;

template <class Key, class Hash = hash<Key>>
using HashSet = HashMap<Key, __gnu_pbds::null_type, Hash>;

template <class Key, class Value, class Compare = std::less<Key>>
using OrderedMap = __gnu_pbds::tree<Key, Value, Compare, __gnu_pbds::rb_tree_tag, __gnu_pbds::tree_order_statistics_node_update>;

template <class Key, class Compare = std::less<Key>>
using OrderedSet = OrderedMap<Key, __gnu_pbds::null_type, Compare>;

#endif // LIB_PBDS_HPP
