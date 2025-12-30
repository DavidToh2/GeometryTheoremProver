#pragma once

#include <map>
#include <memory>
#include <ostream>
#include <iostream>
#include <set>

#include "DD/Predicate.hh"
#include "Object.hh"
#include "Object2.hh"
#include "Value.hh"
#include "Value2.hh"
#include "DD/DDEngine.hh"
#include "AR/AREngine.hh"

template<typename T>    // "alias declaration"
using uptrmap = std::map<std::string, std::unique_ptr<T>>;

template<typename T>
using ptrset = std::set<T*>;

class GeometricGraph {

public:
    int adhoc = 0;

    // Geometric objects

    uptrmap<Point> points;
    uptrmap<Line> lines;
    uptrmap<Circle> circles;
    uptrmap<Segment> segments;
    uptrmap<Triangle> triangles;

    uptrmap<Direction> directions;
    uptrmap<Length> lengths;
    uptrmap<Shape> shapes;

    uptrmap<Angle> angles;
    uptrmap<Ratio> ratios;

    uptrmap<Measure> measures;
    uptrmap<Fraction> fractions;

    // Root geometric objects

    ptrset<Point> root_points;
    ptrset<Line> root_lines;
    ptrset<Circle> root_circles;
    ptrset<Segment> root_segments;
    ptrset<Triangle> root_triangles;

    ptrset<Direction> root_directions;
    ptrset<Length> root_lengths;
    ptrset<Shape> root_shapes;

    ptrset<Angle> root_angles;
    ptrset<Ratio> root_ratios;

    ptrset<Measure> root_measures;
    ptrset<Fraction> root_fractions;

    void __add_new_point(const std::string point_id);
    void __try_add_point(const std::string point_id);
    Point* get_or_add_point(const std::string point_id);

    /* Merges the root of `src` point into the root of `dest` point. 
    Postcondition: After the merge, all elements of `get_root(dest)` 's `on_line` and `on_circle` have the
    reason for the merge `pred` appended to their `why` s.
    Postcondition: After the merge, all lines and circles formerly storing `get_root(src)` in their `points` maps
    will now store `get_root(dest)` instead. This is to ensure that the `points` maps only ever contains root
    points. */
    void merge_points(Point* dest, Point* src, Predicate* pred);


    /* Adds a line connecting the (existing) points `p1` and `p2`.
    Populates the `points` of the new line with `p1` and `p2`, and adds the new line to their `on_line`. 
    Note: `p1` and `p2` should be root points. */
    Line* __add_new_line(Point* p1, Point* p2, Predicate* base_pred);
    /* Gets the root line connecting the (existing) points `p1` and `p2`. 
    Returns `nullptr` if no such line exists yet. 
    Note: `p1` and `p2` must be root points (so that their `on_line` attribute is nonempty). */
    Line* __try_get_line(Point* p1, Point* p2);
    /* Gets the root line connecting the roots of the points `p1` and `p2`, or creates a new line connecting
    them if none exists. */
    Line* get_or_add_line(Point* p1, Point* p2, DDEngine &dd);
    /* Gets the root line connecting the roots of the points `p1` and `p2`. 
    Returns `nullptr` if no such line exists yet. */
    Line* try_get_line(Point* p1, Point* p2);

    /* Merges `src` line into `dest` line. 
    When the lines are merged, `dest` receives the roots of all `points` from `src` using `merge_maps_with_roots()`. */
    void merge_lines(Line* dest, Line* src, Predicate* pred);


    /* Add a new direction to the line `l`.
    Note: Assumes that `l` is a root node. 
    Note: Assumes that `l` does not already have a direction set. (If it does, then the old direction is
    overwritten: see `Line::set_direction()` for details.)*/
    Direction* __add_new_direction(Line* l, Predicate* base_pred);
    /* Gets the root direction of the root node of line `l`, or creates a new direction for the root of `l` if none
    yet exists. */
    Direction* get_or_add_direction(Line* l, DDEngine &dd);

