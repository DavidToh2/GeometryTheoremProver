
#include "Node.hh"
#include "Object2.hh"
#include "Value2.hh"

void Measure::add_angle(Angle* a, Predicate* pred) {
    Measure* root_this = NodeUtils::get_root(this);
    Angle* root_a = NodeUtils::get_root(a);
    root_a->set_measure(root_this, pred);
}

void Measure::merge(Measure* other, Predicate* pred) {
    Measure* root_this = NodeUtils::get_root(this);
    Measure* root_other = NodeUtils::get_root(other);
    if (root_this == root_other) {
        return;
    }
    root_other->parent = root_this;
    root_other->parent_why = pred;
    root_other->root = root_this;

    for (auto [a, p] : root_other->obj2s) {
        root_this->obj2s[a] = pred;
    }
    // std::set::merge has move semantics
    root_this->root_obj2s.merge(root_other->root_obj2s);
    root_other->root_obj2s.clear();
}

Generator<std::pair<Angle*, Angle*>> Measure::all_eq_pairs() {
    Measure* root_this = NodeUtils::get_root(this);
    return NodeUtils::all_pairs<Angle>(root_this->root_obj2s);
}
Generator<std::pair<Angle*, Angle*>> Measure::all_eq_pairs_ordered() {
    Measure* root_this = NodeUtils::get_root(this);
    return NodeUtils::all_pairs_ordered<Angle>(root_this->root_obj2s);
}



void Fraction::add_ratio(Ratio* r, Predicate* pred) {
    Fraction* root_this = NodeUtils::get_root(this);
    Ratio* root_r = NodeUtils::get_root(r);
    root_r->set_fraction(root_this, pred);
}

void Fraction::merge(Fraction* other, Predicate* pred) {
    Fraction* root_this = NodeUtils::get_root(this);
    Fraction* root_other = NodeUtils::get_root(other);
    if (root_this == root_other) {
        return;
    }
    root_other->parent = root_this;
    root_other->parent_why = pred;
    root_other->root = root_this;

    for (auto [r, p] : root_other->obj2s) {
        root_this->obj2s[r] = pred;
    }
    // std::set::merge has move semantics
    root_this->root_obj2s.merge(root_other->root_obj2s);
    root_other->root_obj2s.clear();
}

Generator<std::pair<Ratio*, Ratio*>> Fraction::all_eq_pairs() {
    Fraction* root_this = NodeUtils::get_root(this);
    return NodeUtils::all_pairs<Ratio>(root_this->root_obj2s);
}
Generator<std::pair<Ratio*, Ratio*>> Fraction::all_eq_pairs_ordered() {
    Fraction* root_this = NodeUtils::get_root(this);
    return NodeUtils::all_pairs_ordered<Ratio>(root_this->root_obj2s);
}