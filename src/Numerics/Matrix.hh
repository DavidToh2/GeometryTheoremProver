#pragma once

#include "Numerics.hh"
#include <vector>

/* Matrix class. 

Here, `m` is the number of rows and `n` the number of columns that are
currently in use. 

Element access and modification can be done via `mat(i, j)`.

To extend the number of rows or columns, call `mat.extend_rows()` 
or `mat.extend_columns()` respectively. Rows can also be extended with
those of another matrix (with an equal no. of columns). 

To print the matrix for debugging purposes, call `__print_matrix()` 
or `__print_full_matrix()`.

Internals: `m_s, n_s` are the number of allocated rows and columns. `m_e, n_e` 
are the increments for rows and columns when extending the matrix size. */
class Matrix {
public:
    int m;
    int n;
    int m_s;
    int n_s;
    int m_e;
    int n_e;
    std::vector<double> data;

    Matrix(int m, int n);

    inline bool __in_bounds(int i, int j) const {
        return (i >= 0) && (i < m) && (j >= 0) && (j < n);
    }
    double get(int i, int j) const;
    void set(int i, int j, double value);

    double& operator()(int i, int j);
    const double& operator()(int i, int j) const;

    int extend_rows(int i);
    int extend_columns(int j);
    int extend_rows(Matrix& other);
    int extend_columns(Matrix& other);

    std::string __print_matrix() const;
    std::string __print_full_matrix() const;
};