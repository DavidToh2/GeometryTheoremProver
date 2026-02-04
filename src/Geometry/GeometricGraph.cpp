
#include <map>
#include <memory>
#include <iostream>

#include "DD/Predicate.hh"
#include "Geometry/Node.hh"
#include "Object.hh"
#include "Object2.hh"
#include "Value.hh"
#include "Value2.hh"
#include "GeometricGraph.hh"
#include "Common/Exceptions.hh"
#include "Common/Utils.hh"
#include "DD/DDEngine.hh"
#include "AR/AREngine.hh"
#include "Common/Generator.hh"

#include "Common/Debug.hh"
#if DEBUG_GGRAPH
    #define LOG(x) do {std::cout << x << std::endl;} while(0)
#else 
    #define LOG(x)
#endif


void GeometricGraph::initialise_point_numerics(NumEngine &nm) {
    // Fill in the points
    for (Point* p : nm.order_of_resolution) {
        __set_point_numeric(p, nm.get_cartesian(p));
        __identify_num_eq_points(p);
        LOG(p->name << " : " << point_nums.at(p).to_string());
    }
}
void GeometricGraph::__set_point_numeric(Point* p, CartesianPoint cp) {
    point_nums[p] = cp;
}
void GeometricGraph::__identify_num_eq_points(Point* new_p) {
    CartesianPoint cp = point_nums.at(new_p);
    for (const auto& [other_p, other_cp] : point_nums) {
        if (other_p == new_p) continue;
        if (cp == other_cp) {
            int set_num;
            if (point_to_num_eq_set.contains(other_p)) {
                set_num = point_to_num_eq_set.at(other_p);
            } else {
                set_num = num_eq_point_sets.size();
                num_eq_point_sets.emplace_back(std::set<Point*>{other_p});
                point_to_num_eq_set.insert({other_p, set_num});
            }
            num_eq_point_sets[set_num].insert(new_p);
            point_to_num_eq_set.insert({new_p, set_num});
            return;
        }
    }
}



CartesianLine GeometricGraph::compute_line_from_points(Point* p1, Point* p2) {
    return CartesianLine(point_nums.at(p1), point_nums.at(p2));
}
CartesianRay GeometricGraph::compute_ray_from_points(Point* start, Point* head) {
    return CartesianRay(point_nums.at(start), point_nums.at(head));
}
CartesianCircle GeometricGraph::compute_circle_from_points(Point* center, Point* p) {
    return CartesianCircle(point_nums.at(center), Cartesian::distance(point_nums.at(center), point_nums.at(p)));
}
CartesianCircle GeometricGraph::compute_circle_from_points(Point* p1, Point* p2, Point* p3) {
    return CartesianCircle(point_nums.at(p1),point_nums.at(p2),point_nums.at(p3));
}
double GeometricGraph::compute_direction_angle(Direction* d) {
    if (!(d->root_objs.empty())) {
        return Cartesian::angle_of(line_nums.at(*d->root_objs.begin()));
    }
    return 0.0;
}
double GeometricGraph::compute_direction_angle(Line* l) {
    return Cartesian::angle_of(line_nums.at(l));
}






Point* GeometricGraph::__add_new_point(const std::string point_id, CartesianPoint&& coords) {
    if (Utils::isinmap(point_id, points)) {
        throw GGraphInternalError("Error: Point with id " + point_id + " already exists in GeometricGraph.");
    }
    points[point_id] = std::make_unique<Point>(point_id);
    Point* p = points[point_id].get();
    root_points.insert(p);

    point_nums[p] = std::move(coords);
    return p;
}

void GeometricGraph::__try_add_point(const std::string point_id) {
    if (!points.contains(point_id)) {
        points[point_id] = std::make_unique<Point>(point_id);
        root_points.insert(points[point_id].get());
    }
}

Point* GeometricGraph::get_or_add_point(const std::string point_id) {
    __try_add_point(point_id);
    return points[point_id].get();
}

void GeometricGraph::merge_points(Point* dest, Point* src, Predicate* pred, AREngine& ar) {
    dest = NodeUtils::get_root(dest);
    src = NodeUtils::get_root(src);
    if (dest == src) return;
    if (point_nums.at(dest) != point_nums.at(src)) return;
    root_points.erase(src);

    // Check for newly incident Objects
    auto gen_to_merge_lines = Line::check_incident_lines(dest, src, pred);
    while (gen_to_merge_lines) {
        auto [point, lines] = gen_to_merge_lines();
        auto [l1, l2] = lines;
        if (point_nums.at(point) == point_nums.at(dest)) {
            continue;
        }
        merge_lines(l1, l2, pred, ar);
    }
    auto gen_to_merge_circles_1 = Circle::check_incident_circles_by_intersections(dest, src, pred);
    while (gen_to_merge_circles_1) {
        auto [points, circles] = gen_to_merge_circles_1();
        if (points.second == nullptr) {
            // Merge because of common center
        } else {
            // Merge because of two other common points
            Point* p1 = points.first;
            Point* p2 = points.second;
            if (point_nums.at(p1) == point_nums.at(dest) ||
                point_nums.at(p2) == point_nums.at(dest) ||
                point_nums.at(p1) == point_nums.at(p2)) {
                continue;
            }
        }
        merge_circles(circles.first, circles.second, pred, ar);
    }
    auto gen_to_merge_circles_2 = Circle::check_incident_circles_by_center(dest, src, pred);
    while (gen_to_merge_circles_2) {
        // Merge because of one other common point
        auto [point, pair] = gen_to_merge_circles_2();
        merge_circles(pair.first, pair.second, pred, ar);
    }
    auto gen_to_merge_segments = Segment::check_incident_segments(dest, src, pred);
    while (gen_to_merge_segments) {
        auto pair = gen_to_merge_segments();
        merge_segments(pair.first, pair.second, pred);
    }

    // Invariant: After this stage, `src` should not belong to any object's `points` map.

    dest->merge(src, pred);
    ar.update_point_merger(dest, src, pred);
}




Line* GeometricGraph::__add_new_line(Point* p1, Point* p2, Predicate* base_pred) {
    if (p1 == p2) {
        throw GGraphInternalError("Error: Cannot create line with identical endpoints: " + p1->name);
    }
    if (p1->name > p2->name) {
        std::swap(p1, p2);
    }
    std::string line_id = "l_" + p1->name + "_" + p2->name;
    if (Utils::isinmap(line_id, lines)) {
        throw GGraphInternalError("Error: Line with id " + line_id + " already exists in GeometricGraph.");
    }
    lines[line_id] = std::make_unique<Line>(line_id, p1, p2, base_pred);
    Line* l = lines[line_id].get();
    line_nums[l] = compute_line_from_points(p1, p2); // throws NumericsInternalError if line is degenerate

    root_lines.insert(l);
    p1->set_this_on(l, base_pred);
    p2->set_this_on(l, base_pred);

    return l;
}

Line* GeometricGraph::__try_get_line(Point* p1, Point* p2) {
    auto gen = p1->on_lines();
    Line* l = nullptr;
    while (gen) {
        Line* l0 = gen();
        if (l0->contains(p2)) {
            l = l0;
            break;
        }
    }
    return l;
}

Line* GeometricGraph::get_or_add_line(Point* p1, Point* p2, DDEngine &dd) {
    Point* rp1 = NodeUtils::get_root(p1);
    Point* rp2 = NodeUtils::get_root(p2);
    Line* l = __try_get_line(rp1, rp2);
    if (!l) {
        l = __add_new_line(rp1, rp2, dd.base_pred.get());
    }
    return l;
}

Line* GeometricGraph::try_get_line(Point* p1, Point* p2) {
    return __try_get_line(NodeUtils::get_root(p1), NodeUtils::get_root(p2));
}

void GeometricGraph::merge_lines(Line* dest, Line* src, Predicate* pred, AREngine& ar) {
    dest = NodeUtils::get_root(dest);
    src = NodeUtils::get_root(src);
    if (dest == src) return;

    std::set<Line*> L{dest}, L1{src}, L2{};

    // Step 1: Identify all lines which are incident
    while (!L1.empty()) {
        std::string elements_of_L = "";
        for (Line* l : L) {
            elements_of_L += l->name + " ";
        }
        std::string elements_of_L1 = "";
        for (Line* l : L1) {
            elements_of_L1 += l->name + " ";
        }
        std::string elements_of_L2 = "";
        for (auto it = L.begin(); it != L.end(); ++it) {
            for (auto it1 = L1.begin(); it1 != L1.end(); ++it1){
                Line* l1 = *it, *l2 = *it1;
                auto gen = Line::check_incident_lines(l1, l2, pred);
                while (gen) {
                    std::pair<Line*, std::pair<Point*, Point*>> res = gen();
                    if (!L.contains(res.first) && !L1.contains(res.first)) {
                        // Only deal with the case where the two points are numerically distinct
                        if (point_nums.at(res.second.first) != point_nums.at(res.second.second)) {
                            L2.insert(res.first);
                            elements_of_L2 += res.first->name + " (" + (res.second.first->name) + " " + (res.second.second->name) + "), ";
                        }
                    }
                }
            }
        }
        LOG("Merging lines: L={" << elements_of_L << "}, L1={" << elements_of_L1 << "}, L2={" << elements_of_L2 << "}");
        // set::merge() has move semantics, so L2 is emptied
        L.merge(L1);
        L1.merge(L2);
    }

    // Step 2: Merge all identified lines, which now reside in L
    for (auto it = L.begin(); it != L.end(); ++it) {
        Line* l = NodeUtils::get_root(*it);
        if (l == dest) continue;

        root_lines.erase(NodeUtils::get_root(l));
        auto dirs = dest->merge(l, pred);
        ar.update_line_merger(dest, l, pred);

        // Check if the lines have directions that need to be merged
        if (dirs) {
            set_directions_para(dirs->first, dirs->second, pred);
        }
    }
}




