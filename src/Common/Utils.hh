
#include <algorithm>
#include <iterator>

namespace Utils {

template <typename Range, typename T>
constexpr bool isin(const T& v, const Range& r) {
    using std::begin, std::end;
    return std::find(begin(r), end(r), v) != end(r);
}

template <typename Map, typename Key>
constexpr bool isinmap(const Key& key, const Map& m) {
    return m.find(key) != m.end();
}

} // namespace Utils