    /* Sets `dest` and `src` to be parallel by merging `root_src` into `root_dest`, as well as `root_src->perp` 
    into `root_dest->perp` (if both exist - see `Direction::merge()` for more info). */
    void set_directions_para(Direction* dest, Direction* src, Predicate* pred);
    /* Sets `d1` and `d2` to be perpendicular by merging `root_d1->perp` into `root_d2`, as well as `root_d2->perp`
    into `root_d1` (whenever exists - see `Direction::set_perp()` for more info). */
    void set_directions_perp(Direction* d1, Direction* d2, Predicate* pred);


    /* Adds the circumcircle of the triangle formed by the points `p1`, `p2`, p3`. 
    Note: `p1`, `p2`, `p3` should be root points. */
    Circle* __add_new_circle(Point* p1, Point* p2, Point* p3, Predicate* base_pred);
    /* Adds the circle with center `c` passing through `p1`. 
    Note: `p1` should be a root point. */
    Circle* __add_new_circle(Point* c, Point* p1, Predicate* base_pred);
    /* Add a circle with center `c`. */
    Circle* __add_new_circle(Point* c, Predicate* base_pred);

    /* Gets the root circumcircle of the three points `p1`, `p2`, `p3`.
    Returns `nullptr` if no such circle exists.
    Note: `p1`, `p2`, `p3` must be root points. */
    Circle* __try_get_circle(Point* p1, Point* p2, Point* p3);
    /* Gets the root circle centered at `c` passing through `p1`.
    Returns `nullptr` if no such circle exists.
    Note: `p1` must be a root point. */
    Circle* __try_get_circle(Point* c, Point* p1);
    /* Gets the two circumcircles of `p1p2p3` and `p1p2p4` respectively. 
    A more time-efficient method than calling `__try_get_circle()` twice. 
    Returns `nullptr` in either or both values if the corresponding circumcircle does not exist. 
    Note: All points must be root points. 
    Note: It is entirely possible that both returned pointers point to the exact same circle. */
    std::pair<Circle*, Circle*> __try_get_circles(Point* p1, Point* p2, Point* p3, Point* p4);

    /* Gets the root circumcircle of the roots of the three points `p1`, `p2`, `p3`. 
    Returns `nullptr` if no such circle exists.*/
    Circle* try_get_circle(Point* p1, Point* p2, Point* p3);
    /* Ges the root circle centered at `c` passing through the root of `p1`. 
    Returns `nullptr` if no such circle exists. */
    Circle* try_get_circle(Point* c, Point* p1);
    /* Gets the two circumcircles of the root points of `p1p2p3` and `p1p2p4` respectively. 
    A more time-efficient method than calling `__try_get_circle()` twice. 
    Returns `nullptr` in either or both values if the corresponding circumcircle does not exist. */
    std::pair<Circle*, Circle*> try_get_circles(Point* p1, Point* p2, Point* p3, Point* p4);

    /* Gets the root circumcircle of the roots of the three points `p1`, `p2`, `p3`, creating a new
    circle if it does not yet exist. */
    Circle* get_or_add_circle(Point* p1, Point* p2, Point* p3, DDEngine& dd);
    /* Gets the root circle centered at `c` passing through `p1`, creating a new circle if it does 
    not yet exist. */
    Circle* get_or_add_circle(Point* c, Point* p1, DDEngine& dd);
    /* Gets the center of a given circle `c`, creating a new point as this center if it does not yet
    exist. */
    Point* get_or_add_circle_center(Circle* c, DDEngine& dd);

    /* Merges the root of `src` circle into the root of `dest` circle. */
    void merge_circles(Circle* dest, Circle* src, Predicate* pred);