Direction* GeometricGraph::__add_new_direction(Line* l, Predicate* base_pred) {
    std::string dir_id = "d_" + l->name;
    if (Utils::isinmap(dir_id, directions)) {
        throw GGraphInternalError("Error: Direction with id " + dir_id + " already exists in GeometricGraph.");
    }
    directions[dir_id] = std::make_unique<Direction>(dir_id);
    Direction* dir = directions[dir_id].get();
    direction_gradients[dir] = compute_direction_angle(l);

    dir->add_line(l, base_pred);
    l->set_direction(dir, base_pred);
    root_directions.insert(dir);

    return dir;
}

Direction* GeometricGraph::get_or_add_direction(Line* l, DDEngine &dd) {
    Line* root_l = NodeUtils::get_root(l);
    if (root_l->has_direction()) {
        return root_l->get_direction();
    }
    return __add_new_direction(root_l, dd.base_pred.get());
}


void GeometricGraph::set_directions_para(Direction* dest, Direction* src, Predicate* pred) {
    dest = NodeUtils::get_root(dest);
    src = NodeUtils::get_root(src);
    if (dest == src) return;
    root_directions.erase(NodeUtils::get_root(src));
    if (src->has_perp()) {
        root_directions.erase(NodeUtils::get_root(src->perp));
    }

    // Check for incident angles as a result of the direction merge
    auto gen_to_merge_angles = Direction::check_incident_angles(dest, src, pred);
    while (gen_to_merge_angles) {
        auto pair = gen_to_merge_angles();
        merge_angles(pair.first, pair.second, pred);
    }
    
    // Check if the directions' perps also need to be merged
    auto perps = dest->merge(src, pred);
    if (perps) {
        set_directions_para(perps->first, perps->second, pred);
    }

    // We do not check for incident lines as a result of the merge. This is
    // because we already have the rule para A B A C => coll B C.
}

void GeometricGraph::set_directions_perp(Direction* d1, Direction* d2, Predicate* pred) {
    d1 = NodeUtils::get_root(d1);
    d2 = NodeUtils::get_root(d2);
    if (d1 == d2) {
        throw GGraphInternalError("Error: Cannot set a direction perpendicular to itself.");
    }
    Direction* dp1 = nullptr, *dp2 = nullptr;
    if (d1->has_perp()) {
        dp1 = d1->get_perp();
    }
    if (d2->has_perp()) {
        dp2 = d2->get_perp();
    }
    d1->set_perp(d2, pred);
    if (dp1) {
        if (!dp1->is_root()) {
            root_directions.erase(dp1);
        }
    }
    if (dp2) {
        if (!dp2->is_root()) {
            root_directions.erase(dp2);
        }
    }
}




Circle* GeometricGraph::__add_new_circle(Point* p1, Point* p2, Point* p3, Predicate* base_pred) {
    if ((p1 == p2) || (p2 == p3) || (p3 == p1)) {
        throw GGraphInternalError("Error: Cannot create circumcircle if triangle is degenerate");
    }
    if (p1->name > p2->name) std::swap(p1, p2);
    if (p2->name > p3->name) std::swap(p2, p3);
    if (p1->name > p2->name) std::swap(p1, p2);
    std::string circle_id = "c_" + p1->name + "_" + p2->name + "_" + p3->name;
    if (Utils::isinmap(circle_id, circles)) {
        throw GGraphInternalError("Error: Circle " + circle_id + " already exists in GeometricGraph.");
    }
    circles[circle_id] = std::make_unique<Circle>(circle_id, p1, p2, p3, base_pred);
    Circle* circ = circles[circle_id].get();
    circle_nums[circ] = compute_circle_from_points(p1, p2, p3); // throws NumericsInternalError if points are collinear or coincide

    root_circles.insert(circ);
    p1->set_this_on(circ, base_pred);
    p2->set_this_on(circ, base_pred);
    p3->set_this_on(circ, base_pred);

    return circ;
}
Circle* GeometricGraph::__add_new_circle(Point* c, Point* p1, Predicate* base_pred) {
    std::string circle_id = "c_" + c->name + "_" + p1->name;
    if (Utils::isinmap(circle_id, circles)) {
        throw GGraphInternalError("Error: Circle " + circle_id + " already exists in GeometricGraph.");
    }
    circles[circle_id] = std::make_unique<Circle>(circle_id, c, p1, base_pred);
    Circle* circ = circles[circle_id].get();
    circle_nums[circ] = compute_circle_from_points(c, p1); // throws NumericsInternalError if c, p1 coincide

    root_circles.insert(circ);
    p1->set_this_on(circ, base_pred);
    c->set_this_center_of(circ, base_pred);

    return circ;
}
Circle* GeometricGraph::__add_new_circle(Point* c, Predicate* base_pred) {
    std::string circle_id = "c_" + c->name;
    if (Utils::isinmap(circle_id, circles)) {
        throw GGraphInternalError("Error: Circle " + circle_id + " already exists in GeometricGraph.");
    }
    circles[circle_id] = std::make_unique<Circle>(circle_id, c, base_pred);
    Circle* circ = circles[circle_id].get();
    root_circles.insert(circ);

    // TODO: No numeric added. Not sure if any is necessary in fact.
    // This constructor is unused

    return circ;
}

Circle* GeometricGraph::__try_get_circle(Point* p1, Point* p2, Point* p3) {
    auto gen = p1->on_circles();
    Circle* circ = nullptr;
    while (gen) {
        Circle* circ0 = gen();
        if (circ0->contains(p2) && circ0->contains(p3)) {
            circ = circ0;
            break;
        }
    }
    return circ;
}
Circle* GeometricGraph::__try_get_circle(Point* c, Point* p1) {
    auto gen = c->center_of_circles();
    Circle* circ = nullptr;
    while (gen) {
        Circle* circ0 = gen();
        if (circ0->contains(p1)) {
            circ = circ0;
            break;
        }
    }
    return circ;
}
std::pair<Circle*, Circle*> GeometricGraph::__try_get_circles(Point* p1, Point* p2, Point* p3, Point* p4) {
    auto gen = p1->on_circles();
    std::pair<Circle*, Circle*> ret = {nullptr, nullptr};
    while (gen) {
        Circle* circ0 = gen();
        if (circ0->contains(p2)) {
            if (ret.first == nullptr) {
                if (circ0->contains(p3)) {
                    ret.first = circ0;
                }
            }
            if (ret.second == nullptr) {
                if (circ0->contains(p4)) {
                    ret.second = circ0;
                }
            }
            if (ret.first) {
                if (ret.second) break;
            }
        }
    }
    return ret;
}
Circle* GeometricGraph::try_get_circle(Point* p1, Point* p2, Point* p3) {
    return __try_get_circle(NodeUtils::get_root(p1), NodeUtils::get_root(p2), NodeUtils::get_root(p3));
}
Circle* GeometricGraph::try_get_circle(Point* c, Point* p1) {
    return __try_get_circle(c, NodeUtils::get_root(p1));
}
std::pair<Circle*, Circle*> GeometricGraph::try_get_circles(Point* p1, Point* p2, Point* p3, Point* p4) {
    return __try_get_circles(NodeUtils::get_root(p1), NodeUtils::get_root(p2), NodeUtils::get_root(p3), NodeUtils::get_root(p4));
}


Circle* GeometricGraph::get_or_add_circle(Point* p1, Point* p2, Point* p3, DDEngine& dd) {
    Point* rp1 = NodeUtils::get_root(p1);
    Point* rp2 = NodeUtils::get_root(p2);
    Point* rp3 = NodeUtils::get_root(p3);
    Circle* circ = __try_get_circle(rp1, rp2, rp3);
    if (!circ) {
        circ = __add_new_circle(rp1, rp2, rp3, dd.base_pred.get());
    }
    return circ;
}
Circle* GeometricGraph::get_or_add_circle(Point* c, Point* p1, DDEngine& dd) {
    Point* rp1 = NodeUtils::get_root(p1);
    Point* rc = NodeUtils::get_root(c);
    Circle* circ = __try_get_circle(rc, rp1);
    if (!circ) {
        circ = __add_new_circle(rc, rp1, dd.base_pred.get());
    }
    return circ;
}

