
#include <map>
#include <memory>
#include <iostream>

#include "Object.hh"
#include "Object2.hh"
#include "Value.hh"
#include "GeometricGraph.hh"
#include "Common/Exceptions.hh"
#include "Common/Utils.hh"
#include "DD/DDEngine.hh"

void GeometricGraph::__add_point(const std::string point_id) {
    if (Utils::isinmap(point_id, points)) {
        throw GGraphInternalError("Error: Point with id " + point_id + " already exists in GeometricGraph.");
    }
    points[point_id] = std::make_unique<Point>(point_id);
}

void GeometricGraph::try_add_point(const std::string point_id) {
    if (!Utils::isinmap(point_id, points)) {
        points[point_id] = std::make_unique<Point>(point_id);
    }
}

Point* GeometricGraph::get_or_add_point(const std::string point_id) {
    try_add_point(point_id);
    return points[point_id].get();
}

void GeometricGraph::add_points_to_objects(DDEngine &dd) {

    auto recent_preds_gen = dd.get_recent_predicates();
    while (recent_preds_gen) {
        Predicate* pred = recent_preds_gen();
    }
}

void GeometricGraph::__print_points(std::ostream& os) {
    os << "Points: ";
    for (auto& p : points) {
        Point* point = p.second.get();
        os << point->to_string() << " ";
    }
}