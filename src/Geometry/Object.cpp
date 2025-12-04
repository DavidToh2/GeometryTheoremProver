
#include <map>
#include <vector>

#include "Node.hh"
#include "Value.hh"
#include "Numerics/Numerics.hh"
#include "Geometry/Object.hh"
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




void Point::set_this_on(Line* l, Predicate* pred) {
    Point* rp = NodeUtils::get_root(this);
    Line* rl = NodeUtils::get_root(l);
    if (!Utils::isinmap(l, on_line)) {
        on_line[l] = std::map<Point*, PredVec>();
    }
    if (!Utils::isinmap(this, on_line[l])) {
        on_line[l][this] = PredVec();
    }
    on_line[l][this] += pred;
    on_root_line.insert(rl);
    rl->points[rp] = pred;
}
void Point::set_this_on(Circle* c, Predicate* pred) {
    Point* rp = NodeUtils::get_root(this);
    Circle* rc = NodeUtils::get_root(c);
    if (!Utils::isinmap(c, on_circle)) {
        on_circle[c] = std::map<Point*, PredVec>();
    }
    if (!Utils::isinmap(this, on_circle[c])) {
        on_circle[c][this] = PredVec();
    }
    on_circle[c][this] += pred;
    on_root_circle.insert(rc);
    rc->points[rp] = pred;
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

    for (const auto& [l, _] : root_other->on_line) {
        Utils::replace_key_in_map(l->points, root_other, root_this);
    }
    for (const auto& [c, _] : root_other->on_circle) {
        Utils::replace_key_in_map(c->points, root_other, root_this);
    }

    Point::merge_dmaps(root_this->on_line, root_other->on_line, pred);
    Point::merge_dmaps(root_this->on_circle, root_other->on_circle, pred);

    // std::set::merge has move semantics
    root_this->on_root_line.merge(root_other->on_root_line);
    root_this->on_root_circle.merge(root_other->on_root_circle);
}



/* Line methods */

bool Line::__contains(Point *p) {
    return (points.find(NodeUtils::get_root(p)) != points.end());
}
bool Line::contains(Point *p) {
    return NodeUtils::get_root(this)->__contains(p);
}
void Line::set_direction(Direction* d, Predicate* base_pred) {
    Line* root_this = NodeUtils::get_root(this);
    Direction* root_d = NodeUtils::get_root(d);
    root_this->direction = root_d;
    root_this->direction_why = base_pred;
}
bool Line::__has_direction() {
    return (direction != nullptr);
}
bool Line::has_direction() {
    return NodeUtils::get_root(this)->__has_direction();
}
Direction* Line::__get_direction() {
    return NodeUtils::get_root(this)->direction;
}
Direction* Line::get_direction() {
    return NodeUtils::get_root(this->__get_direction());
}

Generator<std::pair<Point*, Point*>> Line::all_point_pairs() {
    Line* root_this = NodeUtils::get_root(this);
    return NodeUtils::all_pairs<Point>(root_this->points);
}

Generator<std::tuple<Point*, Point*, Point*>> Line::all_point_triples() {
    Line* root_this = NodeUtils::get_root(this);
    return NodeUtils::all_triples<Point>(root_this->points);
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

    root_this->points.insert(root_other->points.begin(), root_other->points.end());

    if (root_other->__has_direction()) {
        root_other->direction->root_objs.erase(root_other);
        if (root_this->__has_direction()) {
            root_this->direction->merge(root_other->direction, pred);
        }
    }
}

bool Line::is_para(Line *l1, Line *l2) {
    if (!l1->has_direction() || !l2->has_direction()) {
        return false;
    }
    l1 = NodeUtils::get_root(l1);
    l2 = NodeUtils::get_root(l2);
    Direction* d1 = l1->get_direction();
    Direction* d2 = l2->get_direction();
    return (d1 == d2);
}

bool Line::is_perp(Line *l1, Line *l2) {
    if (!l1->has_direction() || !l2->has_direction()) {
        return false;
    }
    l1 = NodeUtils::get_root(l1);
    l2 = NodeUtils::get_root(l2);
    Direction* d1 = l1->get_direction();
    Direction* d2 = l2->get_direction();
    if (!d1->has_perp() || !d2->has_perp()) {
        return false;
    }
    return (NodeUtils::get_root(d1->perp) == d2);
}

bool Circle::contains(Point *p) {
    return (points.find(NodeUtils::get_root(p)) != points.end());
}