Point* GeometricGraph::get_or_add_circle_center(Circle* c, DDEngine& dd) {
    Point* p = c->get_center();
    if (p) return p;
    std::string p_id = "adhoc_p" + std::to_string(adhoc++);
    points[p_id] = std::make_unique<Point>(p_id);
    p = points[p_id].get();
    c->set_center(p, dd.base_pred.get());
    return p;
}

void GeometricGraph::set_circle_center(Point* cp, Circle* c, Predicate* pred) {
    NodeUtils::get_root(c)->set_center(NodeUtils::get_root(cp), pred);
}
void GeometricGraph::merge_circles(Circle* dest, Circle* src, Predicate* pred, AREngine& ar) {
    dest = NodeUtils::get_root(dest);
    src = NodeUtils::get_root(src);
    if (dest == src) return;
    root_circles.erase(NodeUtils::get_root(src));
    auto p2 = dest->merge(src, pred);
    if (p2) {
        merge_points(p2->first, p2->second, pred, ar);
    }
}





Segment* GeometricGraph::__add_new_segment(Point* p1, Point* p2, Line* l, Predicate* base_pred) {
    if (p1 == p2) {
        throw GGraphInternalError("Error: Cannot create segment with identical endpoints: " + p1->name);
    }
    if (point_nums[p1] > point_nums[p2]) {
        std::swap(p1, p2);
    }
    std::string segment_id = "s_" + p1->name + "_" + p2->name;
    if (Utils::isinmap(segment_id, segments)) {
        throw GGraphInternalError("Error: Segment with id " + segment_id + " already exists in GeometricGraph.");
    }
    segments[segment_id] = std::make_unique<Segment>(segment_id, p1, p2, l, base_pred);
    Segment* s = segments[segment_id].get();

    root_segments.insert(s);
    p1->set_this_endpoint_of(s, base_pred);
    p2->set_this_endpoint_of(s, base_pred);

    return s;
}
Segment* GeometricGraph::__try_get_segment(Point* p1, Point* p2) {
    for (Segment* s : p1->endpoint_of_root_segment) {
        if (s->other_endpoint(p1) == p2) {
            return s;
        }
    }
    return nullptr;
}
Segment* GeometricGraph::get_or_add_segment(Point* p1, Point* p2, DDEngine &dd) {
    Point* rp1 = NodeUtils::get_root(p1);
    Point* rp2 = NodeUtils::get_root(p2);
    Segment* s = __try_get_segment(rp1, rp2);
    if (!s) {
        Line* l = get_or_add_line(rp1, rp2, dd);
        s = __add_new_segment(rp1, rp2, l, dd.base_pred.get());
    }
    return s;
}
Segment* GeometricGraph::try_get_segment(Point* p1, Point* p2) {
    return __try_get_segment(NodeUtils::get_root(p1), NodeUtils::get_root(p2));
}
void GeometricGraph::merge_segments(Segment* dest, Segment* src, Predicate* pred) {
    dest = NodeUtils::get_root(dest);
    src = NodeUtils::get_root(src);
    if (dest == src) return;
    root_segments.erase(src);

    auto l2 = dest->merge(src, pred);
    if (l2) {
        set_lengths_cong(l2->first, l2->second, pred);
    }
}



Length* GeometricGraph::__add_new_length(Segment* s, Predicate* base_pred) {
    std::string length_id = "l_" + s->name;
    if (Utils::isinmap(length_id, lengths)) {
        throw GGraphInternalError("Error: Length with id " + length_id + " already exists in GeometricGraph.");
    }
    lengths[length_id] = std::make_unique<Length>(length_id);
    Length* l = lengths[length_id].get();
    l->add_segment(s, base_pred);
    s->set_length(l, base_pred);
    root_lengths.insert(l);
    return l;
}
Length* GeometricGraph::get_or_add_length(Segment* s, DDEngine &dd) {
    Segment* root_s = NodeUtils::get_root(s);
    if (root_s->has_length()) {
        return root_s->get_length();
    }
    return __add_new_length(root_s, dd.base_pred.get());
}

void GeometricGraph::set_lengths_cong(Segment* s, Segment* s_other, Predicate* pred) {
    Length* l = s->get_length();
    Length* l_other = s_other->get_length();
    set_lengths_cong(l, l_other, pred);
}
void GeometricGraph::set_lengths_cong(Length* l, Length* l_other, Predicate* pred) {
    l = NodeUtils::get_root(l);
    l_other = NodeUtils::get_root(l_other);
    if (l == l_other) return;
    root_lengths.erase(l_other);

    auto gen_to_merge_ratios = Length::check_incident_ratios(l, l_other, pred);
    while (gen_to_merge_ratios) {
        auto pair = gen_to_merge_ratios();
        merge_ratios(pair.first, pair.second, pred);
    }

    l->merge(l_other, pred);
}





Angle* GeometricGraph::__add_new_angle(Direction* d1, Direction* d2, Predicate* base_pred) {
    std::string angle_id = "a_" + d1->name + "_" + d2->name;
    if (Utils::isinmap(angle_id, angles)) {
        throw GGraphInternalError("Error: Angle with id " + angle_id + " already exists in GeometricGraph.");
    }
    angles[angle_id] = std::make_unique<Angle>(angle_id, d1, d2);
    Angle* a = angles[angle_id].get();
    root_angles.insert(a);
    return a;
}

Angle* GeometricGraph::__add_new_angle(Line* l1, Line* l2, Predicate* base_pred) {
    if (!l1->has_direction()) {
        __add_new_direction(l1, base_pred);
    }
    if (!l2->has_direction()) {
        __add_new_direction(l2, base_pred);
    }
    return __add_new_angle(l1->get_direction(), l2->get_direction(), base_pred);
}
Angle* GeometricGraph::__add_new_angle(Point* p1, Point* p2, Point* p3, Point* p4, Predicate* base_pred) {
    Line* l1 = __try_get_line(p1, p2);
    Line* l2 = __try_get_line(p3, p4);
    if (l1 == nullptr || l2 == nullptr) {
        throw GGraphInternalError("Error: Cannot create new angle as lines do not yet exist.");
    }
    return __add_new_angle(l1, l2, base_pred);
}
Angle* GeometricGraph::__add_new_angle(Point* p1, Point* p2, Point* p3, Predicate* base_pred) {
    Line* l1 = __try_get_line(p1, p2);
    Line* l2 = __try_get_line(p2, p3);
    if (l1 == nullptr || l2 == nullptr) {
        throw GGraphInternalError("Error: Cannot create new angle as lines do not yet exist.");
    }
    return __add_new_angle(l1, l2, base_pred);
}

Angle* GeometricGraph::__try_get_angle(Direction* d1, Direction* d2) {
    auto gen = d1->on_angles_as_direction1();
    Angle* angle = nullptr;
    while (gen) {
        Angle* angle0 = gen();
        if (NodeUtils::same_as(angle0->direction2, d2)) {
            angle = angle0;
            break;
        }
    }
    return angle;
}
Angle* GeometricGraph::__try_get_angle(Line* l1, Line* l2) {
    if (!l1->has_direction() || !l2->has_direction()) {
        return nullptr;
    }
    return __try_get_angle(l1->get_direction(), l2->get_direction());
}
Angle* GeometricGraph::__try_get_angle(Point* p1, Point* p2, Point* p3, Point* p4) {
    Line* l1 = __try_get_line(p1, p2);
    Line* l2 = __try_get_line(p3, p4);
    if (l1 && l2) {
        return __try_get_angle(l1, l2);
    }
    return nullptr;
}
Angle* GeometricGraph::__try_get_angle(Point* p1, Point* p2, Point* p3) {
    Line* l1 = __try_get_line(p1, p2);
    Line* l2 = __try_get_line(p2, p3);
    if (l1 && l2) {
        return __try_get_angle(l1, l2);
    }
    return nullptr;
}

Angle* GeometricGraph::try_get_angle(Direction* d1, Direction* d2) {
    return __try_get_angle(d1, d2);
}
Angle* GeometricGraph::try_get_angle(Line* l1, Line* l2) {
    return __try_get_angle(NodeUtils::get_root(l1), NodeUtils::get_root(l2));
}
Angle* GeometricGraph::try_get_angle(Point* p1, Point* p2, Point* p3, Point* p4) {
    Line* l1 = try_get_line(p1, p2);
    Line* l2 = try_get_line(p3, p4);
    if (l1 && l2) {
        return __try_get_angle(l1, l2);
    }
    return nullptr;
}
Angle* GeometricGraph::try_get_angle(Point* p1, Point* p2, Point* p3) {
    Line* l1 = try_get_line(p1, p2);
    Line* l2 = try_get_line(p2, p3);
    if (l1 && l2) {
        return __try_get_angle(l1, l2);
    }
    return nullptr;
}

