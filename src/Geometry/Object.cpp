
#include <map>
#include <vector>

#include "Node.hh"
#include "Value.hh"
#include "Object.hh"
#include "Object2.hh"
#include "Common/Generator.hh"
#include "Common/Utils.hh"



Generator<Point*> Object::all_points() {
    Object* root_this = NodeUtils::get_root(this);
    for (auto const& it : root_this->points) {
        Point* pbuf = it.first;
        co_yield pbuf;
    }
    co_return;
}
Generator<std::pair<Point*, Point*>> Object::all_point_pairs() {
    Object* root_this = NodeUtils::get_root(this);
    return NodeUtils::all_pairs<Point>(root_this->points);
}
Generator<std::pair<Point*, Point*>> Object::all_point_pairs_ordered() {
    Object* root_this = NodeUtils::get_root(this);
    return NodeUtils::all_pairs_ordered<Point>(root_this->points);
}
Generator<std::tuple<Point*, Point*, Point*>> Object::all_point_triples() {
    Object* root_this = NodeUtils::get_root(this);
    return NodeUtils::all_triples<Point>(root_this->points);
}
Generator<std::tuple<Point*, Point*, Point*>> Object::all_point_triples_ordered() {
    Object* root_this = NodeUtils::get_root(this);
    return NodeUtils::all_triples_ordered<Point>(root_this->points);
}
Generator<std::tuple<Point*, Point*, Point*, Point*>> Object::all_point_quads() {
    Object* root_this = NodeUtils::get_root(this);
    return NodeUtils::all_quads<Point>(root_this->points);
}
Generator<std::tuple<Point*, Point*, Point*, Point*>> Object::all_point_quads_ordered() {
    Object* root_this = NodeUtils::get_root(this);
    return NodeUtils::all_quads_ordered<Point>(root_this->points);
}

bool Object::__contains(Point *p) {
    return (points.find(NodeUtils::get_root(p)) != points.end());
}
bool Object::contains(Point *p) {
    return NodeUtils::get_root(this)->__contains(p);
}

Predicate* Object::__why_contains(Point* p) {
    Point* rp = NodeUtils::get_root(p);
    if (points.contains(rp)) {
        return points[rp];
    }
    return nullptr;
}
Predicate* Object::why_contains(Point *p) {
    return NodeUtils::get_root(this)->__why_contains(p);
}




void Point::set_this_on(Line* l, Predicate* pred) {
    Line* rl = NodeUtils::get_root(l);
    if (on_root_line.contains(rl)) return;

    if (!Utils::isinmap(l, on_line)) {
        on_line[l] = std::map<Point*, Predicate*>();
    }
    if (!Utils::isinmap(this, on_line[l])) {
        on_line[l][this] = pred;
    }
    on_root_line.insert(rl);
    rl->points[this] = pred;
}
void Point::set_this_on(Circle* c, Predicate* pred) {
    Circle* rc = NodeUtils::get_root(c);
    if (on_root_circle.contains(rc)) return;

    if (!Utils::isinmap(c, on_circle)) {
        on_circle[c] = std::map<Point*, Predicate*>();
    }
    if (!Utils::isinmap(this, on_circle[c])) {
        on_circle[c][this] = pred;
    }
    on_root_circle.insert(rc);
    rc->points[this] = pred;
}

bool Point::is_this_on(Line* l) {
    return on_root_line.contains(NodeUtils::get_root(l));
}
bool Point::is_this_on(Circle* c) {
    return on_root_circle.contains(NodeUtils::get_root(c));
}

void Point::set_on(Line* l, Predicate* pred) {
    Point* rp = NodeUtils::get_root(this);
    rp->set_this_on(l, pred);
}
void Point::set_on(Circle* c, Predicate* pred) {
    Point* rp = NodeUtils::get_root(this);
    rp->set_this_on(c, pred);
}

bool Point::is_on(Line* l) {
    Point* rp = NodeUtils::get_root(this);
    return rp->is_this_on(l);
}
bool Point::is_on(Circle* c) {
    Point* rp = NodeUtils::get_root(this);
    return rp->is_this_on(c);
}

Predicate* Point::__why_on(Line* l) {
    Line* rl = NodeUtils::get_root(l);
    Point* rp = NodeUtils::get_root(this);
    if (rp->on_root_line.contains(rl)) {
        if (Utils::isinmap(l, rp->on_line)) {
            if (Utils::isinmap(this, rp->on_line[l])) {
                return rp->on_line[l][this];
            }
        }
    }
    return nullptr;
}
Predicate* Point::why_on(Line* l) {
    Line* rl = NodeUtils::get_root(l);
    Point* rp = NodeUtils::get_root(this);
    return NodeUtils::get_root(this)->__why_on(l);
}

Generator<Line*> Point::on_lines() {
    Point* rp = NodeUtils::get_root(this);
    for (Line* l : rp->on_root_line) {
        co_yield l;
    }
    co_return;
}

Generator<Circle*> Point::on_circles() {
    Point* rp = NodeUtils::get_root(this);
    for (Circle* c : rp->on_root_circle) {
        co_yield c;
    }
    co_return;
}

