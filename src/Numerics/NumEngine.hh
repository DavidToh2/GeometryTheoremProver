#pragma once

#include "Numerics.hh"
#include "Cartesian.hh"
#include "Common/Frac.hh"
#include "Geometry/Object.hh"

class NumEngine {
public:
    std::vector<std::unique_ptr<Numeric>> numerics;

    std::map<Point*, std::vector<Point*>> depends_on;
    std::map<Point*, std::vector<Point*>> depended_by;
    std::map<Point*, std::vector<Numeric*>> nums;

    std::map<Point*, std::vector<EqPoint>> point_coords;


    Numeric* insert_numeric(std::unique_ptr<Numeric>&& num);
};