Angle* GeometricGraph::get_or_add_angle(Direction* d1, Direction* d2, DDEngine& dd) {
    Direction* rd1 = NodeUtils::get_root(d1);
    Direction* rd2 = NodeUtils::get_root(d2);
    Angle* a = __try_get_angle(rd1, rd2);
    if (!a) {
        a = __add_new_angle(rd1, rd2, dd.base_pred.get());
    }
    return a;
}
Angle* GeometricGraph::get_or_add_angle(Line* l1, Line* l2, DDEngine& dd) {
    Line* rl1 = NodeUtils::get_root(l1);
    Line* rl2 = NodeUtils::get_root(l2);
    Angle* a = __try_get_angle(rl1, rl2);
    if (!a) {
        a = __add_new_angle(rl1, rl2, dd.base_pred.get());
    }
    return a;
}
Angle* GeometricGraph::get_or_add_angle(Point* p1, Point* p2, Point* p3, Point* p4, DDEngine& dd) {
    Line* l1 = get_or_add_line(p1, p2, dd);
    Line* l2 = get_or_add_line(p3, p4, dd);
    Angle* a = __try_get_angle(l1, l2);
    if (!a) {
        a = __add_new_angle(l1, l2, dd.base_pred.get());
    }
    return a;
}
Angle* GeometricGraph::get_or_add_angle(Point* p1, Point* p2, Point* p3, DDEngine& dd) {
    Line* l1 = get_or_add_line(p1, p2, dd);
    Line* l2 = get_or_add_line(p2, p3, dd);
    Angle* a = __try_get_angle(l1, l2);
    if (!a) {
        a = __add_new_angle(l1, l2, dd.base_pred.get());
    }
    return a;
}


void GeometricGraph::merge_angles(Angle* dest, Angle* src, Predicate* pred) {
    dest = NodeUtils::get_root(dest);
    src = NodeUtils::get_root(src);
    if (dest == src) return;
    root_angles.erase(src);

    auto ms = dest->merge(src, pred);
    if (ms) {
        set_measures_equal(ms->first, ms->second, pred);
    }
}




Measure* GeometricGraph::__add_new_measure(Angle* a, Predicate* base_pred) {
    std::string measure_id = "m_" + a->name;
    if (Utils::isinmap(measure_id, measures)) {
        throw GGraphInternalError("Error: Measure with id " + measure_id + " already exists in GeometricGraph.");
    }
    measures[measure_id] = std::make_unique<Measure>(measure_id);
    Measure* m = measures[measure_id].get();
    a->set_measure(m, base_pred);
    root_measures.insert(m);
    return m;
}
Measure* GeometricGraph::get_or_add_measure(Angle* a, DDEngine& dd) {
    Angle* ra = NodeUtils::get_root(a);
    if (ra->__has_measure()) {
        return ra->__get_measure();
    }
    return __add_new_measure(ra, dd.base_pred.get());
}


void GeometricGraph::set_measures_equal(Measure* m1, Measure* m2, Predicate* pred) {
    m1 = NodeUtils::get_root(m1);
    m2 = NodeUtils::get_root(m2);
    if (m1 == m2) return;
    root_measures.erase(m2);

    m1->merge(m2, pred);
}

void GeometricGraph::set_measure_val(Measure* m, Frac f, Predicate* pred) {
    if (m->has_val()) {
        if (m->val == f) {
            return;
        } else {
            throw GGraphInternalError("GeometricGraph::set_measure_val(): Contradictory measure value assignment attempted.");
        }
    }
    m->val = f;
    m->val_why = pred;
    if (root_measure_vals.contains(f)) {
        set_measures_equal(root_measure_vals[f], m, pred);
    } else {
        root_measure_vals[f] = m;
    }
}




Ratio* GeometricGraph::__add_new_ratio(Length* l1, Length* l2, Predicate* base_pred) {
    std::string ratio_id = "r_" + l1->name + "_" + l2->name;
    if (Utils::isinmap(ratio_id, ratios)) {
        throw GGraphInternalError("Error: Ratio with id " + ratio_id + " already exists in GeometricGraph.");
    }
    ratios[ratio_id] = std::make_unique<Ratio>(ratio_id, l1, l2);
    Ratio* r = ratios[ratio_id].get();
    root_ratios.insert(r);
    return r;
}
Ratio* GeometricGraph::__add_new_ratio(Segment* s1, Segment* s2, Predicate* base_pred) {
    if (!s1->has_length()) {
        __add_new_length(s1, base_pred);
    }
    if (!s2->has_length()) {
        __add_new_length(s2, base_pred);
    }
    return __add_new_ratio(s1->get_length(), s2->get_length(), base_pred);
}


Ratio* GeometricGraph::__try_get_ratio(Length* l1, Length* l2) {
    auto gen = l1->on_ratios_as_length1();
    Ratio* ratio = nullptr;
    while (gen) {
        Ratio* ratio0 = gen();
        if (NodeUtils::same_as(ratio0->length2, l2)) {
            ratio = ratio0;
            break;
        }
    }
    return ratio;
}
Ratio* GeometricGraph::__try_get_ratio(Segment* s1, Segment* s2) {
    if (!s1->has_length() || !s2->has_length()) {
        return nullptr;
    }
    return __try_get_ratio(s1->get_length(), s2->get_length());
}
Ratio* GeometricGraph::__try_get_ratio(Point* p1, Point* p2, Point* p3, Point* p4) {
    Segment* s1 = __try_get_segment(p1, p2);
    Segment* s2 = __try_get_segment(p3, p4);
    if (s1 && s2) {
        return __try_get_ratio(s1, s2);;
    }
    return nullptr;
}

Ratio* GeometricGraph::try_get_ratio(Length* l1, Length* l2) {
    return __try_get_ratio(NodeUtils::get_root(l1), NodeUtils::get_root(l2));
}
Ratio* GeometricGraph::try_get_ratio(Segment* s1, Segment* s2) {
    return __try_get_ratio(NodeUtils::get_root(s1), NodeUtils::get_root(s2));
}
Ratio* GeometricGraph::try_get_ratio(Point* p1, Point* p2, Point* p3, Point* p4) {
    Segment* s1 = try_get_segment(p1, p2);
    Segment* s2 = try_get_segment(p3, p4);
    if (s1 && s2) {
        return __try_get_ratio(s1, s2);;
    }
    return nullptr;
}

Ratio* GeometricGraph::get_or_add_ratio(Length* l1, Length* l2, DDEngine& dd) {
    Length* rl1 = NodeUtils::get_root(l1);
    Length* rl2 = NodeUtils::get_root(l2);
    Ratio* r = __try_get_ratio(rl1, rl2);
    if (!r) {
        r = __add_new_ratio(rl1, rl2, dd.base_pred.get());
    }
    return r;
}
Ratio* GeometricGraph::get_or_add_ratio(Segment* s1, Segment* s2, DDEngine& dd) {
    Segment* rs1 = NodeUtils::get_root(s1);
    Segment* rs2 = NodeUtils::get_root(s2);
    Ratio* r = __try_get_ratio(rs1, rs2);
    if (!r) {
        r = __add_new_ratio(rs1, rs2, dd.base_pred.get());
    }
    return r;
}
Ratio* GeometricGraph::get_or_add_ratio(Point* p1, Point* p2, Point* p3, Point* p4, DDEngine& dd) {
    Segment* s1 = get_or_add_segment(p1, p2, dd);
    Segment* s2 = get_or_add_segment(p3, p4, dd);
    Ratio* r = __try_get_ratio(s1, s2);
    if (!r) {
        r = __add_new_ratio(s1, s2, dd.base_pred.get());
    }
    return r;
}

void GeometricGraph::merge_ratios(Ratio* dest, Ratio* src, Predicate* pred) {
    dest = NodeUtils::get_root(dest);
    src = NodeUtils::get_root(src);
    if (dest == src) return;
    root_ratios.erase(src);

    auto fracs = dest->merge(src, pred);
    if (fracs) {
        set_fractions_equal(fracs->first, fracs->second, pred);
    }
}




Fraction* GeometricGraph::__add_new_fraction(Ratio* r, Predicate* base_pred) {
    std::string fraction_id = "f_" + r->name;
    if (Utils::isinmap(fraction_id, fractions)) {
        throw GGraphInternalError("Error: Fraction with id " + fraction_id + " already exists in GeometricGraph.");
    }
    fractions[fraction_id] = std::make_unique<Fraction>(fraction_id);
    Fraction* f = fractions[fraction_id].get();
    r->set_fraction(f, base_pred);
    root_fractions.insert(f);
    return f;
}

Fraction* GeometricGraph::get_or_add_fraction(Ratio* r, DDEngine& dd) {
    Ratio* rr = NodeUtils::get_root(r);
    if (rr->__has_fraction()) {
        return rr->__get_fraction();
    }
    return __add_new_fraction(rr, dd.base_pred.get());
}

void GeometricGraph::set_fractions_equal(Fraction* f1, Fraction* f2, Predicate* pred) {
    f1 = NodeUtils::get_root(f1);
    f2 = NodeUtils::get_root(f2);
    if (f1 == f2) return;
    root_fractions.erase(f2);

    f1->merge(f2, pred);
}

