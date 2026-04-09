
#include <cassert>

#include "NumInstance.hh"
#include "Common/Constants.hh"


NumInstance::NumInstance(const std::set<Point*> &points) {
    for (Point* p : points) {
        point_to_cartesian_objs[p] = {}; 
        point_coord_occurences[p] = {};
        point_to_coords[p] = {};
        point_status[p] = UNCOMPUTED;
    }
}
NumInstance::NumInstance(const std::map<std::string, std::unique_ptr<Point>>& point_map) {
    for (auto& [_, ptr] : point_map) {
        Point* p = ptr.get();
        point_to_cartesian_objs[p] = {}; 
        point_coord_occurences[p] = {};
        point_to_coords[p] = {};
        point_status[p] = UNCOMPUTED;
    }
}


void NumInstance::record_computation_status(Numeric* num) {
    for (Point* p : num->args) {
        assert(point_status[p] >= ComputationStatus::RESOLVED);
    }
    for (Point* p : num->outs) {
        point_status[p] = ComputationStatus::COMPUTING;
    }
}
void NumInstance::record_resolution_status(Point* p) {
    assert(point_status[p] == ComputationStatus::COMPUTING);
    point_status[p] = (point_to_coords.at(p).size() > 1) ? 
        ComputationStatus::RESOLVED_WITH_DISCREPANCY : ComputationStatus::RESOLVED;
}

bool NumInstance::check_against_existing_point_numerics(CartesianPoint c) {
    for (const auto& [p, coords] : point_to_coords) {
        if (point_status[p] < ComputationStatus::RESOLVED) continue;
        for (const CartesianPoint& existing_c : coords) {
            if (CartesianPoint::is_close(existing_c, c)) {
                return true;
            }
        }
    }
    return false;
}
CartesianPoint NumInstance::get_most_likely_coords(Point* p) {
    int max_occurences = 0;
    CartesianPoint most_likely_coords;
    for (int i = 0; i < point_to_coords[p].size(); i++) {
        if (point_coord_occurences[p][i] > max_occurences) {
            max_occurences = point_coord_occurences[p][i];
            most_likely_coords = point_to_coords[p][i];
        }
    }
    return most_likely_coords;
}
void NumInstance::update_resolved_centroid_and_radius(CartesianPoint cp) {
    if (num_resolved == 0) {
        centroid_of_resolved_points = cp;
        max_dist = 0;
    } else {
        double dist = Cartesian::distance(centroid_of_resolved_points, cp);
        max_dist = std::max(max_dist, dist);
        centroid_of_resolved_points = (centroid_of_resolved_points * num_resolved + cp) / (num_resolved + 1);
    }
    num_resolved++;
}


void NumInstance::compute_loss() {
    for (auto& [p, coords] : point_to_coords) {
        int s = coords.size();
        if (s <= 1) continue;
        for (int i=0; i<s-1; i++) {
            for (int j=i+1; j<s; j++) {
                loss += point_coord_occurences[p][i] * point_coord_occurences[p][j] 
                    * Cartesian::distance2(coords[i], coords[j]);
            }
        }
    }
}

bool NumInstance::is_valid() const {
    return loss < TOL;
}