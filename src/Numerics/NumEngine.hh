#pragma once

#include "Common/Constants.hh"
#include "Numerics.hh"
#include "Cartesian.hh"
#include "Common/Generator.hh"
#include "Geometry/Object.hh"

/* NumEngine class.

When computing Numerics, the elements of the `numerics` vector may be evaluated 
in order of their insertion. 

The `point_to_cartesian_objs` map contains a list of `CartesianObject`s used to 
compute the `CartesianPoint` of each `Point`. This may take the form of either: 
a single `CartesianPoint`, or a list of `CartesianLine`s and `CartesianCircle`s. 

Note: For the computation to be successful, the first appearance of every point
as an argument can only occur after ALL its appearances as outputs. */
class NumEngine {
public:
    std::vector<std::unique_ptr<Numeric>> numerics;

    std::map<Point*, std::vector<CartesianObject>> point_to_cartesian_objs;
    std::map<Point*, std::vector<CartesianPoint>> point_to_cartesian;

    enum ComputationStatus {
        UNCOMPUTED,
        COMPUTING,
        RESOLVED,
        RESOLVED_WITH_DISCREPANCY
    };
    std::map<Point*, ComputationStatus> point_status;

    Numeric* insert_numeric(std::unique_ptr<Numeric>&& num);

    Generator<CartesianPoint> compute_free(Numeric* num);
    Generator<CartesianPoint> compute_segment(Numeric* num);
    Generator<CartesianPoint> compute_triangle(Numeric* num);
    Generator<CartesianPoint> compute_iso_triangle(Numeric* num);
    Generator<CartesianPoint> compute_r_triangle(Numeric* num);
    Generator<CartesianPoint> compute_riso_triangle(Numeric* num);
    Generator<CartesianPoint> compute_equi_triangle(Numeric* num);
    Generator<CartesianPoint> compute_r_triangle_p(Numeric* num);
    Generator<CartesianPoint> compute_r_triangle_n(Numeric* num);
    Generator<CartesianPoint> compute_equi_triangle_p(Numeric* num);
    Generator<CartesianPoint> compute_quadrilateral(Numeric* num);
    Generator<CartesianPoint> compute_rectangle(Numeric* num);
    Generator<CartesianPoint> compute_square(Numeric* num);
    Generator<CartesianPoint> compute_square_off(Numeric* num);
    Generator<CartesianPoint> compute_pentagon(Numeric* num);
    Generator<CartesianPoint> compute_trapezoid(Numeric* num);
    Generator<CartesianPoint> compute_eq_trapezoid(Numeric* num);

    Generator<CartesianLine> compute_line(Numeric* num);
    Generator<CartesianLine> compute_line_at_angle(Numeric* num);
    Generator<CartesianLine> compute_line_at_angle2(Numeric* num);
    Generator<CartesianLine> compute_line_bisect(Numeric* num);
    Generator<CartesianLine> compute_line_para(Numeric* num);
    Generator<CartesianLine> compute_line_perp(Numeric* num);
    // Generator<Cartesian> compute_ray(Numeric* num);

    Generator<CartesianCircle> compute_circle(Numeric* num);
    Generator<CartesianCircle> compute_circum(Numeric* num);
    Generator<CartesianCircle> compute_diameter(Numeric* num);

    Generator<CartesianPoint> compute_midpoint(Numeric* num);
    Generator<CartesianPoint> compute_trisegment(Numeric* num);
    Generator<CartesianPoint> compute_mirror(Numeric* num);
    Generator<CartesianPoint> compute_reflect(Numeric* num);

    Generator<CartesianPoint> compute_angle_eq2(Numeric* num);
    Generator<CartesianCircle> compute_angle_eq3(Numeric* num);
    Generator<CartesianLine> compute_angle_mirror(Numeric* num);
    Generator<CartesianLine> compute_angle_bisect(Numeric* num);
    Generator<CartesianLine> compute_angle_trisect(Numeric* num);

    Generator<CartesianPoint> compute_tangents(Numeric* num);
    Generator<CartesianLine> compute_common_tangent(Numeric* num);
    Generator<CartesianLine> compute_common_tangent2(Numeric* num);

