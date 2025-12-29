
#include <doctest.h>
#include "Highs.h"
#include <iostream>

#include "AR/LinProg.hh"
#include "Numerics/Matrix.hh"
#include "Numerics/Numerics.hh"

TEST_SUITE("LinProg") {
    TEST_CASE("Exact integer solution") {
        LinProg lp;
        SparseMatrix A(2, 2, 4);
        A.set(0, 0, 1);
        A.set(0, 1, 1);
        A.set(1, 0, 1);
        A.set(1, 1, -1);

        std::vector<double> b = {4, 0};
        std::vector<double> c = {1, 1};

        lp.populate(A, b, c);

        std::vector<double> result;
        bool success = lp.solve(result);

        CHECK(success);
        CHECK(result.size() == 2);
        CHECK(std::abs(result[0] - 2.0) < Frac::TOL);
        CHECK(std::abs(result[1] - 2.0) < Frac::TOL);
    }

    TEST_CASE("No solution") {
        LinProg lp;
        SparseMatrix A(2, 2, 4);
        A.set(0, 0, 1);
        A.set(0, 1, 1);
        A.set(1, 0, 1);
        A.set(1, 1, -1);

        std::vector<double> b = {4, 1};
        std::vector<double> c = {1, 1};

        lp.populate(A, b, c);

        std::vector<double> result;
        bool success = lp.solve(result);

        CHECK_FALSE(success);
    }
}