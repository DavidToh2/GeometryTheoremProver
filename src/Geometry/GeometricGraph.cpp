
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

/* Point functions */

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

/* Line functions */

Line* GeometricGraph::__add_new_line(Point* p1, Point* p2, Predicate* base_pred) {
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
    auto gen = NodeUtils::on_roots<Line>(p1->on_line);
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



void GeometricGraph::__add_new_direction(Line* l, Predicate* pred) {
    std::string dir_id = "D_" + l->name;
    if (Utils::isinmap(dir_id, directions)) {
        throw GGraphInternalError("Error: Direction with id " + dir_id + " already exists in GeometricGraph.");
    }
    directions[dir_id] = std::make_unique<Direction>(dir_id);
    Direction* dir = directions[dir_id].get();
    dir->add_line(l, pred);
    l->direction = dir;
}

void GeometricGraph::add_new_direction(Line* l, Predicate* pred) {
    Line* root_l = NodeUtils::get_root(l);
    if (root_l->has_direction()) {
        return;
    }
    __add_new_direction(root_l, pred);
}

void GeometricGraph::__merge_directions(Direction* dest, Direction* src, Predicate* pred) {
    dest->merge(src, pred);
    root_directions.erase(NodeUtils::get_root(src));
}


/* Predicate setting functions*/


void GeometricGraph::make_collinear(Predicate* pred, DDEngine &dd) {
    Point* p1 = static_cast<Point*>(pred->args[0]);
    Point* p2 = static_cast<Point*>(pred->args[1]);
    Point* p3 = static_cast<Point*>(pred->args[2]);

    auto [rp1, rp2, rp3] = NodeUtils::get_root<Point, 3>({p1, p2, p3});

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

    auto [rp1, rp2, rp3, rp4] = NodeUtils::get_root<Point, 4>({p1, p2, p3, p4});

    Line* p1p2 = get_or_add_line(rp1, rp2, dd);
    Line* p3p4 = get_or_add_line(rp3, rp4, dd);


}

void GeometricGraph::make_perp(Predicate* pred, DDEngine &dd) {
    // TBA
}



bool GeometricGraph::check_collinear(Point* p1, Point* p2, Point* p3) {
    Line* l = __try_get_line(p1, p2);
    if (!l) { return false; }
    return l->contains(p3);
}

bool GeometricGraph::check_collinear(Point* p1, Line* l) {
    return l->contains(p1);
}

bool GeometricGraph::check_cyclic(Point* p1, Point* p2, Point* p3, Point* p4) {
    // TBA
    return false;
}





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
                // TBA
                break;
            case pred_t::PERP:
                // TBA
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
}