void GeometricGraph::set_fraction_val(Fraction* f, Frac val, Predicate* pred) {
    if (f->has_val()) {
        if (f->val == val) {
            return;
        } else {
            throw GGraphInternalError("GeometricGraph::set_fraction_val(): Contradictory fraction value assignment attempted.");
        }
    }
    f->val = val;
    f->val_why = pred;
    if (root_fraction_vals.contains(val)) {
        set_fractions_equal(root_fraction_vals[val], f, pred);
    } else {
        root_fraction_vals[val] = f;
    }
}




bool GeometricGraph::check_coll(Point* p1, Point* p2, Point* p3) {
    Line* l = try_get_line(p1, p2);
    if (!l) { return false; }
    return l->contains(p3);
}

bool GeometricGraph::check_coll(Point* p1, Line* l) { return l->contains(p1); }

bool GeometricGraph::check_coll(Segment* s1, Segment* s2) { return Segment::on_same_line(s1, s2); }

bool GeometricGraph::check_cyclic(Point* p1, Point* p2, Point* p3, Point* p4) {
    Circle* c = try_get_circle(p1, p2, p3);
    if (!c) { return false; }
    return c->contains(p4);
}

bool GeometricGraph::check_cyclic(Point* p1, Circle* c) { return c->contains(p1); }

bool GeometricGraph::check_para(Point* p1, Point* p2, Point* p3, Point* p4) {
    Line* p1p2 = try_get_line(p1, p2);
    if (!p1p2) { return false; }
    return check_para(p3, p4, p1p2);
}

bool GeometricGraph::check_para(Point* p1, Point* p2, Line* l1) {
    Line* p1p2 = try_get_line(p1, p2);
    if (!p1p2) { return false; }
    return check_para(p1p2, l1);
}

bool GeometricGraph::check_para(Line* l1, Line* l2) { return Line::is_para(l1, l2); }

bool GeometricGraph::check_para(Direction* d1, Direction* d2) { return Direction::is_para(d1, d2); }

bool GeometricGraph::check_perp(Point* p1, Point* p2, Point* p3, Point* p4) {
    Line* p1p2 = try_get_line(p1, p2);
    if (!p1p2) { return false; }
    return check_perp(p3, p4, p1p2);
}

bool GeometricGraph::check_perp(Point* p1, Point* p2, Line* l1) {
    Line* p1p2 = try_get_line(p1, p2);
    if (!p1p2) { return false; }
    return check_perp(p1p2, l1);
}

bool GeometricGraph::check_perp(Line* l1, Line* l2) { return Line::is_perp(l1, l2); }

bool GeometricGraph::check_perp(Direction* d1, Direction* d2) { return Direction::is_perp(d1, d2); }

bool GeometricGraph::check_cong(Point* p1, Point* p2, Point* p3, Point* p4) {
    Segment* s1 = try_get_segment(p1, p2);
    Segment* s2 = try_get_segment(p3, p4);
    if (s1 == nullptr || s2 == nullptr) { return false; }
    return check_cong(s1, s2);
}

bool GeometricGraph::check_cong(Segment* s1, Segment* s2) {
    if (!s1->has_length() || !s2->has_length()) { return false; }
    return check_cong(s1->get_length(), s2->get_length());
}

bool GeometricGraph::check_cong(Length* l1, Length* l2) { return NodeUtils::same_as(l1, l2); }


bool GeometricGraph::check_eqangle(Point* p1, Point* p2, Point* p3, Point* p4,
                                      Point* p5, Point* p6, Point* p7, Point* p8) {
    Angle* a1 = try_get_angle(p1, p2, p3, p4);
    Angle* a2 = try_get_angle(p5, p6, p7, p8);
    if (a1 == nullptr || a2 == nullptr) { return false; }
    return Angle::is_equal(a1, a2);
}
bool GeometricGraph::check_eqangle(Point* p1, Point* p2, Point* p3,
                                      Point* p4, Point* p5, Point* p6) {
    Angle* a1 = try_get_angle(p1, p2, p3);
    Angle* a2 = try_get_angle(p4, p5, p6);
    if (a1 == nullptr || a2 == nullptr) { return false; }
    return Angle::is_equal(a1, a2);
}
bool GeometricGraph::check_eqangle(Line* l1a, Line* l1b, Line* l2a, Line* l2b) {
    Angle* a1 = try_get_angle(l1a, l1b);
    Angle* a2 = try_get_angle(l2a, l2b);
    if (a1 == nullptr || a2 == nullptr) { return false; }
    return Angle::is_equal(a1, a2);
}
bool GeometricGraph::check_eqangle(Angle* a1, Angle* a2) { return Angle::is_equal(a1, a2); }


bool GeometricGraph::check_eqratio(Point* p1, Point* p2, Point* p3, Point* p4,
                                      Point* p5, Point* p6, Point* p7, Point* p8) {
    Ratio* r1 = try_get_ratio(p1, p2, p3, p4);
    Ratio* r2 = try_get_ratio(p5, p6, p7, p8);
    if (r1 == nullptr || r2 == nullptr) { return false; }
    return Ratio::is_equal(r1, r2);
}
bool GeometricGraph::check_eqratio(Length* l1a, Length* l1b, Length* l2a, Length* l2b) {
    Ratio* r1 = try_get_ratio(l1a, l1b);
    Ratio* r2 = try_get_ratio(l2a, l2b);
    if (r1 == nullptr || r2 == nullptr) { return false; }
    return Ratio::is_equal(r1, r2);
}
bool GeometricGraph::check_eqratio(Ratio* r1, Ratio* r2) { return Ratio::is_equal(r1, r2); }


bool GeometricGraph::check_midp(Point* m, Point* p1, Point* p2) {
    Segment* s1 = try_get_segment(p1, m);
    Segment* s2 = try_get_segment(m, p2);
    if (s1 == nullptr || s2 == nullptr) { return false; }
    return (!NodeUtils::same_as(s1, s2) && check_coll(s1, s2) && check_cong(s1, s2));
}


bool GeometricGraph::check_circle(Point* c, Point* p1, Point* p2, Point* p3) {
    Circle* circ = try_get_circle(c, p1);
    if (circ) return (circ->contains(p2) && circ->contains(p3));
    circ = try_get_circle(c, p2);
    if (circ) return (circ->contains(p1) && circ->contains(p3));
    circ = try_get_circle(c, p3);
    if (circ) return (circ->contains(p1) && circ->contains(p2));
    circ = try_get_circle(p1, p2, p3);
    if (circ) return check_circle(c, circ);
    return false;
}
bool GeometricGraph::check_circle(Point* c, Circle* circ) {
    return (circ->has_center() && NodeUtils::same_as(c, circ->get_center()));
}


bool GeometricGraph::check_constangle(Angle* a, Frac f) { return Angle::is_equal(a, f); }


bool GeometricGraph::check_constratio(Ratio* r, Frac f) { return Ratio::is_equal(r, f); }


bool GeometricGraph::check_diff(std::set<Point*> &pts) {
    std::set<int> s;
    for (Point* p : pts) {
        if (point_to_num_eq_set.contains(p) && !s.insert(point_to_num_eq_set[p]).second) {
            return false;
        }
    }
    return true;
}


bool GeometricGraph::check_ncoll(std::set<Point*> &pts) {
    for (auto it1 = pts.begin(); it1 != pts.end(); ++it1) {
        for (auto it2 = std::next(it1); it2 != pts.end(); ++it2) {
            for (auto it3 = std::next(it2); it3 != pts.end(); ++it3) {
                if (Cartesian::is_coll(
                    point_nums[*it1],
                    point_nums[*it2],
                    point_nums[*it3]
                )) {
                    return false;
                }
            }
        }
    }
    return true;
}


bool GeometricGraph::check(PredicateTemplate* pred) {
    
    if (!pred->args_filled()) return false;

    switch(pred->name) {
        case pred_t::COLL:
            return check_coll(pred->get_arg_point(0),
                              pred->get_arg_point(1),
                              pred->get_arg_point(2));
            break;
        case pred_t::CYCLIC:
            return check_cyclic(pred->get_arg_point(0),
                                pred->get_arg_point(1),
                                pred->get_arg_point(2),
                                pred->get_arg_point(3));
            break;
        case pred_t::PARA:
            return check_para(pred->get_arg_point(0),
                              pred->get_arg_point(1),
                              pred->get_arg_point(2),
                              pred->get_arg_point(3));
            break;
        case pred_t::PERP:
            return check_perp(pred->get_arg_point(0),
                              pred->get_arg_point(1),
                              pred->get_arg_point(2),
                              pred->get_arg_point(3));
            break;
        case pred_t::CONG:
            return check_cong(pred->get_arg_point(0),
                              pred->get_arg_point(1),
                              pred->get_arg_point(2),
                              pred->get_arg_point(3));
            break;
        case pred_t::EQANGLE:
            return check_eqangle(pred->get_arg_point(0),
                                pred->get_arg_point(1),
                                pred->get_arg_point(2),
                                pred->get_arg_point(3),
                                pred->get_arg_point(4),
                                pred->get_arg_point(5),
                                pred->get_arg_point(6),
                                pred->get_arg_point(7));
            break;
        case pred_t::EQRATIO:
            return check_eqratio(pred->get_arg_point(0),
                                 pred->get_arg_point(1),
                                 pred->get_arg_point(2),
                                 pred->get_arg_point(3),
                                 pred->get_arg_point(4),
                                 pred->get_arg_point(5),
                                 pred->get_arg_point(6),
                                 pred->get_arg_point(7));
            break;
        case pred_t::MIDP:
            return check_midp(pred->get_arg_point(0),
                              pred->get_arg_point(1),
                              pred->get_arg_point(2));
            break;
        case pred_t::CIRCLE:
            return check_circle(pred->get_arg_point(0),
                                pred->get_arg_point(1),
                                pred->get_arg_point(2),
                                pred->get_arg_point(3));
            break;
        default:
            return false;
    }
}




