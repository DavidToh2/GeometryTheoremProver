
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

Note: The `directions_of_angles` and `lengths_of_ratios` maps do not store any corresponding PredSets, as these
relationships are intrinsic to the definition of the angles and ratios rather than being an "ownership". Theoretically
the same could be said for `point_as_segment_endpoint` and `point_as_triangle_vertex`, but we store the PredSets
for now to maintain consistency across the Object class.

As nodes are merged, the `root_maps` allow us to recover the exact nodes that were related from their root
nodes at that instant. For example, if `point_on_lines` recorded some pair `(p, l)`, and at some later 
instance their roots are `(rp, rl)` respectively, then we would expect to see an entry in `point_line_root_map`
pointing `(rp, rl)` to `(p, l)`.

The `root_maps` thus record all mappings, from root nodes to the original related nodes, that are valid at the
current program point. They are updated by Group 1 functions and queried by Group 2 functions.

Note: The `perp_directions_root_map`, `angle_directions_root_map` and `ratio_lengths_root_map` are special as
they only store as keys the final root nodes of the related objects.

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

    std::map<Point*, std::map<Line*, PredSet>> point_on_lines;
    std::map<Point*, std::map<Line*, std::pair<Point*, Line*>>> point_line_root_map;
    std::map<Point*, std::map<Circle*, PredSet>> point_on_circles;
    std::map<Point*, std::map<Circle*, std::pair<Point*, Circle*>>> point_circle_root_map;
    std::map<Point*, std::map<Circle*, PredSet>> point_as_circle_center;
    std::map<Point*, std::map<Circle*, std::pair<Point*, Circle*>>> point_circle_center_root_map;
    std::map<Point*, std::map<Segment*, PredSet>> point_as_segment_endpoint;
    std::map<Point*, std::map<Segment*, std::pair<Point*, Segment*>>> point_segment_endpoint_root_map;
    std::map<Point*, std::map<Triangle*, PredSet>> point_as_triangle_vertex;
    std::map<Point*, std::map<Triangle*, std::pair<Point*, Triangle*>>> point_triangle_vertex_root_map;

    std::map<std::pair<Direction*, Direction*>, PredSet> perp_directions;
    std::map<std::pair<Direction*, Direction*>, std::set<std::pair<Direction*, Direction*>>> perp_directions_root_map;

    std::map<Direction*, std::map<Line*, PredSet>> direction_of_lines;
    std::map<Direction*, std::map<Line*, std::pair<Direction*, Line*>>> direction_line_root_map;
    std::map<Length*, std::map<Segment*, PredSet>> length_of_segments;
    std::map<Length*, std::map<Segment*, std::pair<Length*, Segment*>>> length_segment_root_map;
    std::map<Dimension*, std::map<Triangle*, PredSet>> dimension_of_triangles;
    std::map<Dimension*, std::map<Triangle*, std::pair<Dimension*, Triangle*>>> dimension_triangle_root_map;

    std::map<std::pair<Direction*, Direction*>, Angle*> directions_of_angles;
    std::map<std::pair<Direction*, Direction*>, std::set<std::pair<Direction*, Direction*>>> angle_directions_root_map;
    std::map<std::pair<Length*, Length*>, Ratio*> lengths_of_ratios;
    std::map<std::pair<Length*, Length*>, std::set<std::pair<Length*, Length*>>> ratio_lengths_root_map;

    std::map<Measure*, std::map<Angle*, PredSet>> measure_of_angles;
    std::map<Measure*, std::map<Angle*, std::pair<Measure*, Angle*>>> measure_angle_root_map;
    std::map<Fraction*, std::map<Ratio*, PredSet>> fraction_of_ratios;
    std::map<Fraction*, std::map<Ratio*, std::pair<Fraction*, Ratio*>>> fraction_ratio_root_map;
    std::map<Shape*, std::map<Dimension*, PredSet>> shape_of_dimensions;
    std::map<Shape*, std::map<Dimension*, std::pair<Shape*, Dimension*>>> shape_dimension_root_map;

    std::map<Measure*, std::pair<Frac, PredSet>> measure_vals;
    std::map<Fraction*, std::pair<Frac, PredSet>> fraction_vals;

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

    void set_point_on(Point* p, Line* l, PredSet pred);
    PredSet why_on(Point* p, Line* l);
    void set_point_on(Point* p, Circle* c, PredSet pred);
    PredSet why_on(Point* p, Circle* c);
    void set_point_as_center(Point* p, Circle* c, PredSet pred);
    PredSet why_center(Point* p, Circle* c);
    void set_point_as_endpoint(Point* p, Segment* s, PredSet pred);
    PredSet why_endpoint(Point* p, Segment* s);
    void set_point_as_vertex(Point* p, Triangle* t, PredSet pred);
    PredSet why_vertex(Point* p, Triangle* t);

    void set_directions_perp(Direction* d1, Direction* d2, PredSet pred);
    PredSet why_directions_perp(Direction* d1, Direction* d2);

    void set_direction_of(Direction* d, Line* l, PredSet pred);
    PredSet why_direction_of(Direction* d, Line* l);
    void set_length_of(Length* len, Segment* s, PredSet pred);
    PredSet why_length_of(Length* len, Segment* s);
    void set_dimension_of(Dimension* dim, Triangle* t, PredSet pred);

    void make_angle_with_directions(Angle* a, Direction* d1, Direction* d2);
    /* Extracts the shortest explanation for why directions d1, d2 are currently the
    direction1 and direction2's of angle a respectively. */
    PredSet why_directions_of_angle(Angle* a, Direction* d1, Direction* d2);
    void make_ratio_with_lengths(Ratio* r, Length* len1, Length* len2);
    /* Extracts the shortest explanation for why lengths len1, len2 are currently the
    length1 and length2's of ratio r respectively. */
    PredSet why_lengths_of_ratio(Ratio* r, Length* len1, Length* len2);

    void set_measure_of(Measure* m, Angle* a, PredSet pred);
    PredSet why_measure_of(Measure* m, Angle* a);
    void set_fraction_of(Fraction* f, Ratio* r, PredSet pred);
    PredSet why_fraction_of(Fraction* f, Ratio* r);
    void set_shape_of(Shape* s, Dimension* d, PredSet pred);

    void set_measure_val(Measure* m, Frac val, PredSet pred);
    void set_fraction_val(Fraction* f, Frac val, PredSet pred);

    void set_goal(Predicate* pred);





    /* Identifies the earliest known instance at which the line `l`, or an ancestor of
    it, was assigned some direction `d`. Returns this direction `d`. (Thus, `d` is also
    the "earliest" direction assigned to `l` or an ancestor if itself, and is recorded 
    in `direction_line_root_map`.) */
    Direction* __earliest_direction_of(
        Line* l,
        std::map<Line*, Direction*>& earliest_direction_cache
    );
    /* Identifies the earliest known instance at which the segment `s`, or an ancestor of
    it, was assigned some length `len`. Returns this length `len`. (Thus, `len` is also
    the "earliest" length assigned to `s` or an ancestor of itself, and is recorded in
    `length_segment_root_map`.) */
    Length* __earliest_length_of(
        Segment* s,
        std::map<Segment*, Length*>& earliest_length_cache
    );
    
    /* Identifies the earliest known instance at which the angle `a`, or an ancestor of
    it, was assigned some measure `m`. Returns this measure `m` as well as the ancestor
    angle `aa` at which `m` was assigned (as recorded in `measure_angle_root_map`). */
    std::pair<Angle*, Measure*> __earliest_measure_of(
        Angle* a,
        std::map<Angle*, Measure*>& earliest_measure_cache
    );
    /* Identifies the earliest known instance at which the ratio `r`, or an ancestor of
    it, was assigned some fraction `f`. Returns this fraction `f` as well as the ancestor
    ratio `ra` at which `f` was assigned (as recorded in `fraction_ratio_root_map`). */
    std::pair<Ratio*, Fraction*> __earliest_fraction_of(
        Ratio* r,
        std::map<Ratio*, Fraction*>& earliest_fraction_cache
    );


    /* Given a pair of points `p1, p2`, identifies all LCA lines `l` of `l1, l2` containing 
    some child points `cp1, cp2` of both `p1, p2`; for each LCA line, constructs the 
    PredSet explaining why it contains the two points, defined as the addition of
    - point_on_lines[cp1][l1]
    - point_on_lines[cp2][l2]
    - why_ancestor(cp1, p1)
    - why_ancestor(cp2, p2)
    - why_ancestor(l1, l)
    - why_ancestor(l2, l) 
    Only extracts Lines with Directions. */
    std::tuple<std::map<Line*, PredSet>, Line*> lca_lines_and_why(
        Point* p1, Point* p2,
        std::map<std::pair<Point*, Point*>, PredSet>& why_point_ancestor_cache,
        std::map<std::pair<Line*, Line*>, PredSet>& why_line_ancestor_cache
    );
    /* Given a pair of points `p1, p2`, identifies all LCA segments `s` containing a child 
    point `cp1` of `p1` as an endpoint, and `cp2` of `p2` as another; for each LCA segment,
    constructs the PredSet explaining why it contains the two points, defined as the
    addition of
    - point_as_segment_endpoint[cp1][s1]
    - point_as_segment_endpoint[cp2][s2]
    - why_ancestor(cp1, p1)
    - why_ancestor(cp2, p2)
    - why_ancestor(s1, s)
    - why_ancestor(s2, s) 
    Only extracts Segments with Lengths. */
    std::tuple<std::map<Segment*, PredSet>, Segment*> lca_segments_and_why(
        Point* p1, Point* p2,
        std::map<std::pair<Point*, Point*>, PredSet>& why_point_ancestor_cache,
        std::map<std::pair<Segment*, Segment*>, PredSet>& why_segment_ancestor_cache
    );

    
    /* Given a line `l` and a direction `d`, identifies the children `cl` and `cd` such
    that `cl` was assigned direction `cd` (as recorded in `direction_of_lines`), and
    the PredSet constructed from the addition of
    - direction_of_lines[cd][cl]
    - why_ancestor(cd, d)
    - why_ancestor(cl, l)
    is the smallest possible. */
    std::pair<std::pair<Direction*, Line*>, PredSet> most_explainable_direction_of_line(
        Line* l, Direction* d,
        std::map<std::pair<Direction*, Direction*>, PredSet>& why_direction_ancestor_cache,
        std::map<std::pair<Line*, Line*>, PredSet>& why_line_ancestor_cache
    );
    /* Given a segment `s` and a length `len`, identifies the children `cs` and `clen` such
    that `cs` was assigned length `clen` (as recorded in `length_of_segments`), and the
    PredSet constructed from the addition of
    - length_of_segments[clen][cs]
    - why_ancestor(clen, len)
    - why_ancestor(cs, s)
    is the smallest possible. */
    std::pair<std::pair<Length*, Segment*>, PredSet> most_explainable_length_of_segment(
        Segment* s, Length* len,
        std::map<std::pair<Length*, Length*>, PredSet>& why_length_ancestor_cache,
        std::map<std::pair<Segment*, Segment*>, PredSet>& why_segment_ancestor_cache
    );


    /* Given an angle `a` and a measure `m`, identifies the children `ca` and `cm` such
    that `ca` was assigned measure `cm` (as recorded in `measure_of_angles`), and the
    PredSet constructed from the addition of
    - measure_of_angles[cm][ca]
    - why_ancestor(cm, m)
    - why_ancestor(ca, a)
    is the smallest possible. */
    std::pair<std::pair<Measure*, Angle*>, PredSet> most_explainable_measure_of_angle(
        Angle* a, Measure* m,
        std::map<std::pair<Measure*, Measure*>, PredSet>& why_measure_ancestor_cache,
        std::map<std::pair<Angle*, Angle*>, PredSet>& why_angle_ancestor_cache
    );

    /* Given a ratio `r` and a fraction `f`, identifies the children `cr` and `cf` such
    that `cr` was assigned fraction `cf` (as recorded in `fraction_of_ratios`), and the
    PredSet constructed from the addition of
    - fraction_of_ratios[cf][cr]
    - why_ancestor(cf, f)
    - why_ancestor(cr, r)
    is the smallest possible. */
    std::pair<std::pair<Fraction*, Ratio*>, PredSet> most_explainable_fraction_of_ratio(
        Ratio* r, Fraction* f,
        std::map<std::pair<Fraction*, Fraction*>, PredSet>& why_fraction_ancestor_cache,
        std::map<std::pair<Ratio*, Ratio*>, PredSet>& why_ratio_ancestor_cache
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

    void populate_why(PredSet pred);

    void get_solution(std::stringstream &ss);
};