
#include <map>
#include <algorithm>
#include <set>
#include <iterator>

#include "Common/Constants.hh"
#include "Common/Utils.hh"


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