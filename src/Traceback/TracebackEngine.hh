
#pragma once

#include <vector>
#include <deque>

#include "TracebackUtils.hh"
#include "DD/Predicate.hh"
#include "Geometry/Object.hh"
#include "Geometry/Value.hh"
#include "Geometry/Object2.hh"
#include "Geometry/Value2.hh"

/* TracebackEngine class.

The TracebackEngine exposes three groups of functions:
1. The `record_merge` and `set_x_on/of` functions record merges of nodes, and predicates, invoked during
the proof process respectively;
2. The `why_on` and `why_x_of` functions are invoked during the proof process to obtain the dependency predicates
for the membership of a node in another node's attributes;
3. The `why_pred` functions are invoked at the end of the proof process to derive a set of dependency predicates
necessary for deriving the `pred` in question.

The `x_on_y` and `x_of_y` maps, such as `point_on_lines` and `direction_of_lines`, record the exact instances at
which each pair of nodes in our GeometricGraph were related via some predicate. The exact nodes that were
related are recorded.

As nodes are merged, the `root_maps` allow us to recover the exact nodes that were related from their root
nodes at that instant. For example, if `point_on_lines` recorded some pair `(p, l)`, and at some later 
instance their roots are `(rp, rl)` respectively, then we would expect to see an entry in `point_line_root_map`
pointing `(rp, rl)` to `(p, l)`.

The `root_maps` thus record all mappings, from root nodes to the original related nodes, that are valid at the
current program point. They are updated by Group 1 functions and queried by Group 2 functions.

The `x_on_y` and `x_of_y` maps are updated by Group 1 functions, and queried by both Group 2 and Group 3 
functions.

IMPORTANT INVARIANT: All nodes passed to any function must be root nodes at their time of invocation.
The traceback will only search through predicates where these nodes were involved as root nodes. 

INVARIANT: The predicate traceback functions `why_pred()` can only take points which are distinct at the
end of the solving process.

Note: `tr->record_merge()` should be called for Directions before Lines, for `__earliest_direction_of()` to
work as intended. */
class TracebackEngine {

public:
    Predicate* goal;

    std::map<Point*, std::map<Line*, Predicate*>> point_on_lines;
    std::map<Point*, std::map<Line*, std::pair<Point*, Line*>>> point_line_root_map;
    std::map<Point*, std::map<Circle*, Predicate*>> point_on_circles;
    std::map<Point*, std::map<Circle*, std::pair<Point*, Circle*>>> point_circle_root_map;
    std::map<Point*, std::map<Circle*, Predicate*>> point_as_circle_center;
    std::map<Point*, std::map<Circle*, std::pair<Point*, Circle*>>> point_circle_center_root_map;
    std::map<Point*, std::map<Segment*, Predicate*>> point_as_segment_endpoint;
    std::map<Point*, std::map<Segment*, std::pair<Point*, Segment*>>> point_segment_endpoint_root_map;
    std::map<Point*, std::map<Triangle*, Predicate*>> point_as_triangle_vertex;
    std::map<Point*, std::map<Triangle*, std::pair<Point*, Triangle*>>> point_triangle_vertex_root_map;

    std::map<std::pair<Direction*, Direction*>, Predicate*> perp_directions;
    std::map<std::pair<Direction*, Direction*>, std::set<std::pair<Direction*, Direction*>>> perp_directions_root_map;

    std::map<Direction*, std::map<Line*, Predicate*>> direction_of_lines;
    std::map<Direction*, std::map<Line*, std::pair<Direction*, Line*>>> direction_line_root_map;
    std::map<Length*, std::map<Segment*, Predicate*>> length_of_segments;
    std::map<Length*, std::map<Segment*, std::pair<Length*, Segment*>>> length_segment_root_map;
    std::map<Dimension*, std::map<Triangle*, Predicate*>> dimension_of_triangles;
    std::map<Dimension*, std::map<Triangle*, std::pair<Dimension*, Triangle*>>> dimension_triangle_root_map;

    std::map<Angle*, std::map<Direction*, std::pair<Angle*, Direction*>>> direction_angle_root_map;
    std::map<Ratio*, std::map<Length*, std::pair<Ratio*, Length*>>> length_ratio_root_map;

    std::map<Measure*, std::map<Angle*, Predicate*>> measure_of_angles;
    std::map<Measure*, std::map<Angle*, std::pair<Measure*, Angle*>>> measure_angle_root_map;
    std::map<Fraction*, std::map<Ratio*, Predicate*>> fraction_of_ratios;
    std::map<Fraction*, std::map<Ratio*, std::pair<Fraction*, Ratio*>>> fraction_ratio_root_map;
    std::map<Shape*, std::map<Dimension*, Predicate*>> shape_of_dimensions;
    std::map<Shape*, std::map<Dimension*, std::pair<Shape*, Dimension*>>> shape_dimension_root_map;

