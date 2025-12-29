
#include "Highs.h"
#include <vector>

#include "LinProg.hh"
#include "Numerics/Matrix.hh"

LinProg::LinProg(
    bool verbose
) : last(0) {
    highs.setOptionValue("log_to_console", verbose);
    model.lp_.a_matrix_.start_ = {0};
    model.lp_.sense_ = ObjSense::kMinimize;
    model.lp_.a_matrix_.format_ = MatrixFormat::kColwise;
}

void LinProg::populate(
    const SparseMatrix& A,
    const std::vector<double>& b,
    const std::vector<double>& c
) {
    populate_matrix_A(A);
    populate_target(b);
    populate_cost(c);
}

void LinProg::populate_matrix_A(
    const SparseMatrix& A
) {
    model.lp_.num_col_ = A.n;
    model.lp_.num_row_ = A.m;
    
    model.lp_.col_lower_ = std::vector<double>(A.n, 0.0);
    model.lp_.col_upper_ = std::vector<double>(A.n, 1.0e30);

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
void LinProg::populate_target(
    const std::vector<double>& b
) {
    model.lp_.row_lower_ = b;
    model.lp_.row_upper_ = b;
}

void LinProg::populate_cost(
    const std::vector<double>& c
) {
    model.lp_.col_cost_ = c;
}

bool LinProg::solve(
    std::vector<double>& result,
    bool verbose
) {
    HighsStatus status;
    status = highs.passModel(model);
    if (status != HighsStatus::kOk) {
        if (verbose) std::cout << "LinProg::solve: passModel failed with status " << static_cast<int>(status) << "\n";
        return false;
    }
    status = highs.run();
    if (status != HighsStatus::kOk) {
        if (verbose) std::cout << "LinProg::solve: run failed with status " << static_cast<int>(status) << "\n";
        return false;
    }
    HighsInfo info = highs.getInfo();
    if (!info.primal_solution_status) {
        if (verbose) std::cout << "LinProg::solve: no primal solution\n";
        return false;
    }
    HighsSolution sol = highs.getSolution();
    result = sol.col_value;
    return true;
}

std::string LinProg::__print_matrix_A() const {
    std::string s;
    for (int k=0; k<model.lp_.a_matrix_.num_col_; k++) {
        int i = model.lp_.a_matrix_.start_[k], j = model.lp_.a_matrix_.start_[k+1];
        while (i < j) {
            s += "A[" + std::to_string(model.lp_.a_matrix_.index_[i]) + "," + std::to_string(k) + "] = " + std::to_string(model.lp_.a_matrix_.value_[i]) + "; ";
            i++;
        }
        s += "\n";
    }
    return s;
}

std::string LinProg::__print_result(
    const std::vector<double>& result
) {
    std::string s = "[";
    for (size_t i = 0; i < result.size(); i++) {
        s += std::to_string(result[i]);
        if (i < result.size() - 1) {
            s += ", ";
        }
    }
    s += "]";
    return s;
}