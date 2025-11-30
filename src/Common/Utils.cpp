
#include <map>
#include <algorithm>
#include <iterator>

#include "Common/Constants.hh"
#include "Common/Utils.hh"


template <typename Key, typename Map>
void Utils::replace_key_in_map(Map& m, Key old_key, Key new_key) {
    auto it = m.find(old_key);
    if (it != m.end()) {
        std::swap(m[new_key], it->second);
        m.erase(old_key);
    }
}

template <typename Map, typename Value>
void Utils::merge_maps(Map& dest, const Map& src, Value& overwrite_value) {
    for (const auto& [key, _] : src) {
        if (!isinmap(key, dest)) {
            dest[key] = overwrite_value;
        }
    }
}

pred_t Utils::to_pred_t(const std::string s) {
    return static_cast<pred_t>(std::distance(
        Constants::PREDICATE_NAMES,
        std::find(
            std::begin(Constants::PREDICATE_NAMES),
            std::end(Constants::PREDICATE_NAMES),
            s
        )
    ));
}

std::string Utils::to_pred_str(pred_t p) {
    return Constants::PREDICATE_NAMES[static_cast<size_t>(p)];
}