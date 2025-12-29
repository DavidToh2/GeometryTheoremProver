
#include "Highs.h"
#include <vector>

#include "LinProg.hh"
#include "Numerics/Matrix.hh"

LinProg::LinProg() : last(0) {
    highs.setHighsOptionValue("log_to_console", false);
    model.lp_.a_matrix_.start_ = {0};
}

void LinProg::populate(
    const SparseMatrix& A,
    const std::vector<double>& b,
    const std::vector<double>& c
) {
    model.lp_.num_col_ = A.n;
    model.lp_.num_row_ = A.m;
    model.lp_.sense_ = ObjSense::kMinimize;
    model.lp_.col_cost_ = c;
    model.lp_.col_lower_ = std::vector<double>(A.n, 0.0);
    model.lp_.col_upper_ = std::vector<double>(A.n, 1.0e30);
    model.lp_.row_lower_ = b;
    model.lp_.row_upper_ = b;

    model.lp_.a_matrix_.format_ = MatrixFormat::kColwise;
    while (last < A.n) {
        for (int k=0; k<A.s; k++) {
            if (A.row_indices[last][k] >= 0) {
                model.lp_.a_matrix_.index_.emplace_back(A.row_indices[last][k]);
                model.lp_.a_matrix_.value_.emplace_back(A.values[last][k]);
            }
        }
        model.lp_.a_matrix_.start_.emplace_back(model.lp_.a_matrix_.value_.size());
        last++;
    }
}

bool LinProg::solve(
    std::vector<double>& result
) {
    HighsStatus status;
    status = highs.passModel(model);
    if (status != HighsStatus::kOk) {
        std::cout << "LinProg::solve: passModel failed\n";
        return false;
    }
    status = highs.run();
    if (status != HighsStatus::kOk) {
        std::cout << "LinProg::solve: run failed\n";
        return false;
    }
    HighsInfo info = highs.getInfo();
    if (!info.primal_solution_status) {
        std::cout << "LinProg::solve: no primal solution\n";
        return false;
    }
    HighsSolution sol = highs.getSolution();
    result = sol.col_value;
    return true;
}

std::string LinProg::__print_matrix_A() const {
    for (int k=0; k<model.lp_.a_matrix_.num_col_; k++) {
        int i = model.lp_.a_matrix_.start_[k], j = model.lp_.a_matrix_.start_[k+1];
        while (i < j) {
            std::cout << "A[" << model.lp_.a_matrix_.index_[i] << "," << k << "] = " << model.lp_.a_matrix_.value_[i] << "; ";
            i++;
        }
        std::cout << "\n";
    }
}