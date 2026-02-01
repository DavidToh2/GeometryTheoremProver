
#include <cassert>
#include <iostream>
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
        on_line[l] = std::map<Point*, PredVec>();
    }
    if (!on_line[l].contains(this)) {
        on_line[l][this] += pred;
    }
    on_root_line.insert(rl);
    rl->points[this] = pred;
}
void Point::set_this_on(Circle* c, Predicate* pred) {
    Circle* rc = NodeUtils::get_root(c);
    if (on_root_circle.contains(rc)) return;

    if (!on_circle.contains(c)) {
        on_circle[c] = std::map<Point*, PredVec>();
    }
    if (!on_circle[c].contains(this)) {
        on_circle[c][this] += pred;
    }
    on_root_circle.insert(rc);
    rc->points[this] = pred;
}
void Point::set_this_center_of(Circle* c, Predicate* pred) {
    Circle* rc = NodeUtils::get_root(c);
    if (center_of_root_circle.contains(rc)) return;

    if (!center_of_circle.contains(c)) {
        center_of_circle[c] = std::map<Point*, PredVec>();
    }
    if (!center_of_circle[c].contains(this)) {
        center_of_circle[c][this] += pred;
    }
    center_of_root_circle.insert(rc);
    rc->center = this;
    rc->center_why = pred;
}
void Point::set_this_endpoint_of(Segment* s, Predicate* pred) {
    Segment* rs = NodeUtils::get_root(s);
    if (endpoint_of_root_segment.contains(rs)) return;

    if (!endpoint_of_segment.contains(s)) {
        endpoint_of_segment[s] = std::map<Point*, PredVec>();
    }
    if (!endpoint_of_segment[s].contains(this)) {
        endpoint_of_segment[s][this] += pred;
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

PredVec Point::__why_on(Line* l) {
    Line* rl = NodeUtils::get_root(l);
    Point* rp = NodeUtils::get_root(this);
    if (rp->on_root_line.contains(rl)) {
        if (rp->on_line.contains(l)) {
            if (rp->on_line[l].contains(this)) {
                return rp->on_line[l][this];
            }
        }
    }
    // TODO: Fix this. We should store the PredVecs somewhere else and use pointers to
    // refer to them
    return {};
}
PredVec Point::why_on(Line* l) {
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

Generator<Circle*> Point::center_of_circles() {
    Point* rp = NodeUtils::get_root(this);
    for (Circle* c : rp->center_of_root_circle) {
        co_yield c;
    }
    co_return;
}

Generator<Segment*> Point::endpoint_of_segments() {
    Point* rp = NodeUtils::get_root(this);
    for (Segment* s : rp->endpoint_of_root_segment) {
        co_yield s;
    }
    co_return;
}

Generator<std::pair<Segment*, Segment*>> Point::endpoint_of_segment_pairs() {
    return NodeUtils::all_pairs(NodeUtils::get_root(this)->endpoint_of_root_segment);
}

void Point::merge(Point* other, Predicate* pred) {

    Point* root_this = NodeUtils::get_root(this);
    Point* root_other = NodeUtils::get_root(other);
    root_this->Node::merge(root_other, pred);

    Point::merge_dmaps(root_this->on_line, root_other->on_line, pred);
    Point::merge_dmaps(root_this->on_circle, root_other->on_circle, pred);
    Point::merge_dmaps(root_this->center_of_circle, root_other->center_of_circle, pred);
    Point::merge_dmaps(root_this->endpoint_of_segment, root_other->endpoint_of_segment, pred);

    // std::set::merge has move semantics
    root_this->on_root_line.merge(root_other->on_root_line);
    root_this->on_root_circle.merge(root_other->on_root_circle);
    root_this->center_of_root_circle.merge(root_other->center_of_root_circle);
    root_this->endpoint_of_root_segment.merge(root_other->endpoint_of_root_segment);
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

std::optional<std::pair<Direction*, Direction*>> Line::merge(Line* other, Predicate* pred) {
    Line* root_this = NodeUtils::get_root(this);
    Line* root_other = NodeUtils::get_root(other);
    if (root_this == root_other) {
        return std::nullopt;
    }
    root_this->Node::merge(root_other, pred);

    /* For every point pt in root_other->points, set pt on root_this.
    
    Comment: We have to be careful of a special case here, where l->merge(l_other) is being called
    as a result of a point incidence check, where l->contains(p), l_other->contains(p_other), and p_other
    is being merged into p. In this scenario, the incidence check has already happened, so p_other has
    been replaced with p in l_other->points. 
    
    When we call p->merge(p_other) later on, p_other->on_root_line will be merged into that of p.
    Note that p_other->on_root_line still contains l_other. As a result, p->on_root_line would then
    contain l_other, which is not desirable since l_other is no longer a root.
    
    We thus need to remove l_other from p_other->on_root_line specifically, before entering this function. */
    for (const auto& [pt, _] : root_other->points) {
        pt->on_root_line.erase(root_other);
            
        if (!root_this->points.contains(pt)) {
            pt->set_this_on(root_this, pred);
        }
    }

    if (root_other->__has_direction()) {
        root_other->__get_direction()->root_objs.erase(root_other);
        if (root_this->__has_direction()) {
            return {{root_this->__get_direction(), root_other->__get_direction()}};
        } else {
            root_this->set_direction(root_other->direction, pred);
        }
    }
    return std::nullopt;
}

Generator<std::pair<Point*, std::pair<Line*, Line*>>> 
Line::check_incident_lines(Point *p, Point *other_p, Predicate *pred) {

    /* Note: By the object incidence invariant, it is entirely possible for two distinct lines to simultaneously
    contain two or more point objects which are numerically equivalent. What this means for the point_to_line
    map is that:
    - it is entirely possible for any line l to contain both [p, other_p] simultaneously. We want to disregard
    these lines entirely, as the merger of p and other_p does not provide any information in this case. We thus
    only examine points q != {p, other_p}.
    - in the first part, it is entirely possible that some point q that is numerically equivalent to p ends
    up lying on more than one line of p->on_root_line, so q may theoretically appear multiple times in the
    point_to_line map. However, because q is numerically equivalent to p, we can disregard it, so our approach
    of storing the most recent line for q in point_to_line is safe.
    - we may also find that some line l in both p->on_root_line and other_p->on_root_line contain the same pair
    of points [q1, q2] (which are numerically equivalent), so both point_to_line[q1] and point_to_line[q2] = l.
    However, because GeometricGraph::merge_lines() is idempotent, so returning {l, l2} twice is no biggie. */

    std::map<Point*, Line*> point_to_line;
    for (auto it = p->on_root_line.begin(); it != p->on_root_line.end(); ++it) {
        Line* l1 = *it;
        Utils::replace_key_in_map(l1->points, other_p, p);
        for (auto& [p1, _] : l1->points) {
            if (p1 != p) {
                point_to_line[p1] = l1;
            }
        }
    }
    for (auto it = other_p->on_root_line.begin(); it != other_p->on_root_line.end(); ) {
        Line* l2 = *it;
        bool merge_happened = false;
        Utils::replace_key_in_map(l2->points, other_p, p);
        for (auto& [p1, _] : l2->points) {
            if (point_to_line.contains(p1)) {
                // See the comment in Line::merge() for an explanation of why this line is necessary
                if (!merge_happened) it = other_p->on_root_line.erase(it);
                merge_happened = true;
                co_yield {p1, {point_to_line[p1], l2}};
            }
        }
        if (!merge_happened) ++it;
    }
    co_return;
}

Generator<std::pair<Line*, std::pair<Point*, Point*>>> 
Line::check_incident_lines(Line* l, Line* other_l, Predicate* pred) {

    std::map<Line*, Point*> line_to_point;
    for (auto it = l->points.begin(); it != l->points.end(); ++it) {
        Point* p1 = it->first;
        for (auto it1 = p1->on_root_line.begin(); it1 != p1->on_root_line.end(); ++it1) {
            Line* l1 = *it1;
            if (l1 != l && l1 != other_l) {
                line_to_point[l1] = p1;  
            }
        }
    }
    for (auto it = other_l->points.begin(); it != other_l->points.end(); ++it) {
        Point* p2 = it->first;
        for (auto it2 = p2->on_root_line.begin(); it2 != p2->on_root_line.end(); ++it2) {
            Line* l2 = *it2;
            if (l2 != l && l2 != other_l) {
                if (line_to_point.contains(l2)) {
                    Point* p1 = line_to_point[l2];
                    if (p1 != p2) {
                        co_yield {l2, {p1, p2}};
                    }
                }
            }
        }
    }
    co_return;
}





void Circle::__set_center(Point* p, Predicate* pred) {
    p = NodeUtils::get_root(p);
    center = p;
    center_why = pred;
}
void Circle::set_center(Point* p, Predicate* pred) {
    NodeUtils::get_root(this)->__set_center(p, pred);
}
bool Circle::__has_center() {
    return (center != nullptr);
}
bool Circle::has_center() {
    return NodeUtils::get_root(this)->__has_center();
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

std::optional<std::pair<Point*, Point*>> Circle::merge(Circle* other, Predicate* pred) {
    Circle* root_this = NodeUtils::get_root(this);
    Circle* root_other = NodeUtils::get_root(other);
    if (root_this == root_other) {
        return std::nullopt;
    }
    root_other->parent = root_this;
    root_other->parent_why = pred;
    root_other->root = root_this;

    for (const auto& [pt, _] : root_other->points) {
        pt->on_root_circle.erase(root_other);
        if (!root_this->points.contains(pt)) {
            pt->set_this_on(root_this, pred);
        }
    }

    if (root_other->__has_center()) {
        root_other->__get_center()->center_of_root_circle.erase(root_other);
        if (root_this->__has_center()) {
            return {{root_this->__get_center(), root_other->center}};
        } else {
            root_this->__set_center(root_other->center, pred);
        }
    }
    return std::nullopt;
}

Generator<std::pair<std::pair<Point*, Point*>, std::pair<Circle*, Circle*>>> 
Circle::check_incident_circles_by_intersections(Point *p, Point *other_p, Predicate *pred) {

    std::map<std::pair<Point*, Point*>, Circle*> point_pair_to_circle;
    std::map<Point*, Circle*> center_to_circle;
    for (auto it = p->on_root_circle.begin(); it != p->on_root_circle.end(); ++it) {
        Circle* c1 = *it;
        Utils::replace_key_in_map(c1->points, other_p, p);
        auto gen_point_pairs_on_c1 = c1->all_point_pairs_ordered();
        while (gen_point_pairs_on_c1) {
            auto pair1 = gen_point_pairs_on_c1();
            if (pair1.first == p || pair1.second == p) continue;
            point_pair_to_circle[pair1] = c1;
        }
        if (c1->has_center()) {
            Point* c = c1->get_center();
            center_to_circle[c] = c1;
        }
    }
    for (auto it = other_p->on_root_circle.begin(); it != other_p->on_root_circle.end(); ) {
        Circle* c2 = *it;
        bool merge_happened = false;
        Utils::replace_key_in_map(c2->points, other_p, p);
        auto gen_point_pairs_on_c2 = c2->all_point_pairs();
        while (gen_point_pairs_on_c2) {
            auto pair2 = gen_point_pairs_on_c2();
            if (point_pair_to_circle.contains(pair2)) {
                if (!merge_happened) it = other_p->on_root_circle.erase(it);
                merge_happened = true;
                co_yield {pair2, {point_pair_to_circle[pair2], c2}};
            }
        }
        if (c2->has_center()) {
            Point* c = c2->get_center();
            if (center_to_circle.contains(c)) {
                if (!merge_happened) it = other_p->on_root_circle.erase(it);
                merge_happened = true;
                co_yield {{c, nullptr},{center_to_circle[c], c2}};
            }
        }
        if (!merge_happened) ++it;
    }
    co_return;
}
Generator<std::pair<Point*, std::pair<Circle*, Circle*>>> 
Circle::check_incident_circles_by_center(Point *p, Point *other_p, Predicate *pred) {

    std::map<Point*, Circle*> point_to_circle;
    for (auto it = p->center_of_root_circle.begin(); it != p->center_of_root_circle.end(); ++it) {
        Circle* c1 = *it;
        for (auto [p1, _] : c1->points) {
            assert(!point_to_circle.contains(p1));
            point_to_circle[p1] = c1;
        }
    }
    for (auto it = other_p->center_of_root_circle.begin(); it != other_p->center_of_root_circle.end(); ) {
        Circle* c2 = *it;
        bool merge_happened = false;
        c2->set_center(p, c2->center_why);
        
        for (auto [p1, _] : c2->points) {
            if (point_to_circle.contains(p1)) {
                if (!merge_happened) it = other_p->center_of_root_circle.erase(it);
                merge_happened = true;
                co_yield {p, {point_to_circle[p1], c2}};
            }
        }
        if (!merge_happened) ++it;
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
    return NodeUtils::get_root(this)->__get_line();
}

bool Segment::on_same_line(Segment* s1, Segment* s2) {
    return (NodeUtils::same_as(s1->get_line(), s2->get_line()));
}

Generator<Ratio*> Segment::on_ratios_as_segment1() {
    return this->get_length()->on_ratios_as_length1();
}
Generator<Ratio*> Segment::on_ratios_as_segment2() {
    return this->get_length()->on_ratios_as_length2();
}


std::optional<std::pair<Length*, Length*>> Segment::merge(Segment* other, Predicate* pred) {
    Segment* root_this = NodeUtils::get_root(this);
    Segment* root_other = NodeUtils::get_root(other);
    if (root_this == root_other) {
        return std::nullopt;
    }
    root_other->parent = root_this;
    root_other->parent_why = pred;
    root_other->root = root_this;

    root_other->endpoints[0]->endpoint_of_root_segment.erase(root_other);
    root_other->endpoints[1]->endpoint_of_root_segment.erase(root_other);

    root_this->points.merge(root_other->points);

    if (root_other->__has_length()) {
        root_other->__get_length()->root_objs.erase(root_other);
        if (root_this->__has_length()) {
            return {{root_this->__get_length(), root_other->length}};
        } else {
            root_this->set_length(root_other->length, pred);
        }
    }
    return std::nullopt;
}
Generator<std::pair<Segment*, Segment*>> Segment::check_incident_segments(Point *p, Point *other_p, Predicate *pred) {
    std::map<Point*, Segment*> endpoint_to_segment;
    for (auto it = p->endpoint_of_root_segment.begin(); it != p->endpoint_of_root_segment.end(); ++it) {
        Segment* s = *it;
        Point* p1 = s->other_endpoint(p);
        if (p1 == other_p) {
            throw GGraphInternalError("Segment::check_segments_with_endpoint(): The segment " 
                    + s->name + " has the two endpoints " + p->name + " and " + other_p->name + ", which are being merged.");
        }
        endpoint_to_segment[p1] = s;
    }
    for (auto it = other_p->endpoint_of_root_segment.begin(); it != other_p->endpoint_of_root_segment.end(); ) {
        Segment* s1 = *it;
        bool merge_happened = false;

        Point* p2 = s1->other_endpoint(other_p);

        // Replace other_p with p in s1->endpoints
        if (s1->endpoints[0] == other_p) {
            s1->endpoints[0] = p;
        } else if (s1->endpoints[1] == other_p) {
            s1->endpoints[1] = p;
        }

        if (p2 == p) {
            throw GGraphInternalError("Segment::check_segments_with_endpoint(): The segment " 
                    + s1->name + " has the two endpoints " + p->name + " and " + other_p->name + ", which are being merged.");
        }
        // Check if both the segments s = p-p2 and s1 = other_p-p2 exist
        if (endpoint_to_segment.contains(p2)) {
            assert(!merge_happened);
            if (!merge_happened) it = other_p->endpoint_of_root_segment.erase(it);
            merge_happened = true;
            co_yield {endpoint_to_segment[p2], s1};
        }
        if (!merge_happened) ++it;
    }
    co_return;
}
