#pragma once

#include <map>
#include <memory>
#include <ostream>
#include <iostream>
#include <set>

#include "Object.hh"
#include "Object2.hh"
#include "Value.hh"
#include "Value2.hh"
#include "DD/DDEngine.hh"
#include "Common/Generator.hh"

template<typename T>    // "alias declaration"
using uptrmap = std::map<std::string, std::unique_ptr<T>>;

template<typename T>
using ptrset = std::set<T*>;

class GeometricGraph {

public:
    // Geometric objects

    uptrmap<Point> points;
    uptrmap<Line> lines;
    uptrmap<Circle> circles;
    uptrmap<Triangle> triangles;
    uptrmap<Quadrilateral> quadrilaterals;

    uptrmap<Angle> angles;
    uptrmap<Segment> segments;
    uptrmap<Ratio> ratios;

    uptrmap<Direction> directions;
    uptrmap<Cong> congs;
    uptrmap<Sim> sims;

    uptrmap<Measure> measures;
    uptrmap<Length> lengths;
    uptrmap<Fraction> fractions;

    // Root geometric objects

    ptrset<Point> root_points;
    ptrset<Line> root_lines;
    ptrset<Circle> root_circles;
    ptrset<Triangle> root_triangles;
    ptrset<Quadrilateral> root_quadrilaterals;

    ptrset<Angle> root_angles;
    ptrset<Segment> root_segments;
    ptrset<Ratio> root_ratios;

    ptrset<Direction> root_directions;
    ptrset<Cong> root_congs;
    ptrset<Sim> root_sims;

    ptrset<Measure> root_measures;
    ptrset<Length> root_lengths;
    ptrset<Fraction> root_fractions;

    void __add_new_point(const std::string point_id);
    void __try_add_point(const std::string point_id);
    Point* get_or_add_point(const std::string point_id);

    /* Merges `get_root(src)` point into `get_root(dest)` point. 
    
    Postcondition: After the merge, all elements of `get_root(dest)` 's `on_line` and `on_circle` have the
    reason for the merge `pred` appended to their `why` s.
    Postcondition: After the merge, all lines and circles formerly storing `get_root(src)` in their `points` maps
    will now store `get_root(dest)` instead. This is to ensure that the `points` maps only ever contains root
    points.
    Note: There is no assumption that `dest` and `src` have to be root points here. */
    void __merge_points(Point* dest, Point* src, Predicate* pred);

    /* Adds a line connecting the (existing) points `p1` and `p2`.
    Populates the `points` of the new line with `p1` and `p2`, and adds the new line to their `on_line`. */
    Line* __add_new_line(Point* p1, Point* p2, Predicate* base_pred);

    /* Gets the root line connecting the (existing) points `p1` and `p2`. 
    Returns `nullptr` if no such line exists yet. 
    
    Precondition: `p1` and `p2` must be root points (so that their `on_line` attribute is nonempty). */
    Line* __try_get_line(Point* p1, Point* p2);

    /* Gets the root line connecting the (existing) points `p1` and `p2`, or creates a new line connecting
    them if none exists. */
    Line* get_or_add_line(Point* p1, Point* p2, DDEngine &dd);

    /* Gets the root line connecting the (existing) points `p1` and `p2`. 
    Returns `nullptr` if no such line exists yet. */
    Line* try_get_line(Point* p1, Point* p2);

    /* Merges `src` line into `dest` line. 
    When the lines are merged, `dest` receives the roots of all `points` from `src` using `merge_maps_with_roots()`. 
    
    Precondition: Assumes that `dest` and `src` are already roots. This is guaranteed by `__try_get_line()`. */
    void __merge_lines(Line* dest, Line* src, Predicate* pred);



    void __add_new_direction(Line* l, Predicate* pred);
    void add_new_direction(Line* l, Predicate* pred);
    void __merge_directions(Direction* dest, Direction* src, Predicate* pred);



    void make_collinear(Predicate* pred, DDEngine &dd);
    void make_cyclic(Predicate* pred, DDEngine &dd);
    void make_para(Predicate* pred, DDEngine &dd);
    void make_perp(Predicate* pred, DDEngine &dd);


    /* Check collinearity. This is done by checking if the root lines `p1p2` and `p1p3` are identical.
    
    Preconditions: See the invariants for `make_collinear()`. */
    bool check_collinear(Point* p1, Point* p2, Point* p3);
    /* Check if the point p1 lies on the line l.
    
    Precondition: l should be a root line. */
    bool check_collinear(Point* p1, Line* l);

    bool check_cyclic(Point* p1, Point* p2, Point* p3, Point* p4);

    /* Synthesize new geometric objects based on recently added predicates. */
    void synthesise_objects(DDEngine &dd);
    void synthesise_object2s(DDEngine &dd);

    /* Link objects which are related. */
    void link_lines(DDEngine &dd);
    

    /* Unify nodes which have been shown to be identical. */
    void unify_points(DDEngine &dd);
    void unify_objects(DDEngine &dd);
    void unify_values(DDEngine &dd);
    
    void __print_points(std::ostream &os = std::cout);
};