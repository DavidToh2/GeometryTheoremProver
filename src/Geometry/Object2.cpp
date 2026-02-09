
#include "Node.hh"
#include "Value2.hh"
#include "Object2.hh"
#include "Common/Exceptions.hh"


void Angle::set_measure(Measure* m, Predicate* base_pred) {
    Angle* root_this = NodeUtils::get_root(this);
    Measure* root_m = NodeUtils::get_root(m);
    if (root_this->__has_measure()) {
        root_this->measure->merge(root_m, base_pred);
    } else {
        root_this->measure = root_m;
        root_this->measure_why = base_pred;
        root_m->obj2s[this] = base_pred;
        root_m->root_obj2s.insert(root_this);
    }
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
    if (ra1 == ra2) {
        return true;
    }
    if (!ra1->has_measure() || !ra2->has_measure()) {
        return false;
    }
    Measure* m1 = ra1->get_measure();
    Measure* m2 = ra2->get_measure();
    return (m1 == m2);
}
bool Angle::is_equal(Angle* a, Frac f) {
    Angle* ra = NodeUtils::get_root(a);
    if (!ra->has_measure()) return false;
    return (ra->__get_measure()->val == f);
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

std::optional<std::pair<Measure*, Measure*>> Angle::__merge(Angle* other, Predicate* pred) {
    if (this == other) {
        return std::nullopt;
    }
    other->parent = this;
    other->parent_why = pred;
    other->root = this;

    direction1->on_angles_1.erase(other);
    direction2->on_angles_2.erase(other);

    if (other->__has_measure()) {
        other->__get_measure()->root_obj2s.erase(other);
        if (__has_measure()) {
            return {{__get_measure(), other->measure}};
        } else {
            set_measure(other->measure, pred);
        }
    }
    return std::nullopt;
}
std::optional<std::pair<Measure*, Measure*>> Angle::merge(Angle* other, Predicate* pred) {
    Angle* root_this = NodeUtils::get_root(this);
    Angle* root_other = NodeUtils::get_root(other);
    if (!NodeUtils::same_as(root_this->direction1, root_other->direction1) || 
        !NodeUtils::same_as(root_this->direction2, root_other->direction2)) {
        throw GGraphInternalError("Error: Cannot merge angles " + root_this->name + " and " + root_other->name + " with different directions.");
    }
    return root_this->__merge(root_other, pred);
}



void Ratio::set_fraction(Fraction* f, Predicate* base_pred) {
    Ratio* root_this = NodeUtils::get_root(this);
    Fraction* root_f = NodeUtils::get_root(f);
    if (root_this->__has_fraction() && root_this->__get_fraction() == root_f) return;
    
    root_this->fraction = root_f;
    root_this->fraction_why = base_pred;
    root_f->obj2s[this] = base_pred;
    root_f->root_obj2s.insert(root_this);
}
bool Ratio::__has_fraction() {
    return (fraction != nullptr);
}
bool Ratio::has_fraction() {
    return NodeUtils::get_root(this)->__has_fraction();
}
Fraction* Ratio::__get_fraction() {
    Fraction* f = NodeUtils::get_root(fraction);
    if (f != fraction) fraction = f;
    return f;
}
Fraction* Ratio::get_fraction() {
    return NodeUtils::get_root(this)->__get_fraction();
}

bool Ratio::is_equal(Ratio* r1, Ratio* r2) {
    Ratio* rr1 = NodeUtils::get_root(r1);
    Ratio* rr2 = NodeUtils::get_root(r2);
    if (rr1 == rr2) {
        return true;
    }
    if (!rr1->has_fraction() || !rr2->has_fraction()) {
        return false;
    }
    Fraction* f1 = rr1->get_fraction();
    Fraction* f2 = rr2->get_fraction();
    return (f1 == f2);
}
bool Ratio::is_equal(Ratio* r, Frac f) {
    Ratio* rr = NodeUtils::get_root(r);
    if (!rr->has_fraction()) return false;
    return (rr->__get_fraction()->val == f);
}

Generator<std::pair<Segment*, Segment*>> Ratio::all_segment_pairs() {
    Ratio* root_r = NodeUtils::get_root(this);
    for (Segment* s1 : root_r->length1->root_objs) {
        for (Segment* s2 : root_r->length2->root_objs) {
            co_yield {s1, s2};
        }
    }
    co_return;
}

std::optional<std::pair<Fraction*, Fraction*>> Ratio::__merge(Ratio* other, Predicate* pred) {
    if (this == other) {
        return std::nullopt;
    }
    other->parent = this;
    other->parent_why = pred;
    other->root = this;

    length1->on_ratio_1.erase(other);
    length2->on_ratio_2.erase(other);

    if (other->__has_fraction()) {
        other->__get_fraction()->root_obj2s.erase(other);
        if (__has_fraction()) {
            return {{__get_fraction(), other->fraction}};
        } else {
            set_fraction(other->fraction, pred);
        }
    }
    return std::nullopt;
}
std::optional<std::pair<Fraction*, Fraction*>> Ratio::merge(Ratio* other, Predicate* pred) {
    Ratio* root_this = NodeUtils::get_root(this);
    Ratio* root_other = NodeUtils::get_root(other);
    if (!NodeUtils::same_as(root_this->length1, root_other->length1) || 
        !NodeUtils::same_as(root_this->length2, root_other->length2)) {
        throw GGraphInternalError("Error: Cannot merge ratios " + root_this->name + " and " + root_other->name + " with different lengths.");
    }
    return root_this->__merge(root_other, pred);
}



void Dimension::add_triangle(Triangle* t, Predicate* pred) {
    Triangle* root_t = NodeUtils::get_root(t);
    root_triangles[root_t] = pred;
    root_t->dimension = NodeUtils::get_root(this);
    root_t->dimension_why = pred;
}
void Dimension::perm_all_triangles(std::array<int, 3> perm) {
    Dimension* root_d = NodeUtils::get_root(this);
    if (this != root_d) return;

    for (auto& [t, pred] : root_d->root_triangles) {
        t->permute(perm);
    }
    std::array<bool, 3> old_isosceles_mask = isosceles_mask;
    for (int i = 0; i < 3; ++i) {
        isosceles_mask[i] = old_isosceles_mask[perm[i]];
    }
}
void Dimension::set_shape(Shape* s, Predicate* pred) {
    Dimension* root_d = NodeUtils::get_root(this);
    Shape* root_s = NodeUtils::get_root(s);
    root_d->shape = root_s;
    root_d->shape_why = pred;
    root_s->obj2s[root_d] = pred;
    root_s->root_obj2s.insert(root_d);
}
bool Dimension::has_shape() {
    return NodeUtils::get_root(this)->shape != nullptr;
}
Shape* Dimension::__get_shape() {
    Shape* rs = NodeUtils::get_root(shape);
    if (rs != shape) shape = rs;
    return rs;
}
Shape* Dimension::get_shape() {
    return NodeUtils::get_root(this)->__get_shape();
}

void Dimension::set_isosceles(int i1, int i2) {
    isosceles_mask[i1] = true;
    isosceles_mask[i2] = true;
}
void Dimension::set_isosceles_mask(std::array<bool, 3> mask) {
    isosceles_mask = mask;
}
void Dimension::setor_isosceles_mask(std::array<bool, 3> mask) {
    for (int i = 0; i < 3; ++i) {
        isosceles_mask[i] = (isosceles_mask[i] || mask[i]);
    }
}
std::array<bool, 3> Dimension::or_isosceles_masks(std::array<bool, 3> mask1, std::array<bool, 3> mask2) {
    return { (mask1[0] || mask2[0]), (mask1[1] || mask2[1]), (mask1[2] || mask2[2]) };
}

bool Dimension::is_congruent(Dimension* d1, Dimension* d2) {
    Dimension* rd1 = NodeUtils::get_root(d1);
    Dimension* rd2 = NodeUtils::get_root(d2);
    if (rd1 == rd2) {
        return true;
    }
    if (!rd1->has_shape() || !rd2->has_shape()) {
        return false;
    }
    return NodeUtils::same_as(rd1->shape, rd2->shape);
}
Generator<std::pair<Triangle*, Triangle*>> Dimension::all_cong_pairs() {
    Dimension* root_d = NodeUtils::get_root(this);
    return NodeUtils::all_pairs<Triangle>(root_d->root_triangles);
}
Generator<std::pair<Triangle*, Triangle*>> Dimension::all_cong_pairs_ordered() {
    Dimension* root_d = NodeUtils::get_root(this);
    return NodeUtils::all_pairs_ordered<Triangle>(root_d->root_triangles);
}

void Dimension::merge(Dimension* other, Predicate* pred) {
    Dimension* root_this = NodeUtils::get_root(this);
    Dimension* root_other = NodeUtils::get_root(other);
    if (root_this == root_other) {
        return;
    }
    root_other->parent = root_this;
    root_other->parent_why = pred;
    root_other->root = root_this;

    if (root_other->has_shape()) {
        root_other->get_shape()->root_obj2s.erase(root_other);
        if (!root_this->has_shape()) {
            root_this->set_shape(root_other->get_shape(), pred);
        }
    }

    root_this->root_triangles.merge(root_other->root_triangles);
    root_other->root_triangles.clear();
}