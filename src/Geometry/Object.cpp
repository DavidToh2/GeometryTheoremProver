
#include <map>
#include <vector>

#include "Common/Exceptions.hh"
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

    if (!on_line.contains(l)) {
        on_line[l] = std::map<Point*, Predicate*>();
    }
    if (!on_line[l].contains(this)) {
        on_line[l][this] = pred;
    }
    on_root_line.insert(rl);
    rl->points[this] = pred;
}
void Point::set_this_on(Circle* c, Predicate* pred) {
    Circle* rc = NodeUtils::get_root(c);
    if (on_root_circle.contains(rc)) return;

    if (!on_circle.contains(c)) {
        on_circle[c] = std::map<Point*, Predicate*>();
    }
    if (!on_circle[c].contains(this)) {
        on_circle[c][this] = pred;
    }
    on_root_circle.insert(rc);
    rc->points[this] = pred;
}
void Point::set_this_endpoint_of(Segment* s, Predicate* pred) {
    Segment* rs = NodeUtils::get_root(s);
    if (endpoint_of_root_segment.contains(rs)) return;

    if (!endpoint_of_segment.contains(s)) {
        endpoint_of_segment[s] = std::map<Point*, Predicate*>();
    }
    if (!endpoint_of_segment[s].contains(this)) {
        endpoint_of_segment[s][this] = pred;
    }
    endpoint_of_root_segment.insert(rs);
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
        if (rp->on_line.contains(l)) {
            if (rp->on_line[l].contains(this)) {
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

    for (Line* l : root_other->on_root_line) {
        Utils::replace_key_in_map(l->points, root_other, root_this);
    }
    for (Circle* c : root_other->on_root_circle) {
        Utils::replace_key_in_map(c->points, root_other, root_this);
    }

    Point::merge_dmaps(root_this->on_line, root_other->on_line);
    Point::merge_dmaps(root_this->on_circle, root_other->on_circle);

    // std::set::merge has move semantics
    root_this->on_root_line.merge(root_other->on_root_line);
    root_this->on_root_circle.merge(root_other->on_root_circle);

    Segment::check_segments_with_endpoint(root_this, root_other, pred);
    Point::merge_dmaps(root_this->endpoint_of_segment, root_other->endpoint_of_segment);
}





void Line::set_direction(Direction* d, Predicate* pred) {
    Line* root_this = NodeUtils::get_root(this);
    Direction* root_d = NodeUtils::get_root(d);
    if (root_this->__has_direction()) {
        root_this->direction->merge(root_d, pred);
    } else {
        root_this->direction = root_d;
        root_this->direction_why = pred;
        root_d->objs[this] = pred;
        root_d->root_objs.insert(root_this);
    }
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
        if (!root_this->points.contains(pt)) {
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
        if (!root_this->points.contains(pt)) {
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




void Segment::set_length(Length* l, Predicate* pred) {
    Segment* root_this = NodeUtils::get_root(this);
    Length* root_l = NodeUtils::get_root(l);
    if (root_this->__has_length()) {
        root_this->length->merge(root_l, pred);
    } else {
        root_this->length = root_l;
        root_this->length_why = pred;
        root_l->objs[this] = pred;
        root_l->root_objs.insert(root_this);
    }
}
bool Segment::__has_length() {
    return (length != nullptr);
}
bool Segment::has_length() {
    return NodeUtils::get_root(this)->__has_length();
}
Length* Segment::__get_length() {
    Length* l = NodeUtils::get_root(length);
    if (l != length) length = l;
    return l;
}
Length* Segment::get_length() {
    return NodeUtils::get_root(this)->__get_length();
}

Line* Segment::__get_line() {
    Line* l = NodeUtils::get_root(on_line);
    if (l != on_line) on_line = l;
    return l;
}
Line* Segment::get_line() {
    return NodeUtils::get_root(this->__get_line());
}

Generator<Ratio*> Segment::on_ratios_as_segment1() {
    return this->get_length()->on_ratios_as_length1();
}
Generator<Ratio*> Segment::on_ratios_as_segment2() {
    return this->get_length()->on_ratios_as_length2();
}


void Segment::merge(Segment* other, Predicate* pred) {
    Segment* root_this = NodeUtils::get_root(this);
    Segment* root_other = NodeUtils::get_root(other);
    if (root_this == root_other) {
        return;
    }
    root_other->parent = root_this;
    root_other->parent_why = pred;
    root_other->root = root_this;

    root_other->endpoints[0]->endpoint_of_root_segment.erase(root_other);
    root_other->endpoints[1]->endpoint_of_root_segment.erase(root_other);

    root_this->points.merge(root_other->points);

    root_this->length->merge(root_other->length, pred);
}
void Segment::check_segments_with_endpoint(Point *p, Point *other_p, Predicate *pred) {
    std::map<Point*, Segment*> stp1;
    for (Segment* s : p->endpoint_of_root_segment) {
        Point* p1 = s->other_endpoint(p);
        if (p1 == other_p) {
            throw GGraphInternalError("Segment::check_segments_with_endpoint(): The segment " 
                    + s->name + " has the two endpoints " + p->name + " and " + other_p->name + ", which are being merged.");
        }
        stp1[p1] = s;
    }
    for (auto it = other_p->endpoint_of_root_segment.begin(); it != other_p->endpoint_of_root_segment.end();) {
        Segment* s = *it;
        Point* p2 = s->other_endpoint(other_p);
        if (p2 == p) {
            throw GGraphInternalError("Segment::check_segments_with_endpoint(): The segment " 
                    + s->name + " has the two endpoints " + p->name + " and " + other_p->name + ", which are being merged.");
        }

        // Replace other_p with p in s.endpoints
        if (s->endpoints[0] == other_p) {
            s->endpoints[0] = p;
        } else if (s->endpoints[1] == other_p) {
            s->endpoints[1] = p;
        }

        // If the segments s1 = p-p2 and s = other_p-p2 exist, merge s1 into s
        if (stp1.contains(p2)) {
            Segment* s1 = stp1[p2];
            s->merge(s1, pred);
            p->endpoint_of_root_segment.erase(s1);
            p2->endpoint_of_root_segment.erase(s1);
        }
        
        p->endpoint_of_root_segment.insert(s);
        it = other_p->endpoint_of_root_segment.erase(it);
    }
}
