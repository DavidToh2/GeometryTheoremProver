
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
    if (!l->has_direction()) {
        l->set_direction(this, pred);
    }
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

std::optional<std::pair<Direction*, Direction*>> Direction::merge(Direction* other, Predicate* pred) {
    Direction* root_this = NodeUtils::get_root(this);
    Direction* root_other = NodeUtils::get_root(other);
    if (root_this == root_other) {
        return std::nullopt;
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

    root_this->on_angles_1.merge(root_other->on_angles_1);
    root_this->on_angles_2.merge(root_other->on_angles_2);

    return root_this->__check_perps_for_merge(root_other, pred);
}

std::optional<std::pair<Direction*, Direction*>> Direction::__check_perps_for_merge(Direction* other, Predicate* pred) {
    if (other->__has_perp()) {
        if (__has_perp()) {
            if (!NodeUtils::same_as(__get_perp(), other->__get_perp())) {
                return {{perp, other->perp}};
            }
        } else {
            __set_perp(other->__get_perp(), pred);
            perp_why = other->perp_why;
        }
    }
    // No need to populate root_other->perp, as the most up-to-date records of `perp` are always stored by
    // the root node
    return std::nullopt;
}

Generator<std::pair<Angle*, Angle*>> Direction::check_incident_angles(Direction* d, Direction* other_d, Predicate* pred) {
    std::map<Direction*, Angle*> dir1_to_angle;
    std::map<Direction*, Angle*> dir2_to_angle;
    for (Angle* a : d->on_angles_1) {
        Direction* d2 = a->direction2;
        if (d != d2) {
            dir2_to_angle[d2] = a;
        }
    }
    for (auto it = other_d->on_angles_1.begin(); it != other_d->on_angles_1.end(); ) {
        Angle* a = *it;
        bool merge_happened = false;
        a->direction1 = d;

        Direction* d2 = a->direction2;
        if (other_d != d2 && dir2_to_angle.contains(d2)) {
            Angle* a1 = dir2_to_angle[d2];
            if (!merge_happened) it = other_d->on_angles_1.erase(it);
            merge_happened = true;
            co_yield {a1, a};
        }
        if (!merge_happened) ++it;
    }
    for (Angle* a : d->on_angles_2) {
        Direction* d1 = a->direction1;
        if (d != d1) {
            dir1_to_angle[d1] = a;
        }
    }
    for (auto it = other_d->on_angles_2.begin(); it != other_d->on_angles_2.end(); ) {
        Angle* a = *it;
        bool merge_happened = false;
        a->direction2 = d;

        Direction* d1 = a->direction1;
        if (other_d != d1 && dir1_to_angle.contains(d1)) {
            Angle* a1 = dir1_to_angle[d1];
            if (!merge_happened) it = other_d->on_angles_2.erase(it);
            merge_happened = true;
            co_yield {a1, a};
        } 
        if (!merge_happened) ++it;
    }
    co_return;
}

bool Direction::is_para(Direction *d1, Direction *d2) {
    return (NodeUtils::get_root(d1) == NodeUtils::get_root(d2));
}
bool Direction::is_perp(Direction* d1, Direction* d2) {
    if (!d1->has_perp()) {
        return false;
    }
    return (d1->get_perp() == NodeUtils::get_root(d2));
}





void Length::add_segment(Segment* s, Predicate* pred) {
    if (!s->__has_length()) {
        s->set_length(this, pred);
    }
}

Generator<std::pair<Segment*, Segment*>> Length::all_cong_pairs() {
    Length* root_this = NodeUtils::get_root(this);
    return NodeUtils::all_pairs<Segment>(root_this->root_objs);
}
Generator<std::pair<Segment*, Segment*>> Length::all_cong_pairs_ordered() {
    Length* root_this = NodeUtils::get_root(this);
    return NodeUtils::all_pairs_ordered<Segment>(root_this->root_objs);
}

Generator<Ratio*> Length::on_ratios_as_length1() {
    Length* root_this = NodeUtils::get_root(this);
    for (Ratio* r : root_this->on_ratio_1) {
        co_yield r;
    }
    co_return;
}
Generator<Ratio*> Length::on_ratios_as_length2() {
    Length* root_this = NodeUtils::get_root(this);
    for (Ratio* r : root_this->on_ratio_2) {
        co_yield r;
    }
    co_return;
}

void Length::merge(Length* other, Predicate* pred) {
    Length* root_this = NodeUtils::get_root(this);
    Length* root_other = NodeUtils::get_root(other);
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

    root_this->on_ratio_1.merge(root_other->on_ratio_1);
    root_this->on_ratio_2.merge(root_other->on_ratio_2);
}

Generator<std::pair<Ratio*, Ratio*>> Length::check_incident_ratios(Length* l, Length* other_l, Predicate* pred) {
    std::map<Length*, Ratio*> len1_to_ratio;
    std::map<Length*, Ratio*> len2_to_ratio;
    for (Ratio* r : l->on_ratio_1) {
        Length* l2 = r->length2;
        if (l != l2) {
            len2_to_ratio[l2] = r;
        }
    }
    for (auto it = other_l->on_ratio_1.begin(); it != other_l->on_ratio_1.end(); ) {
        Ratio* r = *it;
        bool merge_happened = false;
        r->length1 = l;

        Length* l2 = r->length2;
        if (other_l != l2 && len2_to_ratio.contains(l2)) {
            Ratio* r1 = len2_to_ratio[l2];
            if (!merge_happened) it = other_l->on_ratio_1.erase(it);
            merge_happened = true;
            co_yield {r1, r};
        }
        if (!merge_happened) ++it;
    }
    for (Ratio* r : l->on_ratio_2) {
        Length* l1 = r->length1;
        if (l != l1) {
            len1_to_ratio[l1] = r;
        }
    }
    for (auto it = other_l->on_ratio_2.begin(); it != other_l->on_ratio_2.end(); ) {
        Ratio* r = *it;
        bool merge_happened = false;
        r->length2 = l;

        Length* l1 = r->length1;
        if (other_l != l1 && len1_to_ratio.contains(l1)) {
            Ratio* r1 = len1_to_ratio[l1];
            if (!merge_happened) it = other_l->on_ratio_2.erase(it);
            merge_happened = true;
            co_yield {r1, r};
        } 
        if (!merge_happened) ++it;
    }
    co_return;
}