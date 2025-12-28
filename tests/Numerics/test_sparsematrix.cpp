
#include "doctest.h"
#include <iostream>

#include "Numerics/Matrix.hh"
#include "Common/Exceptions.hh"

TEST_SUITE("SparseMatrix") {
    
    TEST_CASE("Explicit get and set") {
        SparseMatrix mat(5, 5, 3);
        
        for (int i=0; i<2; i++) {
            for (int j=0; j<3; j++) {
                mat.set(i, j, 3*i + j + 1);
            }
        }

        bool all_pass_1 = true;
        for (int j=0; j<3; j++) {
            for (int i=0; i<2; i++) {
                all_pass_1 = all_pass_1 && (mat.get(i, j) == 3*i + j + 1);
            }
            all_pass_1 = all_pass_1 && (mat.get(2, j) == 0);
        }
        CHECK(all_pass_1);

        for (int i=0; i<2; i++) {
            for (int j=0; j<3; j++) {
                mat.set(i, j, 0);
            }
        }
        for (int i=2; i<5; i++) {
            for (int j=0; j<5; j++) {
                mat.set(i, j, 5*i + j + 1);
            }
        }
        CHECK_THROWS_AS(mat.set(5, 0, 1), NumericsError);
        CHECK_THROWS_AS(mat.get(0, 5), NumericsError);

        bool all_pass_2 = true;
        for (int j=0; j<5; j++) {
            for (int i=0; i<2; i++) {
                all_pass_2 = all_pass_2 && (mat.get(i, j) == 0);
            }
            for (int i=2; i<5; i++) {
                all_pass_2 = all_pass_2 && (mat.get(i, j) == 5*i + j + 1);
            }
        }
        CHECK(all_pass_2);

        bool all_pass_3 = true;
        for (int j=0; j<5; j++) {
            CHECK(mat.row_indices[j].size() == 3);
            CHECK(mat.values[j].size() == 3);
        }
    }

    TEST_CASE("Column limits") {
        SparseMatrix mat(3, 3, 2);
        
        for (int i=0; i<2; i++) {
            for (int j=0; j<2; j++) {
                mat.set(i, j, 3*i + j + 1);
            }
        }
        CHECK_FALSE(mat.set(2, 0, 1));

        CHECK(mat.set(1, 1));
        for (int i=2; i>0; i--) {
            for (int j=2; j>0; j--) {
                if (i==1 && j==1) {
                    CHECK_FALSE(mat.set(i, j, 3*i + j + 1));
                } else {
                    CHECK(mat.set(i, j, 3*i + j + 1));
                }
            }
        }

        bool all_pass = true;
        
        for (auto& [i, j] : std::vector<std::pair<int, int>>{
            {0,0}, {0,1}, {1,0}, {1,2}, {2,1}, {2,2}
        }) {
            all_pass = all_pass && (mat.get(i, j) == 3*i + j + 1);
        }
        
        for (int i=0; i<3; i++) {
            all_pass = all_pass && (mat.get(i, 2-i) == 0);
        }
        CHECK(all_pass);
    }

    TEST_CASE("Extend rows and columns") {
        SparseMatrix mat(2, 2, 5);
        for (int i=0; i<2; i++) {
            for (int j=0; j<2; j++) {
                mat.set(i, j, 2*i + j + 1);
            }
        }

        CHECK(mat.extend_rows(3) == 3);
        CHECK(mat.extend_columns(2) == 2);

        for (int i=0; i<5; i++) {
            for (int j=0; j<4; j++) {
                mat.set(i, j, 10*i + j + 1);
            }
        }

        SparseMatrix mat_other_1(4, 2, 4);
        for (int i=0; i<4; i++) {
            for (int j=0; j<2; j++) {
                mat_other_1.set(i, j, 10*i + j + 5);
            }
        }
        CHECK(mat.extend_columns(mat_other_1) == 2);


        CHECK(mat.set(4, 4, 45));
        CHECK(mat.set(4, 5, 46));
        mat.extend_rows(2);

        SparseMatrix mat_other_2(6, 2, 6);
        CHECK(mat.extend_columns(mat_other_2) == 0);
        SparseMatrix mat_other_3(8, 2, 4);
        CHECK(mat.extend_columns(mat_other_3) == 0);

        for (int i=0; i<4; i++) {
            mat.set(i, i);
            mat.set(i+1, i);
        }
        for (int j=0; j<4; j++) {
            CHECK(mat.set(5, j, 51 + j));
            CHECK(mat.set(6, j, 61 + j));
        }

        // for (int i=0; i<7; i++) {
        //     for (int j=0; j<6; j++) {
        //        std::cout << mat.get(i, j) << ",";
        //     }
        //     std::cout << std::endl;
        // }

        bool all_pass = true;
        for (int j=0; j<6; j++) {
            for (int i=0; i<7; i++) {
                if (j < 4) {
                    if (i == j || i == j+1) {
                        all_pass = all_pass && (mat.get(i, j) == 0);
                    } else {
                        all_pass = all_pass && (mat.get(i, j) == 10*i + j + 1);
                    }
                } else if (i < 5) {
                    all_pass = all_pass && (mat.get(i, j) == 10*i + j + 1);
                } else {
                    all_pass = all_pass && (mat.get(i, j) == 0);
                }
            }
        }
        CHECK(all_pass);
    }
}