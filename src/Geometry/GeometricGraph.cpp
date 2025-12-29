
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




void GeometricGraph::__add_new_point(const std::string point_id) {
    if (Utils::isinmap(point_id, points)) {
        throw GGraphInternalError("Error: Point with id " + point_id + " already exists in GeometricGraph.");
    }
    points[point_id] = std::make_unique<Point>(point_id);
    root_points.insert(points[point_id].get());
}

void GeometricGraph::__try_add_point(const std::string point_id) {
    if (!Utils::isinmap(point_id, points)) {
        points[point_id] = std::make_unique<Point>(point_id);
        root_points.insert(points[point_id].get());
    }
}

Point* GeometricGraph::get_or_add_point(const std::string point_id) {
    __try_add_point(point_id);
    return points[point_id].get();
}

void GeometricGraph::merge_points(Point* dest, Point* src, Predicate* pred) {
    if (dest == src) return;
    root_points.erase(NodeUtils::get_root(src));
    dest->merge(src, pred);
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
    p1->set_this_on(l, base_pred);
    p2->set_this_on(l, base_pred);
    root_lines.insert(l);
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

void GeometricGraph::merge_lines(Line* dest, Line* src, Predicate* pred) {
    if (dest == src) return;
    root_lines.erase(NodeUtils::get_root(src));
    dest->merge(src, pred);
}




Direction* GeometricGraph::__add_new_direction(Line* l, Predicate* base_pred) {
    std::string dir_id = "d_" + l->name;
    if (Utils::isinmap(dir_id, directions)) {
        throw GGraphInternalError("Error: Direction with id " + dir_id + " already exists in GeometricGraph.");
    }
    directions[dir_id] = std::make_unique<Direction>(dir_id);
    Direction* dir = directions[dir_id].get();
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
    if (dest == src) return;
    root_directions.erase(NodeUtils::get_root(src));
    if (src->has_perp()) {
        root_directions.erase(NodeUtils::get_root(src->perp));
    }
    dest->merge(src, pred);
}

void GeometricGraph::set_directions_perp(Direction* d1, Direction* d2, Predicate* pred) {
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
    p1->set_this_on(circ, base_pred);
    p2->set_this_on(circ, base_pred);
    p3->set_this_on(circ, base_pred);
    root_circles.insert(circ);
    return circ;
}
Circle* GeometricGraph::__add_new_circle(Point* c, Point* p1, Predicate* base_pred) {
    std::string circle_id = "c_" + c->name + "_" + p1->name;
    if (Utils::isinmap(circle_id, circles)) {
        throw GGraphInternalError("Error: Circle " + circle_id + " already exists in GeometricGraph.");
    }
    circles[circle_id] = std::make_unique<Circle>(circle_id, c, p1, base_pred);
    Circle* circ = circles[circle_id].get();
    p1->set_this_on(circ, base_pred);
    root_circles.insert(circ);
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
    auto gen = p1->on_circles();
    Circle* circ = nullptr;
    while (gen) {
        Circle* circ0 = gen();
        if (circ0->get_center() == NodeUtils::get_root(c)) {
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

void GeometricGraph::merge_circles(Circle* dest, Circle* src, Predicate* pred) {
    if (dest == src) return;
    root_circles.erase(NodeUtils::get_root(src));
    dest->merge(src, pred);
}





Angle* GeometricGraph::__add_new_angle(Direction* d1, Direction* d2, Predicate* base_pred) {
    std::string angle_id = "a_" + d1->name + "_" + d2->name;
    if (Utils::isinmap(angle_id, angles)) {
        throw GGraphInternalError("Error: Angle with id " + angle_id + " already exists in GeometricGraph.");
    }
    angles[angle_id] = std::make_unique<Angle>(angle_id, d1, d2);
    Angle* a = angles[angle_id].get();
    d1->on_angles_1.insert(a);
    d2->on_angles_2.insert(a);
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
    if (dest == src) return;
    root_angles.erase(NodeUtils::get_root(src));
    dest->merge(src, pred);
}




Measure* GeometricGraph::__add_new_measure(Angle* a, Predicate* base_pred) {
    std::string measure_id = "m_" + a->name;
    if (Utils::isinmap(measure_id, measures)) {
        throw GGraphInternalError("Error: Measure with id " + measure_id + " already exists in GeometricGraph.");
    }
    measures[measure_id] = std::make_unique<Measure>(measure_id);
    Measure* m = measures[measure_id].get();
    m->add_angle(a, base_pred);
    a->set_measure(m, base_pred);
    root_measures.insert(m);
    return m;
}
Measure* GeometricGraph::get_or_add_measure(Angle* a, DDEngine& dd) {
    Angle* ra = NodeUtils::get_root(a);
    if (ra->has_measure()) {
        return ra->get_measure();
    }
    return __add_new_measure(ra, dd.base_pred.get());
}
void GeometricGraph::set_measures_equal(Measure* m1, Measure* m2, Predicate* pred) {
    if (m1 == m2) return;
    root_measures.erase(NodeUtils::get_root(m2));
    m1->merge(m2, pred);
}





bool GeometricGraph::check_coll(Point* p1, Point* p2, Point* p3) {
    Line* l = try_get_line(p1, p2);
    if (!l) { return false; }
    return l->contains(p3);
}

bool GeometricGraph::check_coll(Point* p1, Line* l) { return l->contains(p1); }

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


bool GeometricGraph::check_postcondition(PredicateTemplate* pred) {
    
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
        if (p3p1) merge_lines(p2p3, p3p1, pred);
        if (p1p2) merge_lines(p2p3, p1p2, pred);
    } else if (p3p1) {
        if (p1p2) merge_lines(p3p1, p1p2, pred);
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
        if (c341) merge_circles(c234, c341, pred);
        if (c412) merge_circles(c234, c412, pred);
        if (c123) merge_circles(c234, c123, pred);
    } else if (c341) {
        if (c412) merge_circles(c341, c412, pred);
        if (c123) merge_circles(c341, c123, pred);
    } else if (c412) {
        if (c123) merge_circles(c412, c123, pred);
    }
    return true;
}

bool GeometricGraph::make_para(Predicate* pred, DDEngine &dd, AREngine &ar) {
    Point* p1 = static_cast<Point*>(pred->args[0]);
    Point* p2 = static_cast<Point*>(pred->args[1]);
    Point* p3 = static_cast<Point*>(pred->args[2]);
    Point* p4 = static_cast<Point*>(pred->args[3]);

    auto [rp1, rp2, rp3, rp4] = NodeUtils::get_roots<Point, 4>({p1, p2, p3, p4});

    if (check_para(rp1, rp2, rp3, rp4)) return false;

    Line* p1p2 = get_or_add_line(rp1, rp2, dd);
    Line* p3p4 = get_or_add_line(rp3, rp4, dd);

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

bool GeometricGraph::make_perp(Predicate* pred, DDEngine &dd, AREngine &ar) {
    Point* p1 = static_cast<Point*>(pred->args[0]);
    Point* p2 = static_cast<Point*>(pred->args[1]);
    Point* p3 = static_cast<Point*>(pred->args[2]);
    Point* p4 = static_cast<Point*>(pred->args[3]);

    auto [rp1, rp2, rp3, rp4] = NodeUtils::get_roots<Point, 4>({p1, p2, p3, p4});

    if (check_perp(rp1, rp2, rp3, rp4)) return false;

    Line* p1p2 = get_or_add_line(rp1, rp2, dd);
    Line* p3p4 = get_or_add_line(rp3, rp4, dd);

    Direction* d12 = get_or_add_direction(p1p2, dd);
    Direction* d34 = get_or_add_direction(p3p4, dd);
    set_directions_perp(d12, d34, pred);

    ar.add_perp(d12, d34, pred);

    return true;
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
    ar.add_eqangle(a1, a2, pred);
    
    return true;
}

bool GeometricGraph::make_circle(Predicate* pred, DDEngine &dd, AREngine &ar) {
    Point* c = static_cast<Point*>(pred->args[0]);
    Point* p1 = static_cast<Point*>(pred->args[1]);
    Point* p2 = static_cast<Point*>(pred->args[2]);
    Point* p3 = static_cast<Point*>(pred->args[3]);

    if (check_circle(c, p1, p2, p3)) return false;

    Circle* circ = get_or_add_circle(p1, p2, p3, dd);
    circ->set_center(c, pred);
    return true;
}





void GeometricGraph::synthesise_preds(DDEngine &dd, AREngine &ar) {

    auto recent_preds_gen = dd.get_recent_predicates();

    while (recent_preds_gen) {
        Predicate* pred = recent_preds_gen();

        switch(pred->name) {
            case pred_t::COLL:
                make_coll(pred, dd, ar);
                break;
            case pred_t::CYCLIC:
                make_cyclic(pred, dd, ar);
                break;
            case pred_t::PARA:
                make_para(pred, dd, ar);
                break;
            case pred_t::PERP:
                make_perp(pred, dd, ar);
                break;
            case pred_t::EQANGLE:
                make_eqangle(pred, dd, ar);
                break;
            case pred_t::CIRCLE:
                make_circle(pred, dd, ar);
                break;
            default:
                break;
        }
        
    }
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

    adhoc = 0;
}