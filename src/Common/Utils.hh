
#pragma once

#include <map>
#include <algorithm>
#include <set>
#include <iterator>

#include "Common/Constants.hh"

namespace Utils {

    template <typename Range, typename T>
    constexpr bool isin(const T& v, const Range& r) {
        using std::begin, std::end;
        return std::find(begin(r), end(r), v) != end(r);
    }

    template <typename Key, typename Map>
    constexpr bool isinmap(const Key& key, const Map& m) {
        return m.find(key) != m.end();
    }

    /* If map `m` contains the entry `{old_key: value}`, replace it with `{new_key: value}`. */
    template <typename Key, typename V>
    void replace_key_in_map(std::map<Key, V>& m, Key old_key, Key new_key) {
        auto it = m.find(old_key);
        if (it != m.end()) {
            std::swap(m[new_key], it->second);
            m.erase(old_key);
        }
    }

    /* If set `s` contains the value `old`, replace it with `new`. */
    template <typename T>
    void replace_value_in_set(std::set<T>& s, T old_value, T new_value) {
        s.erase(old_value);
        s.insert(new_value);
    }

    /* Merge the keys from `src` into `dest`. Overwrite every single value from `src` with `overwrite_value`, as 
    long as the key is not already in `dest`. */
    template <typename Map, typename Value>
    void merge_maps(Map& dest, const Map& src, Value& overwrite_value) {
        for (const auto& [key, _] : src) {
            if (!isinmap(key, dest)) {
                dest[key] = overwrite_value;
            }
        }
    }

    pred_t to_pred_t(const std::string s);
    std::string to_pred_str(pred_t p);

} // namespace Utils