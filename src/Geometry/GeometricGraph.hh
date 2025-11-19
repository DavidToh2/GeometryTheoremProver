#pragma once

#include <map>
#include <memory>
#include <ostream>
#include <iostream>

#include "Object.hh"
#include "Object2.hh"
#include "Value.hh"
#include "DD/DDEngine.hh"

template<typename T>    // "alias declaration"
using ptrmap = std::map<std::string, std::unique_ptr<T>>;

class GeometricGraph {

public:
    // Geometric objects

    ptrmap<Point> points;
    ptrmap<Line> lines;
    ptrmap<Circle> circles;
    ptrmap<Triangle> triangles;
    ptrmap<Quadrilateral> quadrilaterals;

    ptrmap<Angle> angles;
    ptrmap<Segment> segments;
    ptrmap<Ratio> ratios;

    ptrmap<Measure> measures;
    ptrmap<Length> lengths;
    ptrmap<Fraction> fractions;

    void __add_point(const std::string point_id);
    void try_add_point(const std::string point_id);
    Point* get_or_add_point(const std::string point_id);

    void __add_line(const std::string p1, const std::string p2);
    void try_add_line(const std::string p1, const std::string p2);
    Line* get_or_add_line(const std::string p1, const std::string p2);

    /* Add points to lines, circles, triangles or quadrilaterals (latter two TBA).

    Create new lines or circles as necessary here. */
    void add_point_to_line(const std::string p, const std::string l);
    void add_point_to_circle(const std::string p, const std::string c);
    void add_points_to_objects(DDEngine &dd);

    /* Unify points which have been shown to be identical. */
    void unify_points(DDEngine &dd);
    void unify_objects(DDEngine &dd);
    
    void __print_points(std::ostream &os = std::cout);
};