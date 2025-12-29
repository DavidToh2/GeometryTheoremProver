
#include "Highs.h"
#include <vector>

#include "Numerics/Matrix.hh"
#include "Numerics/Numerics.hh"

/* Linear program solver wrapper class.

Solve the linear program `min c^T * x` subject to `A * x = b, x >= 0`.

Documentation: 
- https://ergo-code.github.io/HiGHS/dev/ 
- https://github.com/ERGO-Code/HiGHS/blob/master/examples/call_highs_from_cpp.cpp */
class LinProg {
public:
    Highs highs;
    HighsModel model;

    int last;

    LinProg();

    void populate(
        const SparseMatrix& A,
        const std::vector<double>& b,
        const std::vector<double>& c
    );

    bool solve(
        std::vector<double>& result
    );

    std::string __print_matrix_A() const;
};