bool GeometricGraph::make_coll(Predicate* pred, DDEngine &dd, AREngine &ar) {
    Point* p1 = static_cast<Point*>(pred->args[0]);
    Point* p2 = static_cast<Point*>(pred->args[1]);
    Point* p3 = static_cast<Point*>(pred->args[2]);

    auto [rp1, rp2, rp3] = NodeUtils::get_roots<Point, 3>({p1, p2, p3});

    if (check_coll(rp1, rp2, rp3)) return false;

    Line* p1p2 = __try_get_line(rp1, rp2);
    Line* p2p3 = __try_get_line(rp2, rp3);
    Line* p3p1 = __try_get_line(rp3, rp1);
    Point* tp = rp3;
    Line* l = p1p2;

    if (p2p3) {
        tp = rp1;
        l = p2p3;
    } else if (p3p1) {
        tp = rp2;
        l = p3p1;
    } else if (p1p2 == nullptr) {
        tp = rp3;
        l = __add_new_line(rp1, rp2, dd.base_pred.get());
    }

    tp->set_this_on(l, pred);

    if (p2p3) {
        if (p3p1) merge_lines(p2p3, p3p1, pred, ar);
        if (p1p2) merge_lines(p2p3, p1p2, pred, ar);
    } else if (p3p1) {
        if (p1p2) merge_lines(p3p1, p1p2, pred, ar);
    }
    return true;
}

bool GeometricGraph::make_cyclic(Predicate* pred, DDEngine &dd, AREngine &ar) {
    Point* p1 = static_cast<Point*>(pred->args[0]);
    Point* p2 = static_cast<Point*>(pred->args[1]);
    Point* p3 = static_cast<Point*>(pred->args[2]);
    Point* p4 = static_cast<Point*>(pred->args[3]);

    auto [rp1, rp2, rp3, rp4] = NodeUtils::get_roots<Point, 4>({p1, p2, p3, p4});

    if (check_cyclic(rp1, rp2, rp3, rp4)) return false;

    auto [c123, c412] = __try_get_circles(rp1, rp2, rp3, rp4);
    auto [c341, c234] = __try_get_circles(rp3, rp4, rp1, rp2);

    Point* tp = rp4;
    Circle* c = c123;

    if (c234) {
        tp = rp1;
        c = c234;
    } else if (c341) {
        tp = rp2;
        c = c341;
    } else if (c412) {
        tp = rp3;
        c = c412;
    } else if (c123 == nullptr) {
        tp = rp4;
        c = __add_new_circle(rp1, rp2, rp3, dd.base_pred.get());
    }

    tp->set_this_on(c, pred);

    if (c234) {
        if (c341) merge_circles(c234, c341, pred, ar);
        if (c412) merge_circles(c234, c412, pred, ar);
        if (c123) merge_circles(c234, c123, pred, ar);
    } else if (c341) {
        if (c412) merge_circles(c341, c412, pred, ar);
        if (c123) merge_circles(c341, c123, pred, ar);
    } else if (c412) {
        if (c123) merge_circles(c412, c123, pred, ar);
    }
    return true;
}

bool GeometricGraph::make_para(Predicate* pred, DDEngine &dd, AREngine &ar) {
    Point* p1 = static_cast<Point*>(pred->args[0]);
    Point* p2 = static_cast<Point*>(pred->args[1]);
    Point* p3 = static_cast<Point*>(pred->args[2]);
    Point* p4 = static_cast<Point*>(pred->args[3]);

    if (check_para(p1, p2, p3, p4)) return false;

    Line* p1p2 = get_or_add_line(p1, p2, dd);
    Line* p3p4 = get_or_add_line(p3, p4, dd);

    Direction* d12 = get_or_add_direction(p1p2, dd);
    if (p3p4->has_direction()) {
        Direction* d34 = get_or_add_direction(p3p4, dd);
        set_directions_para(d12, d34, pred);

        ar.add_para(d12, d34, pred);
    } else {
        d12->add_line(p3p4, pred);
    }
    return true;
}

bool GeometricGraph::make_ar_para(Predicate* pred) {
    Direction* d1 = static_cast<Direction*>(pred->args[0]);
    Direction* d2 = static_cast<Direction*>(pred->args[1]);

    if (check_para(d1, d2)) return false;

    Line* l1 = get_line_from_direction(d1);
    Line* l2 = get_line_from_direction(d2);

    auto [p1, p2] = get_points_on_line(l1);
    auto [p3, p4] = get_points_on_line(l2);
    pred->args[0] = p1;
    pred->args[1] = p2;
    pred->args.emplace_back(p3);
    pred->args.emplace_back(p4);

    set_directions_para(d1, d2, pred);
    return true;
}

bool GeometricGraph::make_perp(Predicate* pred, DDEngine &dd, AREngine &ar) {
    Point* p1 = static_cast<Point*>(pred->args[0]);
    Point* p2 = static_cast<Point*>(pred->args[1]);
    Point* p3 = static_cast<Point*>(pred->args[2]);
    Point* p4 = static_cast<Point*>(pred->args[3]);

    if (check_perp(p1, p2, p3, p4)) return false;

    Line* p1p2 = get_or_add_line(p1, p2, dd);
    Line* p3p4 = get_or_add_line(p3, p4, dd);

    Direction* d12 = get_or_add_direction(p1p2, dd);
    Direction* d34 = get_or_add_direction(p3p4, dd);
    set_directions_perp(d12, d34, pred);

    if (direction_gradients[d12] < direction_gradients[d34]) {
        std::swap(d12, d34);
    }

    ar.add_perp(d12, d34, pred);

    return true;
}

bool GeometricGraph::make_ar_perp(Predicate* pred) {
    // Exactly the same as make_ar_para.
    Direction* d1 = static_cast<Direction*>(pred->args[0]);
    Direction* d2 = static_cast<Direction*>(pred->args[1]);

    if (check_perp(d1, d2)) return false;

    Line* l1 = get_line_from_direction(d1);
    Line* l2 = get_line_from_direction(d2);

    auto [p1, p2] = get_points_on_line(l1);
    auto [p3, p4] = get_points_on_line(l2);
    pred->args[0] = p1;
    pred->args[1] = p2;
    pred->args.emplace_back(p3);
    pred->args.emplace_back(p4);

    set_directions_perp(d1, d2, pred);
    return true;
}

bool GeometricGraph::make_cong(Predicate* pred, DDEngine &dd, AREngine &ar) {
    Point* p1 = static_cast<Point*>(pred->args[0]);
    Point* p2 = static_cast<Point*>(pred->args[1]);
    Point* p3 = static_cast<Point*>(pred->args[2]);
    Point* p4 = static_cast<Point*>(pred->args[3]);

    Segment* s1 = get_or_add_segment(p1, p2, dd);
    Segment* s2 = get_or_add_segment(p3, p4, dd);

    if (check_cong(s1, s2)) return false;

    Length* l1 = get_or_add_length(s1, dd);
    Length* l2 = get_or_add_length(s2, dd);
    set_lengths_cong(l1, l2, pred);

    // Invariant: point_nums[p1] < point_nums[p2] and point_nums[p3] < point_nums[p4]
    // by definition of how GeometricGraph::__add_new_segment works.
    ar.add_cong(s1, s2, l1, l2, pred);
    return true;
}

bool GeometricGraph::make_ar_cong(Predicate* pred, DDEngine &dd) {
    switch(pred->args.size()) {
        case 2: {
            Length* l1 = static_cast<Length*>(pred->args[0]);
            Length* l2 = static_cast<Length*>(pred->args[1]);

            if (check_cong(l1, l2)) return false;

            Segment* s1 = get_segment_from_length(l1);
            Segment* s2 = get_segment_from_length(l2);

            auto [p1, p2] = s1->endpoints;
            auto [p3, p4] = s2->endpoints;

            pred->args[0] = p1;
            pred->args[1] = p2;
            pred->args.emplace_back(p3);
            pred->args.emplace_back(p4);

            set_lengths_cong(l1, l2, pred);
            return true;
        } break;

        default: {
            Point* p1 = static_cast<Point*>(pred->args[0]);
            Point* p2 = static_cast<Point*>(pred->args[1]);
            Point* p3 = static_cast<Point*>(pred->args[2]);
            Point* p4 = static_cast<Point*>(pred->args[3]);

            Segment* s1 = get_or_add_segment(p1, p2, dd);
            Segment* s2 = get_or_add_segment(p3, p4, dd);

            Length* l1 = get_or_add_length(s1, dd);
            Length* l2 = get_or_add_length(s2, dd);

            if (check_cong(l1, l2)) return false;

            set_lengths_cong(l1, l2, pred);
            return true;
        } break;
    }
}

