#pragma once

#include "Common/Constants.hh"
#include "Numerics.hh"
#include "Cartesian.hh"
#include "NumInstance.hh"
#include "Common/Generator.hh"
#include "Geometry/Object.hh"

/* NumEngine class.

`Order_of_ops`: stores an even-length vector of ints [a0, a1, ...] indicating that
the numeric resolution sequence proceeds as follows: compute a0 numerics, then
resolve the first a1 points in `order_of_resolution`, and so on and so forth. 

`progress`: integer indicating the number of operations performed so far until the
first resolution conflict. */
class NumEngine {
public:
    std::vector<std::unique_ptr<Numeric>> numerics;

    std::set<Point*> all_points;

    std::vector<int> order_of_ops;
    std::vector<Point*> order_of_resolution;

    std::vector<NumInstance> instances;
    NumInstance final_inst;

    Numeric* insert_numeric(std::unique_ptr<Numeric>&& num);

    void compute_free(NumInstance& inst, Numeric* num);
    void compute_segment(NumInstance& inst, Numeric* num);
    void compute_triangle(NumInstance& inst, Numeric* num);
    void compute_iso_triangle(NumInstance& inst, Numeric* num);
    void compute_r_triangle(NumInstance& inst, Numeric* num);
    void compute_riso_triangle(NumInstance& inst, Numeric* num);
    void compute_equi_triangle(NumInstance& inst, Numeric* num);
    void compute_r_triangle_p(NumInstance& inst, Numeric* num);
    void compute_r_triangle_n(NumInstance& inst, Numeric* num);
    void compute_equi_triangle_p(NumInstance& inst, Numeric* num);

    /* Generate a random convex quadrilateral. */
    void compute_quadrilateral(NumInstance& inst, Numeric* num);
    /* Generate a random cyclic quadrilateral with points in this order. */
    void compute_cyclic_quad(NumInstance& inst, Numeric* num);
    void compute_rectangle(NumInstance& inst, Numeric* num);
    void compute_square(NumInstance& inst, Numeric* num);
    /* Given two points A, B: generate two points X, Y such that ABXY is a square 
    with points in counterclockwise order. */
    void compute_square_off_p(NumInstance& inst, Numeric* num);
    /* Generate a random convex pentagon. */
    // void compute_pentagon(Numeric* num);

    /* Given four points A, B, C, D: generate a random trapezoid with AB//CD */
    void compute_trapezoid(NumInstance& inst, Numeric* num);
    void compute_eq_trapezoid(NumInstance& inst, Numeric* num);

    void compute_line(NumInstance& inst, Numeric* num);
    /* Given five points A, B, C, D, E: generate a line of points X such that
    the directed angles <(XA, AB) = <(CD, DE). */
    void compute_line_at_angle(NumInstance& inst, Numeric* num);
    void compute_line_bisect(NumInstance& inst, Numeric* num);
    void compute_line_para(NumInstance& inst, Numeric* num);
    void compute_line_perp(NumInstance& inst, Numeric* num);
    void compute_ray(NumInstance& inst, Numeric* num);

    /* Given three points A, B, C: generate the circle containing all points X
    such that XA = BC. */
    void compute_circle(NumInstance& inst, Numeric* num);
    /* Given three points A, B, C: generate their circumcircle. */
    void compute_circum(NumInstance& inst, Numeric* num);
    /* Given two points A, B: generate the circle with AB as diameter. */
    void compute_diameter(NumInstance& inst, Numeric* num);

    void compute_midpoint(NumInstance& inst, Numeric* num);
    void compute_trisegment(NumInstance& inst, Numeric* num);
    /* Given two points A, B: generate the reflection of A in B. */
    void compute_mirror(NumInstance& inst, Numeric* num);
    /* Given three points A, B, C: generate the reflection of A in line BC. */
    void compute_reflect(NumInstance& inst, Numeric* num);

