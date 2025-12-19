
#include "Node.hh"
#include "Object.hh"
#include "Value.hh"
#include "Object2.hh"

template class Value<Line>;
template class Value<Triangle>;

/* Direction methods */

bool Direction::__has_perp() { return (perp != nullptr); }
bool Direction::has_perp() { return NodeUtils::get_root(this)->__has_perp(); }

void Direction::__set_perp(Direction *d, Predicate* pred) {
    this->perp = d;
    this->perp_why = pred;
}
void Direction::set_perp(Direction *d, Predicate* pred) {
    Direction* root_this = NodeUtils::get_root(this);
    Direction* root_d = NodeUtils::get_root(d);

    if (root_this->__has_perp()) {
        if (root_d->__has_perp()) {
            root_this->merge(root_d->perp, pred);
        } else {
            root_d->__set_perp(root_this, pred);
        }
        root_d->merge(root_this->perp, pred);
    } else {
        root_this->__set_perp(root_d, pred);
        if (root_d->__has_perp()) {
            root_this->merge(root_d->perp, pred); 
        } else {
            root_d->__set_perp(root_this, pred);
        }
    }
}

Direction* Direction::__get_perp() {
    return NodeUtils::get_root(perp);
}
Direction* Direction::get_perp() {
    return NodeUtils::get_root(this)->__get_perp();
}

void Direction::add_line(Line* l, Predicate* pred) {
    Direction* root_this = NodeUtils::get_root(this);
    Line* root_l = NodeUtils::get_root(l);
    root_l->set_direction(root_this, pred);
}

Generator<std::pair<Line*, Line*>> Direction::all_para_pairs() {
    Direction* root_this = NodeUtils::get_root(this);
    return NodeUtils::all_pairs<Line>(root_this->root_objs);
}
Generator<std::pair<Line*, Line*>> Direction::all_para_pairs_ordered() {
    Direction* root_this = NodeUtils::get_root(this);
    return NodeUtils::all_pairs_ordered<Line>(root_this->root_objs);
}

Generator<std::pair<Line*, Line*>> Direction::all_perp_pairs() {
    Direction* root_this = NodeUtils::get_root(this);
    if (!root_this->__has_perp()) {
        co_return;
    }
    Direction* perp_dir = NodeUtils::get_root(root_this->perp);
    for (Line* l1 : root_this->root_objs) {
        for (Line* l2 : perp_dir->root_objs) {
            co_yield {l1, l2};
        }
    }
    co_return;
}
Generator<std::pair<Line*, Line*>> Direction::all_perp_pairs_ordered() {
    Direction* root_this = NodeUtils::get_root(this);
    if (!root_this->__has_perp()) {
        co_return;
    }
    Direction* perp_dir = NodeUtils::get_root(root_this->perp);
    for (Line* l1 : root_this->root_objs) {
        for (Line* l2 : perp_dir->root_objs) {
            co_yield {l1, l2};
            co_yield {l2, l1};
        }
    }
    co_return;
}

Generator<Angle*> Direction::on_angles_as_direction1() {
    Direction* root_this = NodeUtils::get_root(this);
    for (Angle* a : root_this->on_angles_1) {
        co_yield a;
    }
    co_return;
}
Generator<Angle*> Direction::on_angles_as_direction2() {
    Direction* root_this = NodeUtils::get_root(this);
    for (Angle* a : root_this->on_angles_2) {
        co_yield a;
    }
    co_return;
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
        root_this->objs[obj] = root_other->objs[obj];
    }
    // std::set::merge has move semantics
    root_this->root_objs.merge(root_other->root_objs);
    root_other->root_objs.clear();

    for (Angle* a : root_other->on_angles_1) {
        a->direction1 = root_this;
    }
    for (Angle* a : root_other->on_angles_2) {
        a->direction2 = root_this;
    }
    root_this->on_angles_1.merge(root_other->on_angles_1);
    root_this->on_angles_2.merge(root_other->on_angles_2);

    root_this->__merge_perps(root_other, pred);
}

void Direction::__merge_perps(Direction* other, Predicate* pred) {
    if (other->__has_perp()) {
        if (__has_perp()) {
            perp->merge(other->perp, pred);
        } else {
            __set_perp(other->__get_perp(), pred);
            perp_why = other->perp_why;
        }
    }
    // No need to populate root_other->perp, as the most up-to-date records of `perp` are always stored by
    // the root node
}