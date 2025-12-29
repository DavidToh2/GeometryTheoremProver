
#include "doctest.h"

#include "Numerics/Matrix.hh"
#include "Common/Exceptions.hh"

TEST_SUITE("Matrix") {
    
    /* [1, 2, 3,
        4, 5, 6,
        7, 8, 9] */
    TEST_CASE("Explicit get and set") {
        Matrix mat(3, 3);
        
        for (int i=0; i<2; i++) {
            for (int j=0; j<3; j++) {
                mat.set(i, j, 3*i + j + 1);
            }
        }

        bool all_pass = true;
        for (int j=0; j<3; j++) {
            for (int i=0; i<2; i++) {
                all_pass = all_pass && (mat.get(i, j) == 3*i + j + 1);
            }
            all_pass = all_pass && (mat.get(2, j) == 0);
        }
        CHECK(all_pass);

        CHECK_THROWS_AS(mat.get(3, 0), NumericsError);
        CHECK_THROWS_AS(mat.set(0, 3, 1), NumericsError);
    }
    /* [1, 2, 3,
        4, 5, 6,
        7, 8, 9] */
    TEST_CASE("Operator get and set") {
        Matrix mat(3, 3);
        for (int i=0; i<2; i++) {
            for (int j=0; j<3; j++) {
                mat(i, j) = 3*i + j + 1;
            }
        }

        bool all_pass = true;
        for (int j=0; j<3; j++) {
            for (int i=0; i<2; i++) {
                all_pass = all_pass && (mat(i, j) == 3*i + j + 1);
            }
            all_pass = all_pass && (mat(2, j) == 0);
        }
        CHECK(all_pass);

        CHECK_THROWS_AS(mat(3, 0), NumericsError);
        CHECK_THROWS_AS(mat(0, 3) = 1, NumericsError);
    }

    /* [1,  2,  3,
        4,  5,  6]
        concat
       [12, 13, 14,
        18, 19, 20]*/
    TEST_CASE("Matrix extend rows") {
        Matrix mat(2, 3);
        for (int i=0; i<2; i++) {
            for (int j=0; j<3; j++) {
                mat(i, j) = 3*i + j + 1;
            }
        }

        CHECK_THROWS_AS(mat(2, 0), NumericsError);
        CHECK_THROWS_AS(mat(0, 3) = 7, NumericsError);

        mat.extend_rows(2);

        bool all_pass = true;
        for (int i=0; i<2; i++) {
            for (int j=0; j<3; j++) {
                all_pass = all_pass && (mat(i, j) == 3*i + j + 1);
            }
        }
        for (int i=2; i<4; i++) {
            for (int j=0; j<3; j++) {
                all_pass = all_pass && (mat(i, j) == 0);
            }
        }

        for (int i=2; i<4; i++) {
            for (int j=0; j<3; j++) {
                mat(i, j) = 6*i + j + 1;
            }
        }
        for (int i=0; i<2; i++) {
            for (int j=0; j<3; j++) {
                all_pass = all_pass && (mat(i, j) == 3*i + j + 1);
            }
        }
        for (int i=2; i<4; i++) {
            for (int j=0; j<3; j++) {
                all_pass = all_pass && (mat(i, j) == 6*i + j + 1);
            }
        }

        CHECK(all_pass);
        CHECK_THROWS_AS(mat(4, 2), NumericsError);
        CHECK_THROWS_AS(mat(2, 3), NumericsError);
        CHECK_THROWS_AS(mat(3, 3) = 1, NumericsError);
    }
    /* [1,  2,       [3,  4,
        5,  6, concat 7,  8,
        9, 10]        11, 12] */
    TEST_CASE("Matrix extend columns") {
        Matrix mat(3, 2);
        for (int i=0; i<3; i++) {
            for (int j=0; j<2; j++) {
                mat(i, j) = 4*i + j + 1;
            }
        }

        CHECK_THROWS_AS(mat(0, 2) = 7, NumericsError);
        CHECK_THROWS_AS(mat(3, 0), NumericsError);

        mat.extend_columns(2);

        bool all_pass = true;
        for (int i=0; i<3; i++) {
            for (int j=0; j<2; j++) {
                all_pass = all_pass && (mat(i, j) == 4*i + j + 1);
            }
            for (int j=2; j<4; j++) {
                all_pass = all_pass && (mat(i, j) == 0);
            }
        }

        for (int i=0; i<3; i++) {
            for (int j=2; j<4; j++) {
                mat(i, j) = 4*i + j + 1;
            }
        }
        for (int i=0; i<3; i++) {
            for (int j=0; j<4; j++) {
                all_pass = all_pass && (mat(i, j) == 4*i + j + 1);
            }
        }

        CHECK(all_pass);
        CHECK_THROWS_AS(mat(2, 4) , NumericsError);
        CHECK_THROWS_AS(mat(3, 3) , NumericsError);
        CHECK_THROWS_AS(mat(3, 4) = 1, NumericsError);
    }

    /* [1,  2,  3,
        4,  5,  6]
        concat
       [7,  8,  9,
        10, 11, 12]*/
    TEST_CASE("Matrix extend rows with other matrix") {
        Matrix mat(2, 3);
        for (int i=0; i<2; i++) {
            for (int j=0; j<3; j++) {
                mat(i, j) = 3*i + j + 1;
            }
        }

        Matrix mat_other(2, 3);
        for (int i=0; i<2; i++) {
            for (int j=0; j<3; j++) {
                mat_other(i, j) = 3*(i+2) + j + 1;
            }
        }

        CHECK(mat.extend_rows(mat_other) == 2);

        bool all_pass = true;
        for (int i=0; i<4; i++) {
            for (int j=0; j<3; j++) {
                all_pass = all_pass && (mat(i, j) == 3*i + j + 1);
            }
        }
        CHECK(all_pass);
        CHECK_THROWS_AS(mat(4, 0) = 7, NumericsError);
        CHECK_THROWS_AS(mat(0, 3) = 7, NumericsError);
        CHECK_THROWS_AS(mat(2, 3) = 7, NumericsError);
    }
    /* [1,  2,       [3,  4,
        5,  6, concat 7,  8,
        9, 10]        11, 12] */
    TEST_CASE("Matrix extend columns with other matrix") {
        Matrix mat(3, 2);
        for (int i=0; i<3; i++) {
            for (int j=0; j<2; j++) {
                mat(i, j) = 4*i + j + 1;
            }
        }

        Matrix mat_other(3, 2);
        for (int i=0; i<3; i++) {
            for (int j=0; j<2; j++) {
                mat_other(i, j) = 4*i + j + 3;
            }
        }

        CHECK(mat.extend_columns(mat_other) == 2);

        bool all_pass = true;
        for (int i=0; i<3; i++) {
            for (int j=0; j<4; j++) {
                all_pass = all_pass && (mat(i, j) == 4*i + j + 1);
            }
        }
        CHECK(all_pass);
        CHECK_THROWS_AS(mat(2, 4) = 7, NumericsError);
        CHECK_THROWS_AS(mat(3, 0) = 7, NumericsError);
        CHECK_THROWS_AS(mat(3, 2) = 7, NumericsError);
    }

    /* 19x19 -> extend_rows to 24x19 -> extend_columns to 24x22 */
    TEST_CASE("Matrix expand") {
        Matrix mat(19, 19);
        for (int i = 0; i < 19; i++) {
            for (int j = 0; j < 19; j++) {
                mat(i, j) = i * 100 + j + 1;
            }
        }

        Matrix mat_other_1(5, 19);
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 19; j++) {
                mat_other_1(i, j) = (i + 19) * 100 + j + 1;
            }   
        }

        Matrix mat_other_2(24, 3);
        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 3; j++) {
                mat_other_2(i, j) = i * 100 + (j + 19) + 1;
            }   
        }

        CHECK(mat.extend_rows(mat_other_1) == 5);
        CHECK(mat.extend_columns(mat_other_2) == 3);
        
        bool all_pass = true;
        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 22; j++) {
                all_pass = all_pass && (mat(i, j) == i * 100 + j + 1);
            }
        }
        CHECK(all_pass);
        CHECK_THROWS_AS(mat(24, 1) = 7, NumericsError);
        CHECK_THROWS_AS(mat(1, 22) = 7, NumericsError);
        CHECK_THROWS_AS(mat(24, 22) = 7, NumericsError);
    }
}