    std::map<Measure*, std::pair<Frac, Predicate*>> measure_vals;
    std::map<Fraction*, std::pair<Frac, Predicate*>> fraction_vals;

    void record_merge(Point* dest, Point* src);
    void record_merge(Line* dest, Line* src);
    void record_merge(Circle* dest, Circle* src);
    void record_merge(Segment* dest, Segment* src);
    void record_merge(Triangle* dest, Triangle* src);

    void record_merge(Direction* dest, Direction* src);
    void record_merge(Length* dest, Length* src);
    void record_merge(Dimension* dest, Dimension* src);

    void record_merge(Angle* dest, Angle* src);
    void record_merge(Ratio* dest, Ratio* src);
    void record_merge(Shape* dest, Shape* src);

    void record_merge(Measure* dest, Measure* src);
    void record_merge(Fraction* dest, Fraction* src);

    void set_point_on(Point* p, Line* l, Predicate* pred);
    PredSet why_on(Point* p, Line* l);
    void set_point_on(Point* p, Circle* c, Predicate* pred);
    PredSet why_on(Point* p, Circle* c);
    void set_point_as_center(Point* p, Circle* c, Predicate* pred);
    PredSet why_center(Point* p, Circle* c);
    void set_point_as_endpoint(Point* p, Segment* s, Predicate* pred);
    PredSet why_endpoint(Point* p, Segment* s);
    void set_point_as_vertex(Point* p, Triangle* t, Predicate* pred);
    PredSet why_vertex(Point* p, Triangle* t);

    void set_directions_perp(Direction* d1, Direction* d2, Predicate* pred);
    PredSet why_directions_perp(Direction* d1, Direction* d2);

    void set_direction_of(Direction* d, Line* l, Predicate* pred);
    PredSet why_direction_of(Direction* d, Line* l);
    void set_length_of(Length* len, Segment* s, Predicate* pred);
    void set_dimension_of(Dimension* dim, Triangle* t, Predicate* pred);

    PredSet why_direction_of(Direction* d, Angle* a);
    PredSet why_length_of(Length* len, Ratio* r);

    void set_measure_of(Measure* m, Angle* a, Predicate* pred);
    void set_fraction_of(Fraction* f, Ratio* r, Predicate* pred);
    void set_shape_of(Shape* s, Dimension* d, Predicate* pred);

    void set_measure_val(Measure* m, Frac val, Predicate* pred);
    void set_fraction_val(Fraction* f, Frac val, Predicate* pred);

    void set_goal(Predicate* pred);



    
    Direction* __earliest_direction_of(Line* l);



    
    std::tuple<std::map<Line*, PredSet>, Line*> lca_lines_and_why(
        Point* p1, Point* p2,
        std::map<std::pair<Point*, Point*>, PredSet> why_point_ancestor_cache,
        std::map<std::pair<Line*, Line*>, PredSet> why_line_ancestor_cache
    );




    

    PredSet why_coll(Point* p1, Point* p2, Point* p3);

    PredSet why_cyclic(Point* p1, Point* p2, Point* p3, Point* p4);

    PredSet why_para(Point* p1, Point* p2, Point* p3, Point* p4);

    PredSet why_perp(Point* p1, Point* p2, Point* p3, Point* p4);

    PredSet why_cong(Point* p1, Point* p2, Point* p3, Point* p4);

    PredSet why_eqangle(Point* p1, Point* p2, Point* p3, Point* p4, Point* p5, Point* p6, Point* p7, Point* p8);

    PredSet why_eqratio(Point* p1, Point* p2, Point* p3, Point* p4, Point* p5, Point* p6, Point* p7, Point* p8);

    PredSet why_contri(Point* p1, Point* p2, Point* p3, Point* p4, Point* p5, Point* p6);

    PredSet why_simtri(Point* p1, Point* p2, Point* p3, Point* p4, Point* p5, Point* p6);

    PredSet why_midp(Point* m, Point* p1, Point* p2);

    PredSet why_circle(Point* c, Point* p1, Point* p2, Point* p3);

    // These two are technically not necessary?:

    PredSet why_constangle(Angle* a, Frac f);

    PredSet why_constratio(Ratio* r, Frac f);

    void populate_why(Predicate* pred);

    void get_solution(std::stringstream &ss);
};