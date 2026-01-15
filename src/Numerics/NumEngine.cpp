
#include "NumEngine.hh"



Numeric* NumEngine::insert_numeric(std::unique_ptr<Numeric> &&num) {
    numerics.emplace_back(std::move(num));
    Numeric* n = numerics.back().get();

    if (!n->is_base_numeric()) {
        for (Point* arg : n->args) {
            for (Point* out : n->outs) {
                depends_on[out].emplace_back(arg);
                depended_by[arg].emplace_back(out);
            }
        }
    }

    return n;
}