
#include "Node.hh"
#include "Value.hh"

void Direction::add_line(Line* l, Predicate* pred) {
    Direction* root_this = NodeUtils::get_root(this);
    Line* root_l = NodeUtils::get_root(l);
    root_this->objs[root_l] = pred;
}

void Direction::merge(Direction* other, Predicate* pred) {
    Direction* root_this = NodeUtils::get_root(this);
    Direction* root_other = NodeUtils::get_root(other);
    if (root_this == root_other) {
        return;
    }
    root_other->root = root_this;
    root_other->parent = root_this;
    root_other->parent_why = pred;
    for (const auto& [obj, p] : root_other->objs) {
        // Todo: Investigate whether other predicate storage behaviours are more preferable
        root_this->objs[obj] = root_other->objs[obj];
    }
}