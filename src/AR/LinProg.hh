
#include <Highs.h>
#include <vector>

#include "Numerics/Matrix.hh"
#include "Numerics/Numerics.hh"

/* Linear program solver wrapper class.

Documentation: 
- https://ergo-code.github.io/HiGHS/dev/ 
- https://github.com/ERGO-Code/HiGHS/blob/master/examples/call_highs_from_cpp.cpp */
class LinProg {
public:
    HighsModel model;

    int last;

    /* */
    void solve(
        const SparseMatrix& A,
        const SparseMatrix& b,
        const std::vector<double>& c
    );
};