    /* Adds the angle with first direction `d1` and second direction `d2`.
    Note: Assumes that `d1` and `d2` are root directions. */
    Angle* __add_new_angle(Direction* d1, Direction* d2, Predicate* base_pred);
    /* Adds the angle with first line `l1` and second line `l2`, creating `direction` s for these two
    lines as necessary. 
    Note: Assumes that `l1` and `l2` are root lines. */
    Angle* __add_new_angle(Line* l1, Line* l2, Predicate* base_pred);
    /* Adds the angle with first line `p1p2` and second line `p3p4`, creating `direction` s for these two
    lines as necessary.
    Lines are fetched by `__try_get_line()`, so this method is not guaranteed to succeed.
    Throws an error if either `p1p2` or `p3p4` does not yet exist.
    Note: This method is not used. */
    Angle* __add_new_angle(Point* p1, Point* p2, Point* p3, Point* p4, Predicate* base_pred);
    /* Adds the angle with first line `p1p2` and second line `p2p3`, creating `direction` s for these two
    lines as necessary.
    Lines are fetched by `__try_get_line()`, so this method is not guaranteed to succeed.
    Throws an error if either `p1p2` or `p2p3` does not yet exist.
    Note: This method is not used. */
    Angle* __add_new_angle(Point* p1, Point* p2, Point* p3, Predicate* base_pred);

    /* Gets the root angle with `direction1 == d1` and `direction2 == d2`. 
    Returns `nullptr` if no such angle yet exists. */
    Angle* __try_get_angle(Direction* d1, Direction* d2);
    /* Gets the root angle with `direction1 == l1.direction` and `direction2 == l2.direction`. 
    Returns `nullptr` if no such angle yet exists. 
    Note: `l1` and `l2` should be root lines. */
    Angle* __try_get_angle(Line* l1, Line* l2);
    /* Gets the root angle with first line `p1p2` and second line `p3p4` using their directions.
    Lines are fetched by `__try_get_line()`. 
    Returns `nullptr` if either of the lines, or the angle itself, does not yet exist. 
    Note: `p1, p2, p3, p4` should all be root points.
    Note: This method is not used. */ 
    Angle* __try_get_angle(Point* p1, Point* p2, Point* p3, Point* p4);
    /* Gets the root angle with first line `p1p2` and second line `p2p3` using their directions.
    Lines are fetched by `__try_get_line()`. 
    Returns `nullptr` if either of the lines, or the angle itself, does not yet exist. 
    Note: `p1, p2, p3` should all be root points.
    Note: This method is not used. */
    Angle* __try_get_angle(Point* p1, Point* p2, Point* p3);

    /* Gets the root angle with `direction1 == d1` and `direction2 == d2`. 
    Returns `nullptr` if no such angle yet exists. */
    Angle* try_get_angle(Direction* d1, Direction* d2);
    /* Gets the root angle with `direction1 == root_l1.direction` and `direction2 == root_l2.direction`
    Returns `nullptr` if no such angle yet exists. */
    Angle* try_get_angle(Line* l1, Line* l2);
    /* Gets the root angle with first line `rp1rp2` and second line `rp3rp4` using their directions.
    Lines are fetched by `try_get_line()`. 
    Returns `nullptr` if either of the lines, or the angle itself, does not yet exist. */
    Angle* try_get_angle(Point* p1, Point* p2, Point* p3, Point* p4);
    /* Gets the root angle with first line `rp1rp2` and second line `rp2rp3` using their directions.
    Lines are fetched by `try_get_line()`. 
    Returns `nullptr` if either of the lines, or the angle itself, does not yet exist. */
    Angle* try_get_angle(Point* p1, Point* p2, Point* p3);

    /* Gets the root angle with `direction1 == root_l1.direction` and `direction2 == root_l2.direction`, creating
    this new angle if it does not yet exist. */
    Angle* get_or_add_angle(Line* l1, Line* l2, DDEngine& dd);
    /* Gets the root angle with first line `p1p2` and second line `p3p4`. These lines are fetched 
    by `get_or_add_line()`, so are guaranteed to be root lines.
    The angle is created if it does not yet exist. */ 
    Angle* get_or_add_angle(Point* p1, Point* p2, Point* p3, Point* p4, DDEngine& dd);
    /* Gets the root angle with first line `p1p2` and second line `p2p3`. These lines are fetched
    by `get_or_add_line()`, so are guaranteed to be root lines.
    The angle is created if it does not yet exist. */
    Angle* get_or_add_angle(Point* p1, Point* p2, Point* p3, DDEngine& dd);