bool GeometricGraph::make_eqangle(Predicate* pred, DDEngine &dd, AREngine &ar) {
    Point* p1 = static_cast<Point*>(pred->args[0]);
    Point* p2 = static_cast<Point*>(pred->args[1]);
    Point* p3 = static_cast<Point*>(pred->args[2]);
    Point* p4 = static_cast<Point*>(pred->args[3]);
    Point* p5 = static_cast<Point*>(pred->args[4]);
    Point* p6 = static_cast<Point*>(pred->args[5]);
    Point* p7 = static_cast<Point*>(pred->args[6]);
    Point* p8 = static_cast<Point*>(pred->args[7]);

    Angle* a1 = get_or_add_angle(p1, p2, p3, p4, dd);
    Angle* a2 = get_or_add_angle(p5, p6, p7, p8, dd);

    if (check_eqangle(a1, a2)) return false;
    
    if (a1->has_measure()) {
        if (a2->has_measure()) {
            Measure* m1 = a1->get_measure();
            Measure* m2 = a2->get_measure();
            set_measures_equal(m1, m2, pred);
        } else {
            Measure* m1 = a1->get_measure();
            a2->set_measure(m1, pred);
        }
    } else if (a2->has_measure()) {
        Measure* m2 = a2->get_measure();
        a1->set_measure(m2, pred);
    } else {
        Measure* m = __add_new_measure(a1, pred);
        a2->set_measure(m, pred);
    }
    
    Direction* d1 = a1->direction1;
    Direction* d2 = a1->direction2;
    Direction* d3 = a2->direction1;
    Direction* d4 = a2->direction2;
    int pi_offset = 0;

    if (direction_gradients[d1] < direction_gradients[d2]) {
        pi_offset += 1;
    }
    if (direction_gradients[d3] < direction_gradients[d4]) {
        pi_offset -= 1;
    }
    ar.add_eqangle(d1, d2, d3, d4, pred, pi_offset);
    
    return true;
}

bool GeometricGraph::make_ar_eqangle(Predicate* pred, DDEngine& dd) {
    Direction* d1 = static_cast<Direction*>(pred->args[0]);
    Direction* d2 = static_cast<Direction*>(pred->args[1]);
    Direction* d3 = static_cast<Direction*>(pred->args[2]);
    Direction* d4 = static_cast<Direction*>(pred->args[3]);

    Angle* a1 = get_or_add_angle(d1, d2, dd);
    Angle* a2 = get_or_add_angle(d3, d4, dd);

    if (check_eqangle(a1, a2)) return false;

    Line* l1 = get_line_from_direction(d1);
    Line* l2 = get_line_from_direction(d2);
    Line* l3 = get_line_from_direction(d3);
    Line* l4 = get_line_from_direction(d4);

    auto [p1, p2] = get_points_on_line(l1);
    auto [p3, p4] = get_points_on_line(l2);
    auto [p5, p6] = get_points_on_line(l3);
    auto [p7, p8] = get_points_on_line(l4);

    pred->args[0] = p1;
    pred->args[1] = p2;
    pred->args[2] = p3;
    pred->args[3] = p4;
    pred->args.emplace_back(p5);
    pred->args.emplace_back(p6);
    pred->args.emplace_back(p7);
    pred->args.emplace_back(p8);

    if (a1->has_measure()) {
        if (a2->has_measure()) {
            Measure* m1 = a1->get_measure();
            Measure* m2 = a2->get_measure();
            set_measures_equal(m1, m2, pred);
        } else {
            Measure* m1 = a1->get_measure();
            a2->set_measure(m1, pred);
        }
    } else if (a2->has_measure()) {
        Measure* m2 = a2->get_measure();
        a1->set_measure(m2, pred);
    } else {
        Measure* m = __add_new_measure(a1, pred);
        a2->set_measure(m, pred);
    }
    return true;
}

bool GeometricGraph::make_eqratio(Predicate* pred, DDEngine &dd, AREngine &ar) {
    Point* p1 = static_cast<Point*>(pred->args[0]);
    Point* p2 = static_cast<Point*>(pred->args[1]);
    Point* p3 = static_cast<Point*>(pred->args[2]);
    Point* p4 = static_cast<Point*>(pred->args[3]);
    Point* p5 = static_cast<Point*>(pred->args[4]);
    Point* p6 = static_cast<Point*>(pred->args[5]);
    Point* p7 = static_cast<Point*>(pred->args[6]);
    Point* p8 = static_cast<Point*>(pred->args[7]);

    Ratio* r1 = get_or_add_ratio(p1, p2, p3, p4, dd);
    Ratio* r2 = get_or_add_ratio(p5, p6, p7, p8, dd);

    if (check_eqratio(r1, r2)) return false;

    if (r1->has_fraction()) {
        if (r2->has_fraction()) {
            Fraction* f1 = r1->get_fraction();
            Fraction* f2 = r2->get_fraction();
            set_fractions_equal(f1, f2, pred);
        } else {
            Fraction* f1 = r1->get_fraction();
            r2->set_fraction(f1, pred);
        }
    } else if (r2->has_fraction()) {
        Fraction* f2 = r2->get_fraction();
        r1->set_fraction(f2, pred);
    } else {
        Fraction* f = __add_new_fraction(r1, pred);
        r2->set_fraction(f, pred);
    }

    Length* l1 = r1->length1;
    Length* l2 = r1->length2;
    Length* l3 = r2->length1;
    Length* l4 = r2->length2;
    ar.add_eqratio(l1, l2, l3, l4, pred);

    return true;
}

bool GeometricGraph::make_ar_eqratio(Predicate* pred, DDEngine &dd) {
    Length* l1 = static_cast<Length*>(pred->args[0]);
    Length* l2 = static_cast<Length*>(pred->args[1]);
    Length* l3 = static_cast<Length*>(pred->args[2]);
    Length* l4 = static_cast<Length*>(pred->args[3]);

    Ratio* r1 = get_or_add_ratio(l1, l2, dd);
    Ratio* r2 = get_or_add_ratio(l3, l4, dd);

    if (check_eqratio(r1, r2)) return false;

    Segment* s1 = get_segment_from_length(l1);
    Segment* s2 = get_segment_from_length(l2);
    Segment* s3 = get_segment_from_length(l3);
    Segment* s4 = get_segment_from_length(l4);

    auto [p1, p2] = s1->endpoints;
    auto [p3, p4] = s2->endpoints;
    auto [p5, p6] = s3->endpoints;
    auto [p7, p8] = s4->endpoints;

    pred->args[0] = p1;
    pred->args[1] = p2;
    pred->args[2] = p3;
    pred->args[3] = p4;
    pred->args.emplace_back(p5);
    pred->args.emplace_back(p6);
    pred->args.emplace_back(p7);
    pred->args.emplace_back(p8);

    if (r1->has_fraction()) {
        if (r2->has_fraction()) {
            Fraction* f1 = r1->get_fraction();
            Fraction* f2 = r2->get_fraction();
            set_fractions_equal(f1, f2, pred);
        } else {
            Fraction* f1 = r1->get_fraction();
            r2->set_fraction(f1, pred);
        }
    } else if (r2->has_fraction()) {
        Fraction* f2 = r2->get_fraction();
        r1->set_fraction(f2, pred);
    } else {
        Fraction* f = __add_new_fraction(r1, pred);
        r2->set_fraction(f, pred);
    }
    return true;
}

bool GeometricGraph::make_midp(Predicate* pred, DDEngine &dd, AREngine &ar) {
    Point* m = static_cast<Point*>(pred->args[0]);
    Point* p1 = static_cast<Point*>(pred->args[1]);
    Point* p2 = static_cast<Point*>(pred->args[2]);

    if (check_midp(m, p1, p2)) return false;

    // Make them collinear first, if necessary
    if (!check_coll(m, p1, p2)) {
        make_coll(pred, dd, ar);
    }

    Segment* s1 = get_or_add_segment(p1, m, dd);
    Segment* s2 = get_or_add_segment(m, p2, dd);
    Length* l1 = get_or_add_length(s1, dd);
    Length* l2 = get_or_add_length(s2, dd);

    if (l1 == l2) return false;

    set_lengths_cong(l1, l2, pred);

    if (point_nums[s1->endpoints[0]] > point_nums[s2->endpoints[0]]) {
        std::swap(s1, s2);
        std::swap(l1, l2);
    }
    
    ar.add_midp(s1, s2, l1, l2, pred);

    return true;
}

bool GeometricGraph::make_circle(Predicate* pred, DDEngine &dd) {
    Point* c = static_cast<Point*>(pred->args[0]);
    Point* p1 = static_cast<Point*>(pred->args[1]);
    Point* p2 = static_cast<Point*>(pred->args[2]);
    Point* p3 = static_cast<Point*>(pred->args[3]);

    if (check_circle(c, p1, p2, p3)) return false;

    Circle* circ = get_or_add_circle(p1, p2, p3, dd);
    set_circle_center(c, circ, pred);
    return true;
}

