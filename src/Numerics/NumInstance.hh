
#pragma once

#include <map>
#include <vector>
#include <span>

#include "Geometry/Object.hh"
#include "Numerics.hh"
#include "Cartesian.hh"

class NumInstance {
public:
    std::map<Point*, std::vector<std::vector<CartesianObject>>> point_to_cartesian_objs;
    std::map<Point*, std::vector<int>> point_coord_occurences;
    std::map<Point*, std::vector<CartesianPoint>> point_to_coords;

    enum ComputationStatus {
        UNCOMPUTED,
        COMPUTING,
        TO_RESOLVE,
        RESOLVED,
        RESOLVED_WITH_DISCREPANCY
    };
    std::map<Point*, ComputationStatus> point_status;
    std::vector<std::vector<double>> params;

    CartesianPoint centroid_of_resolved_points;
    int num_resolved = 0;
    double max_dist;

    NumInstance() = default;
    NumInstance(const std::set<Point*> &points);
    NumInstance(const std::map<std::string, std::unique_ptr<Point>>& point_map);

    constexpr std::span<const CartesianPoint> get_arg_coords(Numeric* num, int i) const {
        return std::span<const CartesianPoint>(point_to_coords.at(num->args[i]));
    }
    constexpr void record_out(Numeric* num, int i, CartesianPoint pt) {
        point_to_coords[num->outs[i]].emplace_back(pt);
    }
    constexpr void record_out(Numeric* num, int i, CartesianObject obj) {
        point_to_cartesian_objs[num->outs[i]].back().emplace_back(obj);
    }
    constexpr void next_outs(Numeric* num) {
        for (Point* p : num->outs) {
            point_to_cartesian_objs.at(p).emplace_back(std::vector<CartesianObject>{});
        }
    }
    constexpr void record_params(std::initializer_list<double> ps) {
        params.emplace_back(ps);
    }


    void record_computation_status(Numeric* num);
    void record_resolution_status(Point* p);

    /* Checks if there is any point that has been resolved, and that has a candidate coordinate
    equal to `c`. Returns `true` if such a point exists. */
    bool check_against_existing_point_numerics(CartesianPoint c);
    /* Fetches the coordinate with the highest occurence count.
    Note: Assumes that the point p has been resolved. */
    CartesianPoint get_most_likely_coords(Point* p);
    /* Updates the `sum_of_resolved_points` and `max_dist` fields with the new coordinate `cp`. */
    void update_resolved_centroid_and_radius(CartesianPoint cp);

    // These functions are for test purposes only
    /* Gets a random coordinate for point p */
    constexpr CartesianPoint __get_coord(Point* p) const {
        return point_to_coords.at(p).back();
    }
    constexpr CartesianObject __get_obj(Point* p) const {
        return point_to_cartesian_objs.at(p).back().back();
    }



    double loss = 0;

    void compute_loss();

    bool is_valid() const;
};