    /* Given three points A, B, C: generate a point X such that the absolute
    angles <(BA, AX) = <(XC, CB). */
    void compute_angle_eq2(NumInstance& inst, Numeric* num);
    /* Given five points A, B, D, E, F: generate the locus of all points X such
    that the *DIRECTED* angles <(AX, XB) = <(ED, DF). 
    (If we wanted to implement this for absolute angles, we would need a "Circle
    Arc" Numeric class which is currently low priority.)*/
    void compute_angle_eq3(NumInstance& inst, Numeric* num);
    /* Given three points A, B, C: generate the locus of all points X such that
    the absolute angles <(BA, BC) = <(BC, BX). */
    void compute_angle_mirror(NumInstance& inst, Numeric* num);
    /* Given three points A, B, C: generate the bisector of the angle ABC. */
    void compute_angle_bisect(NumInstance& inst, Numeric* num);
    /* Given three points A, B, C: generate the external bisector of the angle
    ABC. */
    void compute_angle_exbisect(NumInstance& inst, Numeric* num);
    /* Given three points A, B, C: generate the loci of points X, followed by
    the loci of points Y, such that BA, BX, BY, BC form equal directed angles
    in this order. */
    void compute_angle_trisect(NumInstance& inst, Numeric* num);

    /* Given three points A, B, C: generate the line tangent to the circumcircle
    of triangle ABC at point A. */
    void compute_line_tangent(NumInstance& inst, Numeric* num);
    /* Given three points A, O, B: generate the points X, Y which are the tangents
    from the point A to the circle centered at O with radius OB. */
    void compute_tangents(NumInstance& inst, Numeric* num);
    /* Given four points O, A, I, B: generate two points X, Y such that the circle
    centered at O with radius OA, and the circle centered at I with radius IB,
    have a common EXTERNAL tangent XY, where X is on circle O and Y is on circle I. */
    void compute_common_tangent(NumInstance& inst, Numeric* num);
    /* Given four points O, A, I, B: generate four points X, Y, Z, W such that
    the circle centered at O with radius OA, and the circle centered at I with
    radius IB, have common EXTERNAL tangents XY and ZW, where X, Z are on circle 
    O and Y, W are on circle I. */
    void compute_common_tangent2(NumInstance& inst, Numeric* num);

    std::map<num_t, void(NumEngine::*)(NumInstance&, Numeric*)> compute_function_map_point = {
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

    std::map<num_t, void(NumEngine::*)(NumInstance&, Numeric*)> compute_function_map_ray = {
        {num_t::LINE_AT_ANGLE, &NumEngine::compute_line_at_angle},
        {num_t::ANGLE_MIRROR, &NumEngine::compute_angle_mirror},
        {num_t::ANGLE_BISECT, &NumEngine::compute_angle_bisect},
        {num_t::ANGLE_TRISECT, &NumEngine::compute_angle_trisect},
        {num_t::RAY, &NumEngine::compute_ray},
    };

    std::map<num_t, void(NumEngine::*)(NumInstance&, Numeric*)> compute_function_map_line = {
        {num_t::LINE, &NumEngine::compute_line},
        {num_t::LINE_BISECT, &NumEngine::compute_line_bisect},
        {num_t::LINE_PARA, &NumEngine::compute_line_para},
        {num_t::LINE_PERP, &NumEngine::compute_line_perp},

        {num_t::ANGLE_EXBISECT, &NumEngine::compute_angle_exbisect},

        {num_t::LINE_TANGENT, &NumEngine::compute_line_tangent},
    };

    std::map<num_t, void(NumEngine::*)(NumInstance&, Numeric*)> compute_function_map_circle = {
        {num_t::CIRCLE, &NumEngine::compute_circle},
        {num_t::CIRCUM, &NumEngine::compute_circum},
        {num_t::DIAMETER, &NumEngine::compute_diameter},

        {num_t::ANGLE_EQ3, &NumEngine::compute_angle_eq3},
    };



    void get_operation_order();



    /* Given a numeric, calls the relevant computation function.
    Updates the computation status of the output points. */
    void compute_one(NumInstance& inst, Numeric* num);

    /* Resolves one point `p` to its Cartesian coordinates. */
    void resolve_one(NumInstance& inst, Point* p);

    bool compute(NumInstance& inst);



    /* Draw a valid NumInstance. */
    bool first_draw();



    /* Fetch the Cartesian coordinates of a point from the finalised NumInstance. */
    constexpr CartesianPoint get_cartesian(Point* p) {
        return final_inst.get_most_likely_coords(p);
    }


    void reset_problem();
};