bool GeometricGraph::make_constangle(Predicate* pred, DDEngine &dd, AREngine &ar) {
    Frac f = pred->frac_arg;
    Point* p1 = static_cast<Point*>(pred->args[0]);
    Point* p2 = static_cast<Point*>(pred->args[1]);
    Point* p3 = static_cast<Point*>(pred->args[2]);
    Point* p4 = static_cast<Point*>(pred->args[3]);

    Angle* a = get_or_add_angle(p1, p2, p3, p4, dd);
    Measure* m = get_or_add_measure(a, dd);
    
    set_measure_val(m, f, pred);

    Direction* d1 = a->direction1;
    Direction* d2 = a->direction2;
    ar.add_constangle(d1, d2, f.to_double(), pred);
    return true;
}

bool GeometricGraph::make_ar_constangle(Predicate* pred, DDEngine& dd) {
    Direction* d1 = static_cast<Direction*>(pred->args[0]);
    Direction* d2 = static_cast<Direction*>(pred->args[1]);
    Frac f = pred->frac_arg;
    
    Angle* a = get_or_add_angle(d1, d2, dd);
    Measure* m = get_or_add_measure(a, dd);

    Line* l1 = get_line_from_direction(d1);
    Line* l2 = get_line_from_direction(d2);

    auto [p1, p2] = get_points_on_line(l1);
    auto [p3, p4] = get_points_on_line(l2);

    pred->args[0] = p1;
    pred->args[1] = p2;
    pred->args.emplace_back(p3);
    pred->args.emplace_back(p4);

    set_measure_val(m, f, pred);
    return true;
}

bool GeometricGraph::make_constratio(Predicate* pred, DDEngine &dd, AREngine &ar) {
    Frac f = pred->frac_arg;
    Point* p1 = static_cast<Point*>(pred->args[0]);
    Point* p2 = static_cast<Point*>(pred->args[1]);
    Point* p3 = static_cast<Point*>(pred->args[2]);
    Point* p4 = static_cast<Point*>(pred->args[3]);

    Segment* s1 = get_or_add_segment(p1, p2, dd);
    Segment* s2 = get_or_add_segment(p3, p4, dd);

    Length* l1 = get_or_add_length(s1, dd);
    Length* l2 = get_or_add_length(s2, dd);

    Ratio* r = get_or_add_ratio(l1, l2, dd);
    Fraction* fr = get_or_add_fraction(r, dd);

    set_fraction_val(fr, f, pred);

    ar.add_constratio(l1, l2, f.to_double(), pred);
    return true;
}

bool GeometricGraph::make_ar_constratio(Predicate* pred, DDEngine &dd) {
    Length* l1 = static_cast<Length*>(pred->args[0]);
    Length* l2 = static_cast<Length*>(pred->args[1]);
    Frac f = pred->frac_arg;

    Segment* s1 = get_segment_from_length(l1);
    Segment* s2 = get_segment_from_length(l2);

    auto [p1, p2] = s1->endpoints;
    auto [p3, p4] = s2->endpoints;

    pred->args[0] = p1;
    pred->args[1] = p2;
    pred->args.emplace_back(p3);
    pred->args.emplace_back(p4);

    Ratio* r = get_or_add_ratio(l1, l2, dd);
    Fraction* fr = get_or_add_fraction(r, dd);

    set_fraction_val(fr, f, pred);
    return true;
}




int GeometricGraph::synthesise_preds(DDEngine &dd, AREngine &ar) {

    int num = 0;

    auto recent_preds_gen = dd.get_recent_predicates();

    while (recent_preds_gen) {
        bool res = false;
        Predicate* pred = recent_preds_gen();

        switch(pred->name) {
            case pred_t::COLL:
                res = make_coll(pred, dd, ar);
                break;
            case pred_t::CYCLIC:
                res = make_cyclic(pred, dd, ar);
                break;
            case pred_t::PARA:
                res = make_para(pred, dd, ar);
                break;
            case pred_t::PERP:
                res = make_perp(pred, dd, ar);
                break;
            case pred_t::CONG:
                res = make_cong(pred, dd, ar);
                break;
            case pred_t::EQANGLE:
                res = make_eqangle(pred, dd, ar);
                break;
            case pred_t::EQRATIO:
                res = make_eqratio(pred, dd, ar);
                break;
            case pred_t::MIDP:
                res = make_midp(pred, dd, ar);
                break;
            case pred_t::CIRCLE:
                res = make_circle(pred, dd);
                break;
            case pred_t::CONSTANGLE:
                res = make_constangle(pred, dd, ar);
                break;
            case pred_t::CONSTRATIO:
                res = make_constratio(pred, dd, ar);
                break;
            default:
                break;
        }
        if (res) {
            num += 1;
            LOG("Synthesised predicate: " << pred->to_string());
        }
    }
    return num;
}

int GeometricGraph::synthesise_ar_preds(DDEngine &dd) {
    int num = 0;
    auto recent_preds_gen = dd.get_recent_predicates();
    while (recent_preds_gen) {
        bool res = false;
        Predicate* pred = recent_preds_gen();

        switch(pred->name) {
            case pred_t::PARA:
                res = make_ar_para(pred);
                break;
            case pred_t::PERP:
                res = make_ar_perp(pred);
                break;
            case pred_t::CONG:
                res = make_ar_cong(pred, dd);
                break;
            case pred_t::EQANGLE:
                res = make_ar_eqangle(pred, dd);
                break;
            case pred_t::EQRATIO:
                res = make_ar_eqratio(pred, dd);
                break;
            case pred_t::CONSTANGLE:
                res = make_ar_constangle(pred, dd);
                break;
            case pred_t::CONSTRATIO:
                res = make_ar_constratio(pred, dd);
                break;
            default:
                break;
        }

        if (res) {
            num += 1;
            LOG("Synthesised AR predicate: " << pred->to_string());
        }
    }
    return num;
}



void GeometricGraph::__print_points(std::ostream& os) {
    os << "Points: ";
    for (auto& p : root_points) {
        os << p->to_string() << " ";
    }
    os << std::endl;
}


void GeometricGraph::__print_lines(std::ostream& os) {
    os << "Lines:\n";
    for (auto& l : lines) {
        Line* line = l.second.get();
        os << line->to_string() << " : ";
        for (auto& [p, _] : line->points) {
            os << p->to_string() << " ";
        }
        os << std::endl;
    }
    os << std::endl;
}

void GeometricGraph::__print_circles(std::ostream& os) {
    os << "Circles:\n";
    for (auto& c : circles) {
        Circle* circ = c.second.get();
        os << circ->to_string() << " : ";
        for (auto& [p, _] : circ->points) {
            os << p->to_string() << " ";
        }
        os << std::endl;
    }
    os << std::endl;
}

void GeometricGraph::__print_angles(std::ostream& os) {
    os << "Angles:\n";
    for (auto& a : angles) {
        Angle* angle = a.second.get();
        os << angle->to_string() << " : [" << angle->direction1->to_string() << ", " << angle->direction2->to_string() << "]\n";
    }
    os << std::endl;
}

void GeometricGraph::__print_directions(std::ostream& os) {
    os << "Directions:\n";
    for (auto& d : directions) {
        Direction* dir = d.second.get();
        os << dir->to_string() << " : ";
        for (auto& [l, _] : dir->objs) {
            os << l->to_string() << " ";
        }
        os << std::endl;
    }
    os << std::endl;
}

void GeometricGraph::__print_measures(std::ostream& os) {
    os << "Measures:\n";
    for (auto& m : measures) {
        Measure* measure = m.second.get();
        os << measure->to_string() << " : ";
        for (auto& [a, _] : measure->obj2s) {
            os << a->to_string() << " ";
        }
        os << std::endl;
    }
    os << std::endl;
}


void GeometricGraph::print(std::ostream& os) {
    __print_points(os);
    __print_lines(os);
    __print_directions(os);
    __print_circles(os);
    __print_angles(os);
    __print_measures(os);
}


void GeometricGraph::reset_problem() {

    points.clear();
    lines.clear();
    circles.clear();
    segments.clear();
    triangles.clear();

    directions.clear();
    lengths.clear();
    shapes.clear();

    angles.clear();
    ratios.clear();

    measures.clear();
    fractions.clear();

    root_points.clear();
    root_lines.clear();
    root_circles.clear();
    root_segments.clear();
    root_triangles.clear();

    root_directions.clear();
    root_lengths.clear();
    root_shapes.clear();

    root_angles.clear();
    root_ratios.clear();

    root_measures.clear();
    root_fractions.clear();
    
    root_measure_vals.clear();
    root_fraction_vals.clear();

    point_nums.clear();
    line_nums.clear();
    circle_nums.clear();
    direction_gradients.clear();

    num_eq_point_sets.clear();
    point_to_num_eq_set.clear();

    adhoc = 0;
}