void Point::merge(Point* other, Predicate* pred) {

    Point* root_this = NodeUtils::get_root(this);
    Point* root_other = NodeUtils::get_root(other);
    if (root_this == root_other) {
        return;
    }
    root_other->parent = root_this;
    root_other->parent_why = pred;
    root_other->root = root_this;

    for (const auto& l : root_other->on_root_line) {
        Utils::replace_key_in_map(l->points, root_other, root_this);
    }
    for (const auto& c : root_other->on_root_circle) {
        Utils::replace_key_in_map(c->points, root_other, root_this);
    }

    Point::merge_dmaps(root_this->on_line, root_other->on_line);
    Point::merge_dmaps(root_this->on_circle, root_other->on_circle);

    // std::set::merge has move semantics
    root_this->on_root_line.merge(root_other->on_root_line);
    root_this->on_root_circle.merge(root_other->on_root_circle);
}





void Line::set_direction(Direction* d, Predicate* base_pred) {
    Line* root_this = NodeUtils::get_root(this);
    Direction* root_d = NodeUtils::get_root(d);
    if (root_this->__has_direction() && root_this->__get_direction() == root_d) return;

    root_this->direction = root_d;
    root_this->direction_why = base_pred;
    root_d->objs[this] = base_pred;
    root_d->root_objs.insert(root_this);
}
bool Line::__has_direction() {
    return (direction != nullptr);
}
bool Line::has_direction() {
    return NodeUtils::get_root(this)->__has_direction();
}
Direction* Line::__get_direction() {
    Direction* d = NodeUtils::get_root(direction);
    if (d != direction) direction = d;
    return d;
}
Direction* Line::get_direction() {
    return NodeUtils::get_root(this->__get_direction());
}

Generator<Angle*> Line::on_angles_as_line1() {
    return this->get_direction()->on_angles_as_direction1();
}
Generator<Angle*> Line::on_angles_as_line2() {
    return this->get_direction()->on_angles_as_direction2();
}

void Line::merge(Line* other, Predicate* pred) {
    Line* root_this = NodeUtils::get_root(this);
    Line* root_other = NodeUtils::get_root(other);
    if (root_this == root_other) {
        return;
    }
    root_other->parent = root_this;
    root_other->parent_why = pred;
    root_other->root = root_this;

    for (const auto& [pt, _] : root_other->points) {
        if (!Utils::isinmap(pt, root_this->points)) {
            pt->on_root_line.erase(root_other);
            pt->set_this_on(root_this, pred);
        }
    }

    if (root_other->__has_direction()) {
        root_other->direction->root_objs.erase(root_other);
        if (root_this->__has_direction()) {
            root_this->direction->merge(root_other->direction, pred);
        } else {
            root_this->set_direction(root_other->direction, pred);
        }
    }
}

bool Line::is_para(Line *l1, Line *l2) {
    l1 = NodeUtils::get_root(l1);
    l2 = NodeUtils::get_root(l2);
    if (!l1->has_direction() || !l2->has_direction()) {
        return false;
    }
    Direction* d1 = l1->get_direction();
    Direction* d2 = l2->get_direction();
    return (d1 == d2);
}

bool Line::is_perp(Line *l1, Line *l2) {
    l1 = NodeUtils::get_root(l1);
    l2 = NodeUtils::get_root(l2);
    if (!l1->has_direction() || !l2->has_direction()) {
        return false;
    }
    Direction* d1 = l1->get_direction();
    Direction* d2 = l2->get_direction();
    if (!d1->has_perp() || !d2->has_perp()) {
        return false;
    }
    return (d1->get_perp() == d2);
}




void Circle::__set_center(Point* p, Predicate* pred) {
    p = NodeUtils::get_root(p);
    if (center) {
        p->merge(center, pred);
        center = p;    // maybe should use PredVec instead?
    } else {
        center = p;
    }
}
void Circle::set_center(Point* p, Predicate* pred) {
    NodeUtils::get_root(this)->__set_center(p, pred);
}
Point* Circle::__get_center() {
    Point* c = NodeUtils::get_root(center);
    if (c != center) center = c;
    return c;
}
Point* Circle::get_center() {
    return NodeUtils::get_root(this)->__get_center();
}

Generator<Circle*> Circle::all_circles_through(Point* p1, Point* p2) {
    p1 = NodeUtils::get_root(p1);
    p2 = NodeUtils::get_root(p2);
    std::set<Circle*> cs = Utils::intersect_sets(p1->on_root_circle, p2->on_root_circle);
    for (Circle* c : cs) {
        co_yield c;
    }
    co_return;
}

void Circle::merge(Circle* other, Predicate* pred) {
    Circle* root_this = NodeUtils::get_root(this);
    Circle* root_other = NodeUtils::get_root(other);
    if (root_this == root_other) {
        return;
    }
    root_other->parent = root_this;
    root_other->parent_why = pred;
    root_other->root = root_this;

    for (const auto& [pt, _] : root_other->points) {
        if (!Utils::isinmap(pt, root_this->points)) {
            pt->on_root_circle.erase(root_other);
            pt->set_this_on(root_this, pred);
        }
    }

    Point* c_this = root_this->__get_center();
    Point* c_other = root_other->__get_center();
    if (c_other) {
        if (c_this) c_this->merge(c_other, pred);
        else root_this->__set_center(c_this, pred);
    }
}
