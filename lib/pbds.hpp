#ifndef LIB_PBDS_HPP
#define LIB_PBDS_HPP 1

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

#include <lib/hash.hpp>

template <class Key, class Value, class Hash = ::Hash<Key>>
using HashMap = __gnu_pbds::gp_hash_table<Key,
						Value,
						Hash,
						std::equal_to<Key>,
						__gnu_pbds::direct_mask_range_hashing<>,
						__gnu_pbds::linear_probe_fn<>,
						__gnu_pbds::hash_standard_resize_policy<__gnu_pbds::hash_exponential_size_policy<>,
											__gnu_pbds::hash_load_check_resize_trigger<>,
											true>>;

template <class Key, class Hash = ::Hash<Key>>
using HashSet = HashMap<Key, __gnu_pbds::null_type, Hash>;

template <class Key, class Value, class Compare = std::less<Key>>
using IndexedMap = __gnu_pbds::tree<Key, Value, Compare, __gnu_pbds::rb_tree_tag, __gnu_pbds::tree_order_statistics_node_update>;

template <class Key, class Compare = std::less<Key>>
using IndexedSet = IndexedMap<Key, __gnu_pbds::null_type, Compare>;

#endif // LIB_PBDS_HPP
