
#include "doctest.h"

#include "Numerics/Matrix.hh"
#include "Numerics/Numerics.hh"
#include "Common/Exceptions.hh"

TEST_SUITE("Matrix") {
    TEST_CASE("Explicit get and set") {
        Matrix mat(3, 3);
        mat.set(0, 0, Frac(1, 2));
        mat.set(1, 1, Frac(3, 4));
        mat.set(2, 2, Frac(5, 6));

        CHECK(mat.get(0, 0) == Frac(1, 2));
        CHECK(mat.get(1, 1) == Frac(3, 4));
        CHECK(mat.get(2, 2) == Frac(5, 6));
        CHECK(mat.get(0, 1) == Frac(0));
        CHECK(mat.get(0, 2) == Frac(0));
        CHECK(mat.get(1, 0) == Frac(0));
        CHECK(mat.get(1, 2) == Frac(0));
        CHECK(mat.get(2, 0) == Frac(0));
        CHECK(mat.get(2, 1) == Frac(0));

        CHECK_THROWS_AS(mat.get(3, 0), NumericsError);
        CHECK_THROWS_AS(mat.set(0, 3, Frac(1)), NumericsError);
    }

    TEST_CASE("Operator get and set") {
        Matrix mat(3, 3);
        mat(0, 0) = Frac(1);
        mat(1, 1) = Frac(2);
        mat(2, 2) = Frac(3, 2);

        CHECK(mat(0, 0) == Frac(1));
        CHECK(mat(1, 1) == Frac(2));
        CHECK(mat(2, 2) == Frac(3, 2));
        CHECK(mat(0, 1) == Frac(0));
        CHECK(mat(0, 2) == Frac(0));
        CHECK(mat(1, 0) == Frac(0));
        CHECK(mat(1, 2) == Frac(0));
        CHECK(mat(2, 0) == Frac(0));
        CHECK(mat(2, 1) == Frac(0));
        CHECK_THROWS_AS(mat(2, 3) = Frac(4), NumericsError);
    }

    TEST_CASE("Matrix extend rows") {
        Matrix mat(2, 3);
        mat(0, 0) = Frac(1);
        mat(0, 1) = Frac(2);
        mat(0, 2) = Frac(3);
        mat(1, 0) = Frac(4);
        mat(1, 1) = Frac(5);
        mat(1, 2) = Frac(6);

        CHECK_THROWS_AS(mat(1, 3) = Frac(7), NumericsError);

        mat.extend_rows(2);

        CHECK(mat(0, 0) == Frac(1));
        CHECK(mat(2, 0) == Frac(0));
        CHECK(mat(2, 1) == Frac(0));
        CHECK(mat(2, 2) == Frac(0));
        CHECK(mat(3, 0) == Frac(0));
        CHECK(mat(3, 1) == Frac(0));
        CHECK(mat(3, 2) == Frac(0));

        mat(2, 2) = Frac(7);
        mat(3, 2) = Frac(8);
        CHECK(mat(2, 2) == Frac(7));
        CHECK(mat(3, 2) == Frac(8));
        CHECK_THROWS_AS(mat(4, 2) = Frac(9), NumericsError);

        CHECK_THROWS_AS(mat(2, 3) = Frac(7), NumericsError);
        CHECK_THROWS_AS(mat(3, 3) = Frac(8), NumericsError);
    }

    TEST_CASE("Matrix extend columns") {
        Matrix mat(3, 2);
        mat(0, 0) = Frac(1);
        mat(1, 0) = Frac(2);
        mat(2, 0) = Frac(3);
        mat(0, 1) = Frac(4);
        mat(1, 1) = Frac(5);
        mat(2, 1) = Frac(6);

        CHECK_THROWS_AS(mat(0, 2) = Frac(7), NumericsError);

        mat.extend_columns(2);

        CHECK(mat(0, 0) == Frac(1));
        CHECK(mat(0, 2) == Frac(0));
        CHECK(mat(1, 2) == Frac(0));
        CHECK(mat(2, 2) == Frac(0));
        CHECK(mat(0, 3) == Frac(0));
        CHECK(mat(1, 3) == Frac(0));
        CHECK(mat(2, 3) == Frac(0));

        mat(1, 2) = Frac(7);
        mat(2, 3) = Frac(8);
        CHECK(mat(1, 2) == Frac(7));
        CHECK(mat(2, 3) == Frac(8));
        CHECK_THROWS_AS(mat(2, 4) = Frac(9), NumericsError);

        CHECK_THROWS_AS(mat(3, 3) = Frac(7), NumericsError);
        CHECK_THROWS_AS(mat(3, 4) = Frac(8), NumericsError);
    }
}