
#pragma once

#include <map>
#include <algorithm>
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

    template <typename Key, typename Map>
    void replace_key_in_map(Map& m, Key old_key, Key new_key);

    /* Merge the keys from `src` into `dest`. Overwrite every single value from `src` with `overwrite_value`, as 
    long as the key is not already in `dest`. */
    template <typename Map, typename Value>
    void merge_maps(Map& dest, const Map& src, Value& overwrite_value);

    pred_t to_pred_t(const std::string s);
    std::string to_pred_str(pred_t p);

} // namespace Utils