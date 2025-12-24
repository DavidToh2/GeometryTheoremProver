
#include "Numerics.hh"
#include <vector>
#include <array>

/* Matrix class. 

Here, `m` is the number of rows and `n` the number of columns. */
class Matrix {
public:
    int m;
    int n;
    int m_s;
    int n_s;
    int m_e;
    int n_e;
    std::vector<Frac> data;

    Matrix(int m, int n);

    inline bool __in_bounds(int i, int j) const {
        return (i >= 0) && (i < m) && (j >= 0) && (j < n);
    }
    Frac get(int i, int j) const;
    void set(int i, int j, Frac value);

    Frac& operator()(int i, int j);
    const Frac& operator()(int i, int j) const;

    bool extend_rows(int i);
    bool extend_columns(int j);
    bool extend_rows(Matrix& other);

    std::string __print_matrix() const;
    std::string __print_full_matrix() const;
};