    std::map<num_t, Generator<CartesianPoint>(NumEngine::*)(Numeric*)> compute_function_map_point = {
        {num_t::FREE, &NumEngine::compute_free},
        {num_t::SEGMENT, &NumEngine::compute_segment},
        {num_t::TRIANGLE, &NumEngine::compute_triangle},
        {num_t::ISO_TRIANGLE, &NumEngine::compute_iso_triangle},
        {num_t::R_TRIANGLE, &NumEngine::compute_r_triangle},
        {num_t::RISO_TRIANGLE, &NumEngine::compute_riso_triangle},
        {num_t::EQUI_TRIANGLE, &NumEngine::compute_equi_triangle},
        {num_t::R_TRIANGLE_P, &NumEngine::compute_r_triangle_p},
        {num_t::R_TRIANGLE_N, &NumEngine::compute_r_triangle_n},
        {num_t::EQUI_TRIANGLE_P, &NumEngine::compute_equi_triangle_p},
        {num_t::QUADRILATERAL, &NumEngine::compute_quadrilateral},
        {num_t::RECTANGLE, &NumEngine::compute_rectangle},
        {num_t::SQUARE, &NumEngine::compute_square},
        {num_t::SQUARE_OFF, &NumEngine::compute_square_off},
        {num_t::PENTAGON, &NumEngine::compute_pentagon},
        {num_t::TRAPEZOID, &NumEngine::compute_trapezoid},
        {num_t::EQ_TRAPEZOID, &NumEngine::compute_eq_trapezoid},

        {num_t::MIDPOINT, &NumEngine::compute_midpoint},
        {num_t::TRISEGMENT, &NumEngine::compute_trisegment},
        {num_t::MIRROR, &NumEngine::compute_mirror},
        {num_t::REFLECT, &NumEngine::compute_reflect},

        {num_t::ANGLE_EQ2, &NumEngine::compute_angle_eq2},

        {num_t::TANGENTS, &NumEngine::compute_tangents},
    };

    std::map<num_t, Generator<CartesianLine>(NumEngine::*)(Numeric*)> compute_function_map_line = {
        {num_t::LINE, &NumEngine::compute_line},
        {num_t::LINE_AT_ANGLE, &NumEngine::compute_line_at_angle},
        {num_t::LINE_AT_ANGLE2, &NumEngine::compute_line_at_angle2},
        {num_t::LINE_BISECT, &NumEngine::compute_line_bisect},
        {num_t::LINE_PARA, &NumEngine::compute_line_para},
        {num_t::LINE_PERP, &NumEngine::compute_line_perp},

        {num_t::ANGLE_MIRROR, &NumEngine::compute_angle_mirror},
        {num_t::ANGLE_BISECT, &NumEngine::compute_angle_bisect},
        {num_t::ANGLE_TRISECT, &NumEngine::compute_angle_trisect},

        {num_t::COMMON_TANGENT, &NumEngine::compute_common_tangent},
        {num_t::COMMON_TANGENT2, &NumEngine::compute_common_tangent2}
    };

    std::map<num_t, Generator<CartesianCircle>(NumEngine::*)(Numeric*)> compute_function_map_circle = {
        {num_t::CIRCLE, &NumEngine::compute_circle},
        {num_t::CIRCUM, &NumEngine::compute_circum},
        {num_t::DIAMETER, &NumEngine::compute_diameter},

        {num_t::ANGLE_EQ3, &NumEngine::compute_angle_eq3},
    };

    /* Given a numeric, calls the relevant computation function.
    Updates the computation status of the output points.
    Returns `false` if any of the arguments have not yet been resolved.
    Returns `true` if the computation is successful. */
    bool compute_one(Numeric* num);
    void compute();

    bool resolve_one(Point* p);
    /* Resolves all currently computing points.
    Returns `false` if at least one point is resolved with a discrepancy: it will
    then have multiple coordinates in its `point_to_cartesian` entry.
    Returns `true` if all points are successfully resolved without discrepancy. */
    bool resolve();

    void reset_computation();
    void reset_problem();
};