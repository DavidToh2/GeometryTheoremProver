
#include <Highs.h>
#include <vector>

#include "LinProg.hh"
#include "Numerics/Matrix.hh"
#include "Numerics/Numerics.hh"

void LinProg::solve(
    const SparseMatrix& A,
    const SparseMatrix& b,
    const std::vector<double>& c
) {
    model.lp_.num_col_ = A.n;
    model.lp_.num_row_ = A.m;
    model.lp_.sense_ = ObjSense::kMinimize;
    model.lp_.col_cost_ = c;
    model.lp_.col_lower_ = {0.0, 1.0};
    model.lp_.col_upper_ = {4.0, 1.0e30};
    model.lp_.row_lower_ = {-1.0e30, 5.0, 6.0};
    model.lp_.row_upper_ = {7.0, 15.0, 1.0e30};
}