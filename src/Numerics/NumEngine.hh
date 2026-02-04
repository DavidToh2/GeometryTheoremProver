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
        TO_RESOLVE,
        RESOLVED,
        RESOLVED_WITH_DISCREPANCY
    };
    std::map<Point*, ComputationStatus> point_status;
    int num_resolved = 0;
    CartesianPoint sum_of_resolved_points;
    double max_dist;
    std::vector<Point*> order_of_resolution;

    Numeric* insert_numeric(std::unique_ptr<Numeric>&& num);

    constexpr CartesianPoint get_cartesian(Point* p) {
        return point_to_cartesian.at(p).back();
    }
    constexpr CartesianPoint get_arg_cartesian(Numeric* num, int i) {
        return point_to_cartesian.at(num->args[i]).back();
    }

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

    /* Generate a random convex quadrilateral. */
    Generator<CartesianPoint> compute_quadrilateral(Numeric* num);
    /* Generate a random cyclic quadrilateral with points in this order. */
    Generator<CartesianPoint> compute_cyclic_quad(Numeric* num);
    Generator<CartesianPoint> compute_rectangle(Numeric* num);
    Generator<CartesianPoint> compute_square(Numeric* num);
    /* Given two points A, B: generate two points X, Y such that ABXY is a square 
    with points in counterclockwise order. */
    Generator<CartesianPoint> compute_square_off_p(Numeric* num);
    /* Generate a random convex pentagon. */
    // Generator<CartesianPoint> compute_pentagon(Numeric* num);

    /* Given four points A, B, C, D: generate a random trapezoid with AB//CD */
    Generator<CartesianPoint> compute_trapezoid(Numeric* num);
    Generator<CartesianPoint> compute_eq_trapezoid(Numeric* num);

    Generator<CartesianLine> compute_line(Numeric* num);
    /* Given five points A, B, C, D, E: generate a line of points X such that
    the directed angles <(XA, AB) = <(CD, DE). */
    Generator<CartesianRay> compute_line_at_angle(Numeric* num);
    Generator<CartesianLine> compute_line_bisect(Numeric* num);
    Generator<CartesianLine> compute_line_para(Numeric* num);
    Generator<CartesianLine> compute_line_perp(Numeric* num);
    Generator<CartesianRay> compute_ray(Numeric* num);

    /* Given three points A, B, C: generate the circle containing all points X
    such that XA = BC. */
    Generator<CartesianCircle> compute_circle(Numeric* num);
    /* Given three points A, B, C: generate their circumcircle. */
    Generator<CartesianCircle> compute_circum(Numeric* num);
    /* Given two points A, B: generate the circle with AB as diameter. */
    Generator<CartesianCircle> compute_diameter(Numeric* num);

    Generator<CartesianPoint> compute_midpoint(Numeric* num);
    Generator<CartesianPoint> compute_trisegment(Numeric* num);
    /* Given two points A, B: generate the reflection of A in B. */
    Generator<CartesianPoint> compute_mirror(Numeric* num);
    /* Given three points A, B, C: generate the reflection of A in line BC. */
    Generator<CartesianPoint> compute_reflect(Numeric* num);

    /* Given three points A, B, C: generate a point X such that the absolute
    angles <(BA, AX) = <(XC, CB). */
    Generator<CartesianPoint> compute_angle_eq2(Numeric* num);
    /* Given five points A, B, D, E, F: generate the locus of all points X such
    that the *DIRECTED* angles <(AX, XB) = <(ED, DF). 
    (If we wanted to implement this for absolute angles, we would need a "Circle
    Arc" Numeric class which is currently low priority.)*/
    Generator<CartesianCircle> compute_angle_eq3(Numeric* num);
    /* Given three points A, B, C: generate the locus of all points X such that
    the absolute angles <(BA, BC) = <(BC, BX). */
    Generator<CartesianRay> compute_angle_mirror(Numeric* num);
    /* Given three points A, B, C: generate the bisector of the angle ABC. */
    Generator<CartesianRay> compute_angle_bisect(Numeric* num);
    /* Given three points A, B, C: generate the external bisector of the angle
    ABC. */
    Generator<CartesianLine> compute_angle_exbisect(Numeric* num);
    /* Given three points A, B, C: generate the loci of points X, followed by
    the loci of points Y, such that BA, BX, BY, BC form equal directed angles
    in this order. */
    Generator<CartesianRay> compute_angle_trisect(Numeric* num);

    /* Given three points A, O, B: generate the points X, Y which are the tangents
    from the point A to the circle centered at O with radius OB. */
    Generator<CartesianPoint> compute_tangents(Numeric* num);
    /* Given four points O, A, I, B: generate two points X, Y such that the circle
    centered at O with radius OA, and the circle centered at I with radius IB,
    have a common EXTERNAL tangent XY, where X is on circle O and Y is on circle I. */
    Generator<CartesianPoint> compute_common_tangent(Numeric* num);
    /* Given four points O, A, I, B: generate four points X, Y, Z, W such that
    the circle centered at O with radius OA, and the circle centered at I with
    radius IB, have common EXTERNAL tangents XY and ZW, where X, Z are on circle 
    O and Y, W are on circle I. */
    Generator<CartesianPoint> compute_common_tangent2(Numeric* num);

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
        {num_t::CYCLIC_QUAD, &NumEngine::compute_cyclic_quad},
        {num_t::RECTANGLE, &NumEngine::compute_rectangle},
        {num_t::SQUARE, &NumEngine::compute_square},
        {num_t::SQUARE_OFF_P, &NumEngine::compute_square_off_p},
        // {num_t::PENTAGON, &NumEngine::compute_pentagon},
        {num_t::TRAPEZOID, &NumEngine::compute_trapezoid},
        {num_t::EQ_TRAPEZOID, &NumEngine::compute_eq_trapezoid},

        {num_t::MIDPOINT, &NumEngine::compute_midpoint},
        {num_t::TRISEGMENT, &NumEngine::compute_trisegment},
        {num_t::MIRROR, &NumEngine::compute_mirror},
        {num_t::REFLECT, &NumEngine::compute_reflect},

        {num_t::ANGLE_EQ2, &NumEngine::compute_angle_eq2},

        {num_t::TANGENTS, &NumEngine::compute_tangents},
        {num_t::COMMON_TANGENT, &NumEngine::compute_common_tangent},
        {num_t::COMMON_TANGENT2, &NumEngine::compute_common_tangent2}
    };

    std::map<num_t, Generator<CartesianRay>(NumEngine::*)(Numeric*)> compute_function_map_ray = {
        {num_t::LINE_AT_ANGLE, &NumEngine::compute_line_at_angle},
        {num_t::ANGLE_MIRROR, &NumEngine::compute_angle_mirror},
        {num_t::ANGLE_BISECT, &NumEngine::compute_angle_bisect},
        {num_t::ANGLE_TRISECT, &NumEngine::compute_angle_trisect},
        {num_t::RAY, &NumEngine::compute_ray},
    };

    std::map<num_t, Generator<CartesianLine>(NumEngine::*)(Numeric*)> compute_function_map_line = {
        {num_t::LINE, &NumEngine::compute_line},
        {num_t::LINE_BISECT, &NumEngine::compute_line_bisect},
        {num_t::LINE_PARA, &NumEngine::compute_line_para},
        {num_t::LINE_PERP, &NumEngine::compute_line_perp},

        {num_t::ANGLE_EXBISECT, &NumEngine::compute_angle_exbisect},
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

    /* Updates the `sum_of_resolved_points` and `max_dist` fields with the new
    coordinate `cp`. */
    void update_resolved_radius(CartesianPoint cp);
    /* Resolves one point `p` to its Cartesian coordinates.
    Returns `false` if there is a discrepancy in the computed coordinates (i.e., 
    multiple possible coordinates).
    Returns `true` if the point is successfully resolved without discrepancy.
    In either case, all possible coordinates are stored in `point_to_cartesian`. */
    bool resolve_one(Point* p);
    /* Resolves all points slated to be resolved.
    Returns `false` if at least one point is resolved with a discrepancy: it will
    then have multiple coordinates in its `point_to_cartesian` entry.
    Returns `true` if all points are successfully resolved without discrepancy. */
    bool resolve();
    /* Resolves all remaining points which are still in COMPUTING or TO_RESOLVE 
    status.
    Returns `false` if at least one point is resolved with a discrepancy: it will
    then have multiple coordinates in its `point_to_cartesian` entry.
    Returns `true` if all points are successfully resolved without discrepancy. */
    bool final_resolve();

    /* Checks if there is any point who has been resolved, and whose coordinate is
    equal to `c`. Returns `true` if such a point exists. */
    bool check_against_existing_point_numerics(CartesianPoint &c);

    void reset_computation();
    void reset_problem();
};