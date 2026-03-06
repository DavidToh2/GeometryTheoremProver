
#pragma once

#include <vector>
#include <deque>

#include "TracebackUtils.hh"
#include "DD/Predicate.hh"
#include "Geometry/Object.hh"
#include "Geometry/Value.hh"
#include "Geometry/Object2.hh"
#include "Geometry/Value2.hh"

class TracebackEngine {

public:
    Predicate* goal;

    std::map<Point*, std::map<Line*, Predicate*>> point_on_lines;
    std::map<Point*, std::map<Circle*, Predicate*>> point_on_circles;
    std::map<Point*, std::map<Circle*, Predicate*>> point_as_circle_center;
    std::map<Point*, std::map<Segment*, Predicate*>> point_as_segment_endpoint;
    std::map<Point*, std::map<Triangle*, Predicate*>> point_as_triangle_vertex;

    std::map<std::pair<Direction*, Direction*>, Predicate*> perp_directions;

    std::map<Direction*, std::map<Line*, Predicate*>> direction_of_lines;
    std::map<Length*, std::map<Segment*, Predicate*>> length_of_segments;
    std::map<Dimension*, std::map<Triangle*, Predicate*>> dimension_of_triangles;

    std::map<Measure*, std::map<Angle*, Predicate*>> measure_of_angles;
    std::map<Fraction*, std::map<Ratio*, Predicate*>> fraction_of_ratios;
    std::map<Shape*, std::map<Dimension*, Predicate*>> shape_of_dimensions;

    std::map<Measure*, std::pair<Frac, Predicate*>> measure_vals;
    std::map<Fraction*, std::pair<Frac, Predicate*>> fraction_vals;

    void set_point_on(Point* p, Line* l, Predicate* pred);
    void set_point_on(Point* p, Circle* c, Predicate* pred);
    void set_point_as_center(Point* p, Circle* c, Predicate* pred);
    void set_point_as_endpoint(Point* p, Segment* s, Predicate* pred);
    void set_point_as_vertex(Point* p, Triangle* t, Predicate* pred);

    void set_directions_perp(Direction* d1, Direction* d2, Predicate* pred);

    void set_direction_of(Direction* d, Line* l, Predicate* pred);
    void set_length_of(Length* len, Segment* s, Predicate* pred);
    void set_dimension_of(Dimension* dim, Triangle* t, Predicate* pred);

    void set_measure_of(Measure* m, Angle* a, Predicate* pred);
    void set_fraction_of(Fraction* f, Ratio* r, Predicate* pred);
    void set_shape_of(Shape* s, Dimension* d, Predicate* pred);

    void set_measure_val(Measure* m, Frac val, Predicate* pred);
    void set_fraction_val(Fraction* f, Frac val, Predicate* pred);

    void set_goal(Predicate* pred);

    PredSet why_coll(Point* p1, Point* p2, Point* p3);

    PredSet why_cyclic(Point* p1, Point* p2, Point* p3, Point* p4);

    PredSet why_para(Point* p1, Point* p2, Point* p3, Point* p4);
    PredSet why_para(Line* l1, Line* l2);

    PredSet why_perp(Point* p1, Point* p2, Point* p3, Point* p4);
    PredSet why_perp(Line* l1, Line* l2);

    PredSet why_cong(Point* p1, Point* p2, Point* p3, Point* p4);
    PredSet why_cong(Segment* s1, Segment* s2);

    PredSet why_eqangle(Point* p1, Point* p2, Point* p3, Point* p4, Point* p5, Point* p6, Point* p7, Point* p8);
    PredSet why_eqangle(Line* l1, Line* l2, Line* l3, Line* l4);
    PredSet why_eqangle(Angle* a1, Angle* a2);

    PredSet why_eqratio(Point* p1, Point* p2, Point* p3, Point* p4, Point* p5, Point* p6, Point* p7, Point* p8);
    PredSet why_eqratio(Segment* s1, Segment* s2, Segment* s3, Segment* s4);
    PredSet why_eqratio(Ratio* r1, Ratio* r2);

    PredSet why_contri(Point* p1, Point* p2, Point* p3, Point* p4, Point* p5, Point* p6);
    PredSet why_contri(Triangle* t1, Triangle* t2);

    PredSet why_simtri(Point* p1, Point* p2, Point* p3, Point* p4, Point* p5, Point* p6);
    PredSet why_simtri(Triangle* t1, Triangle* t2);

    PredSet why_midp(Point* m, Point* p1, Point* p2);

    PredSet why_circle(Point* c, Point* p1, Point* p2, Point* p3);

    PredSet why_constangle(Angle* a, Frac f);

    PredSet why_constratio(Ratio* r, Frac f);

    void populate_why(Predicate* pred);

    void get_solution(std::stringstream &ss);
};