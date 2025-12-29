
#include <doctest.h>
#include <iostream>

#include "AR/LinProg.hh"
#include "Numerics/Matrix.hh"
#include "Numerics/Numerics.hh"

TEST_SUITE("LinProg") {
    /* A = [1, 1,
            1, -2] */
    TEST_CASE("Simple test case with exact integer solution") {
        LinProg lp;
        SparseMatrix A(2, 2, 4);
        A.set(0, 0, 1);
        A.set(0, 1, 1);
        A.set(1, 0, 1);
        A.set(1, 1, -2);

        std::vector<double> b = {4, 0};
        std::vector<double> c = {1, 1};

        lp.populate(A, b, c);

        std::vector<double> result;
        bool success = lp.solve(result);

        CHECK(success);
        CHECK(result.size() == 2);
        CHECK(__is_close(result[0], 2.66666667));
        CHECK(__is_close(result[1], 1.33333333));
    }

    /* A = [1,  1,  0,  0,
            -2, 0,  0,  1,
            1,  2,  1,  0,
            0,  0,  1,  1,
            0,  -3, 0,  -2]
    Note: solved with (r1, r2, r3, r4, r5) = (11, 5, -1, 1, 3)
    Expect solutions for the following:
    - r1 + 2*r3 + 3*r4 - 4*r5
    Expect no solutions for the following:
    - r1 + 5*r3 - 2*r5 (has a solution with a negative coefficient)
    - r2 + r3 - r5
    - r1 - r2 + 2*r3 - r4 - r5 (has a solution with a negative coefficient) */
    TEST_CASE("More complex test case") {
        LinProg lp(false);
        SparseMatrix A(5, 0, 3);
        A.extend_columns({{0, 1}, {1, -2}, {2, 1}});
        A.extend_columns({{0, 1}, {2, 2}, {4, -3}});
        A.extend_columns({{2, 1}, {3, 1}});
        A.extend_columns({{1, 1}, {3, 1}, {4, -2}});

        std::vector<double> c = {1, 1, 1, 1};

        SUBCASE("Solution for r1 + 2*r3 + 3*r4 - 4*r5") {
            std::vector<double> b = {1, 0, 2, 3, -4};
            lp.populate(A, b, c);

            std::vector<double> result;
            bool success = lp.solve(result);
            CHECK(success);
            CHECK(result.size() == 4);
            CHECK(result == std::vector<double>{1, 0, 1, 2});
        }
        SUBCASE("No solution for r1 + 5*r3 - 2*r5") {
            std::vector<double> b = {1, 0, 5, 0, -2};
            lp.populate(A, b, c);

            std::vector<double> result;
            bool success = lp.solve(result);
            CHECK_FALSE(success);
        }
        SUBCASE("No solution for r2 + r3 - r5 and r1 - r2 + 2*r3 - r4 - r5") {
            std::vector<double> b = {0, 1, 1, 0, -1};
            lp.populate(A, b, c);

            std::vector<double> result;
            bool success = lp.solve(result);
            CHECK_FALSE(success);

            b = {1, -1, 2, -1, -1};
            lp.populate_target(b);

            success = lp.solve(result);
            CHECK_FALSE(success);
        }
    }

    /* A = [1, 1, 0, 0, 0, 0,
            1, 0, 1, 1, 1, 0,
            1, 1, 0, 1, 0, 1,
            1, 1, 1, 0, 1, 0, 
            0, 1, 1, 0, 1, 1, 
            0, 0, 0, 0, 1, 1] 
    Take   [1,-1, 1,-1,-1, 1] copies of the columns to get 0
    HiGHS returns a consistent result even in the case of multicollinearity.
    */
    TEST_CASE("Test case with linear dependence") {
        LinProg lp;
        SparseMatrix A(6, 0, 4);
        A.extend_columns({{0,1}, {1,1}, {2,1}, {3,1}});
        A.extend_columns({{0,1}, {2,1}, {3,1}, {4,1}});
        A.extend_columns({{1,1}, {3,1}, {4,1}});
        A.extend_columns({{1,1}, {2,1}});
        A.extend_columns({{1,1}, {3,1}, {4,1}, {5,1}});
        A.extend_columns({{2,1}, {4,1}, {5,1}});

        std::vector<double> b = {1, 2, 2, 2, 2, 1};
        std::vector<double> c = {1, 1, 1, 1, 1, 1};

        lp.populate(A, b, c);

        std::vector<double> result;
        std::vector<double> old_result;
        lp.solve(old_result);

        for (int i=0; i<3; i++) {
            bool success = lp.solve(result);
            CHECK(success);
            CHECK(result == old_result);
            // std::cout << LinProg::__print_result(result) << std::endl;
            old_result = result;
        }
    }
}