    /* Merges the root of `src` angle into the root of `dest` angle. */
    void merge_angles(Angle* dest, Angle* src, Predicate* pred);

    /* Add a new measure to the angle `a`.
    Note: Assumes that `a` is a root node. 
    Note: Assumes that `a` does not already have a measure set. (If it does, then the old measure is
    overwritten: see `Angle::set_measure()` for details.) */
    Measure* __add_new_measure(Angle* a, Predicate* base_pred);
    /* Gets the root measure of the root node of angle `a`, or creates a new measure for the root of `a` if none
    yet exists. */
    Measure* get_or_add_measure(Angle* a, DDEngine& dd);
    /* Sets measures `dest` and `src` equal by merging the root of `src` into the root of `dest`. */
    void set_measures_equal(Measure* dest, Measure* src, Predicate* pred);



    /* Check collinearity. This is done by checking if the root lines `p1p2` and `p1p3` are identical. */
    bool check_coll(Point* p1, Point* p2, Point* p3);
    /* Check if the root node of p1 lies on the root node of l. See `Line::contains()` for more info. */
    bool check_coll(Point* p1, Line* l);

    bool check_cyclic(Point* p1, Point* p2, Point* p3, Point* p4);

    bool check_cyclic(Point* p1, Circle* c);

    bool check_para(Point* p1, Point* p2, Point* p3, Point* p4);
    bool check_para(Point* p1, Point* p2, Line* l1);
    bool check_para(Line* l1, Line* l2);

    bool check_perp(Point* p1, Point* p2, Point* p3, Point* p4);
    bool check_perp(Point* p1, Point* p2, Line* l1);
    bool check_perp(Line* l1, Line* l2);

    bool check_eqangle(Point* p1, Point* p2, Point* p3, Point* p4, Point* p5, Point* p6, Point* p7, Point* p8);
    bool check_eqangle(Point* p1, Point* p2, Point* p3, Point* p4, Point* p5, Point* p6);
    bool check_eqangle(Line* l1, Line* l2, Line* l3, Line* l4);
    bool check_eqangle(Angle* a1, Angle* a2);

    bool check_circle(Point* c, Point* p1, Point* p2, Point* p3);
    bool check_circle(Point* c, Circle* circ);

    bool check_postcondition(PredicateTemplate* pred);



    bool make_coll(Predicate* pred, DDEngine &dd, AREngine &ar);
    bool make_cyclic(Predicate* pred, DDEngine &dd, AREngine &ar);
    bool make_para(Predicate* pred, DDEngine &dd, AREngine &ar, bool do_ar);
    bool make_perp(Predicate* pred, DDEngine &dd, AREngine &ar, bool do_ar);
    bool make_cong(Predicate* pred, DDEngine &dd, AREngine &ar, bool do_ar);
    bool make_eqangle(Predicate* pred, DDEngine &dd, AREngine &ar, bool do_ar);
    bool make_eqratio(Predicate* pred, DDEngine &dd, AREngine &ar, bool do_ar);
    bool make_contri(Predicate* pred, DDEngine &dd, AREngine &ar);
    bool make_simtri(Predicate* pred, DDEngine &dd, AREngine &ar);
    bool make_circle(Predicate* pred, DDEngine &dd, AREngine &ar);
    bool make_constangle(Predicate* pred, DDEngine &dd, AREngine &ar, bool do_ar);
    bool make_constratio(Predicate* pred, DDEngine &dd, AREngine &ar, bool do_ar);



    /* Synthesize new geometric objects based on recently added predicates.
    Note: All `make_` functions should be idempotent. */
    void synthesise_preds(DDEngine &dd, AREngine &ar, bool do_ar);
    void synthesise_pred2s(DDEngine &dd);



    Predicate* why(Predicate* pred, DDEngine &dd);


    
    void __print_points(std::ostream &os);
    void __print_lines(std::ostream &os);
    void __print_circles(std::ostream &os);
    void __print_directions(std::ostream &os);
    void __print_angles(std::ostream &os);
    void __print_measures(std::ostream &os);

    void print(std::ostream &os = std::cout);

    void reset_problem();
};