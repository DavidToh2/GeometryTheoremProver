
#include <map>
#include <memory>
#include <iostream>

#include "DD/Predicate.hh"
#include "Geometry/Node.hh"
#include "Object.hh"
#include "Object2.hh"
#include "Value.hh"
#include "GeometricGraph.hh"
#include "Common/Exceptions.hh"
#include "Common/Utils.hh"
#include "DD/DDEngine.hh"





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

void GeometricGraph::__merge_points(Point* dest, Point* src, Predicate* pred) {
    dest->merge(src, pred);
    root_points.erase(NodeUtils::get_root(src));
}




Line* GeometricGraph::__add_new_line(Point* p1, Point* p2, Predicate* base_pred) {
    if (p1 == p2) {
        throw GGraphInternalError("Error: Cannot create line with identical endpoints: " + p1->name);
    }
    std::string line_id = p1->name + p2->name;
    if (Utils::isinmap(line_id, lines)) {
        throw GGraphInternalError("Error: Line with id " + line_id + " already exists in GeometricGraph.");
    }
    lines[line_id] = std::make_unique<Line>(line_id, p1, p2, base_pred);
    Line* l = lines[line_id].get();
    p1->set_this_on(l, base_pred);
    p2->set_this_on(l, base_pred);
    l->points[p1] = base_pred;
    l->points[p2] = base_pred;
    root_lines.insert(l);
    return l;
}

Line* GeometricGraph::__try_get_line(Point* p1, Point* p2) {
    auto gen = NodeUtils::all_roots<Line>(p1->on_line);
    Line* l = nullptr;
    while (gen) {
        Line* l0 = gen();
        if (l0->contains(p2)) {
            l = l0;
            break;
        }
    }
    if (l) {
        return NodeUtils::get_root(l);
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

void GeometricGraph::__merge_lines(Line* dest, Line* src, Predicate* pred) {
    dest->merge(src, pred);
    root_lines.erase(NodeUtils::get_root(src));
}




Direction* GeometricGraph::__add_new_direction(Line* l, Predicate* base_pred) {
    std::string dir_id = "D_" + l->name;
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

void GeometricGraph::__set_directions_para(Direction* dest, Direction* src, Predicate* pred) {
    root_directions.erase(NodeUtils::get_root(src));
    if (src->has_perp()) {
        root_directions.erase(NodeUtils::get_root(src->perp));
    }
    dest->merge(src, pred);
}

void GeometricGraph::__set_directions_perp(Direction* d1, Direction* d2, Predicate* pred) {
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





void GeometricGraph::make_collinear(Predicate* pred, DDEngine &dd) {
    Point* p1 = static_cast<Point*>(pred->args[0]);
    Point* p2 = static_cast<Point*>(pred->args[1]);
    Point* p3 = static_cast<Point*>(pred->args[2]);

    auto [rp1, rp2, rp3] = NodeUtils::get_roots<Point, 3>({p1, p2, p3});

    Line* p1p2 = get_or_add_line(rp1, rp2, dd);
    p1p2->points[rp3] = pred;
    rp3->set_this_on(p1p2, pred);

    if (Line* p1p3 = __try_get_line(rp1, rp3)) {
        __merge_lines(p1p2, p1p3, pred);
    }
    if (Line* p2p3 = __try_get_line(rp2, rp3)) {
        __merge_lines(p1p2, p2p3, pred);
    }
}

void GeometricGraph::make_cyclic(Predicate* pred, DDEngine &dd) {
    // TBA
}

void GeometricGraph::make_para(Predicate* pred, DDEngine &dd) {
    Point* p1 = static_cast<Point*>(pred->args[0]);
    Point* p2 = static_cast<Point*>(pred->args[1]);
    Point* p3 = static_cast<Point*>(pred->args[2]);
    Point* p4 = static_cast<Point*>(pred->args[3]);

    auto [rp1, rp2, rp3, rp4] = NodeUtils::get_roots<Point, 4>({p1, p2, p3, p4});

    Line* p1p2 = get_or_add_line(rp1, rp2, dd);
    Line* p3p4 = get_or_add_line(rp3, rp4, dd);

    Direction* d12 = get_or_add_direction(p1p2, dd);
    if (p3p4->has_direction()) {
        Direction* d34 = get_or_add_direction(p3p4, dd);
        __set_directions_para(d12, d34, pred);
    } else {
        d12->add_line(p3p4, pred);
    }

}

void GeometricGraph::make_perp(Predicate* pred, DDEngine &dd) {
    Point* p1 = static_cast<Point*>(pred->args[0]);
    Point* p2 = static_cast<Point*>(pred->args[1]);
    Point* p3 = static_cast<Point*>(pred->args[2]);
    Point* p4 = static_cast<Point*>(pred->args[3]);

    auto [rp1, rp2, rp3, rp4] = NodeUtils::get_roots<Point, 4>({p1, p2, p3, p4});

    Line* p1p2 = get_or_add_line(rp1, rp2, dd);
    Line* p3p4 = get_or_add_line(rp3, rp4, dd);

    Direction* d12 = get_or_add_direction(p1p2, dd);
    Direction* d34 = get_or_add_direction(p3p4, dd);
    __set_directions_perp(d12, d34, pred);
}



bool GeometricGraph::check_coll(Point* p1, Point* p2, Point* p3) {
    Line* l = __try_get_line(p1, p2);
    if (!l) { return false; }
    return l->contains(p3);
}

bool GeometricGraph::check_collinear(Point* p1, Line* l) { return l->contains(p1); }

bool GeometricGraph::check_cyclic(Point* p1, Point* p2, Point* p3, Point* p4) {
    // TBA
    return false;
}

bool GeometricGraph::check_para(Point* p1, Point* p2, Point* p3, Point* p4) {
    Line* p1p2 = __try_get_line(p1, p2);
    if (!p1p2) { return false; }
    return check_para(p3, p4, p1p2);
}

bool GeometricGraph::check_para(Point* p1, Point* p2, Line* l1) {
    Line* p1p2 = __try_get_line(p1, p2);
    if (!p1p2) { return false; }
    return check_para(p1p2, l1);
}

bool GeometricGraph::check_para(Line* l1, Line* l2) { return Line::is_para(l1, l2); }


bool GeometricGraph::check_perp(Point* p1, Point* p2, Point* p3, Point* p4) {
    Line* p1p2 = __try_get_line(p1, p2);
    if (!p1p2) { return false; }
    return check_perp(p3, p4, p1p2);
}

bool GeometricGraph::check_perp(Point* p1, Point* p2, Line* l1) {
    Line* p1p2 = __try_get_line(p1, p2);
    if (!p1p2) { return false; }
    return check_perp(p1p2, l1);
}

bool GeometricGraph::check_perp(Line* l1, Line* l2) { return Line::is_perp(l1, l2); }



void GeometricGraph::synthesise_objects(DDEngine &dd) {

    auto recent_preds_gen = dd.get_recent_predicates();

    while (recent_preds_gen) {
        Predicate* pred = recent_preds_gen();

        switch(pred->name) {
            case pred_t::COLL:
                make_collinear(pred, dd);
                break;
            case pred_t::CYCLIC:
                make_cyclic(pred, dd);
                break;
            case pred_t::PARA:
                make_para(pred, dd);
                break;
            case pred_t::PERP:
                make_perp(pred, dd);
                break;
            default:
                break;
        }
        
    }
}

void GeometricGraph::__print_points(std::ostream& os) {
    os << "Points: ";
    for (auto& p : points) {
        Point* point = p.second.get();
        os << point->to_string() << " ";
    }
    os << std::endl;
}