
#include <map>
#include <vector>

#include "Node.hh"
#include "Value.hh"
#include "Numerics/Numerics.hh"
#include "Geometry/Object.hh"
#include "Common/Generator.hh"
#include "Common/Utils.hh"

void Point::set_this_on(Circle* c, Predicate* pred) {
    Point* rp = NodeUtils::get_root(this);
    Circle* rc = NodeUtils::get_root(c);
    if (!Utils::isinmap(c, on_circle)) {
        on_circle[c] = std::map<Point*, PredVec>();
    }
    on_circle[c][this].emplace_back(pred);
    rc->points[rp] = pred;
}

void Point::set_this_on(Line* l, Predicate* pred) {
    Point* rp = NodeUtils::get_root(this);
    Line* rl = NodeUtils::get_root(l);
    if (!Utils::isinmap(l, on_line)) {
        on_line[l] = std::map<Point*, PredVec>();
    }
    on_line[l][this].emplace_back(pred);
    rl->points[rp] = pred;
}

bool Point::is_this_on(Line* l) {
    for (const auto& [l0, _] : on_line) {
        if (l0 == l) {
            return true;
        }
    }
    return false;
}

bool Point::is_this_on(Circle* c) {
    for (const auto& [c0, _] : on_circle) {
        if (c0 == c) {
            return true;
        }
    }
    return false;
}

void Point::set_on(Circle* c, Predicate* pred) {
    Point* rp = NodeUtils::get_root(this);
    Circle* rc = NodeUtils::get_root(c);
    if (!Utils::isinmap(c, rp->on_circle)) {
        rp->on_circle[c] = std::map<Point*, PredVec>();
    }
    rp->on_circle[c][this].emplace_back(pred);
    rc->points[rp] = pred;
}

void Point::set_on(Line* l, Predicate* pred) {
    Point* rp = NodeUtils::get_root(this);
    Line* rl = NodeUtils::get_root(l);
    if (!Utils::isinmap(l, rp->on_line)) {
        rp->on_line[l] = std::map<Point*, PredVec>();
    }
    rp->on_line[l][this].emplace_back(pred);
    rl->points[rp] = pred;
}

bool Point::is_on(Line* l) {
    Point* rp = NodeUtils::get_root(this);
    return l->contains(rp);
}

bool Point::is_on(Circle* c) {
    Point* rp = NodeUtils::get_root(this);
    return c->contains(rp);
}

Generator<Line*> Point::on_lines() {
    return NodeUtils::on_roots_dedup<Line>(NodeUtils::get_root(this)->on_line);
}

Generator<Circle*> Point::on_circles() {
    return NodeUtils::on_roots_dedup<Circle>(NodeUtils::get_root(this)->on_circle);
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
}

template <std::derived_from<Object> Key>
void Point::merge_dmaps(std::map<Key*, std::map<Point*, PredVec>> &dest, std::map<Key*, std::map<Point*, PredVec>> &src, Predicate* pred) {
    for (const auto& [obj, _] : src) {
        if (!Utils::isinmap(obj, dest)) {
            dest[obj] = std::map<Point*, PredVec>();
        }
        for (const auto& [pt, _] : src[obj]) {
            src[obj][pt] += pred;
        }
        dest[obj].merge(src[obj]);
        src.erase(obj);
    }
}




bool Line::contains(Point *p) {
    return (points.find(NodeUtils::get_root(p)) != points.end());
}
bool Line::has_direction() {
    return (direction != nullptr);
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
            root_this->points[pt] = pred;
            pt->set_this_on(root_this, pred);
        }
    }

    root_this->points.insert(root_other->points.begin(), root_other->points.end());

    if (root_other->has_direction()) {
        root_this->direction->merge(root_other->direction, pred);
    } else {
        root_this->direction->add_line(root_other, pred);
    }
}


bool Circle::contains(Point *p) {
    return (points.find(NodeUtils::get_root(p)) != points.end());
}