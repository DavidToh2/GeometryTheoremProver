
#include "Node.hh"
#include "Value2.hh"
#include "Object2.hh"
#include "Common/Exceptions.hh"


void Angle::set_measure(Measure* m, Predicate* base_pred) {
    Angle* root_this = NodeUtils::get_root(this);
    Measure* root_m = NodeUtils::get_root(m);
    if (root_this->__has_measure() && root_this->__get_measure() == root_m) return;
    
    root_this->measure = root_m;
    root_this->measure_why = base_pred;
    root_m->obj2s[this] = base_pred;
    root_m->root_obj2s.insert(root_this);
}
Measure* Angle::__get_measure() {
    Measure* m = NodeUtils::get_root(measure);
    if (m != measure) measure = m;
    return m;
}
Measure* Angle::get_measure() {
    return NodeUtils::get_root(this)->__get_measure();
}
bool Angle::__has_measure() { return (measure != nullptr); }
bool Angle::has_measure() { return NodeUtils::get_root(this)->__has_measure(); }

bool Angle::is_equal(Angle *a1, Angle *a2) {
    Angle* ra1 = NodeUtils::get_root(a1);
    Angle* ra2 = NodeUtils::get_root(a2);
    if (!ra1->has_measure() || !ra2->has_measure()) {
        return false;
    }
    Measure* m1 = ra1->get_measure();
    Measure* m2 = ra2->get_measure();
    return (m1 == m2);
}
bool Angle::is_equal(Angle* a, Frac f) {
    Angle* ra = NodeUtils::get_root(a);
    if (!a->has_measure()) return false;
    return (a->measure->val == f);
}

Generator<std::pair<Line*, Line*>> Angle::all_line_pairs() {
    Angle* root_a = NodeUtils::get_root(this);
    for (Line* l1 : root_a->direction1->root_objs) {
        for (Line* l2 : root_a->direction2->root_objs) {
            co_yield {l1, l2};
        }
    }
    co_return;
}

void Angle::__merge(Angle* other, Predicate* pred) {
    if (this == other) {
        return;
    }
    other->parent = this;
    other->parent_why = pred;
    other->root = this;

    direction1->on_angles_1.erase(other);
    direction2->on_angles_2.erase(other);

    if (other->__has_measure()) {
        other->measure->obj2s.erase(other);
        if (__has_measure()) {
            measure->merge(other->measure, pred);
        } else {
            set_measure(other->measure, pred);
        }
    }
}
void Angle::merge(Angle* other, Predicate* pred) {
    Angle* root_this = NodeUtils::get_root(this);
    Angle* root_other = NodeUtils::get_root(other);
    if (root_this->direction1 != root_other->direction1 || root_this->direction2 != root_other->direction2) {
        throw GGraphInternalError("Error: Cannot merge angles " + root_this->name + " and " + root_other->name + " with different directions.");
    }
    root_this->__merge(root_other, pred);
}