#pragma once

#include <map>
#include <memory>

#include "Object.hh"
#include "Object2.hh"
#include "Value.hh"

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
    
};