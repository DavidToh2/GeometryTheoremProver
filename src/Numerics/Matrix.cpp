
#include "Matrix.hh"
#include "Numerics.hh"
#include "Common/Exceptions.hh"

Matrix::Matrix(int m, int n) {
    this->m = m;
    this->n = n;
    this->m_s = (m < 5) ? 5 : m;
    this->n_s = (n < 20) ? 20 : n;
    this->m_e = (m < 20) ? m : 20;
    this->n_e = (n < 20) ? n : 20;
    this->data = std::vector<double>(m_s * n_s, 0);
}


double Matrix::get(int i, int j) const {
    if (__in_bounds(i, j)) return data[i * n_s + j];
    throw NumericsError("Matrix index out of bounds");
}
void Matrix::set(int i, int j, double value) {
    if (__in_bounds(i, j)) data[i * n_s + j] = value;
    else throw NumericsError("Matrix index out of bounds");
}

double& Matrix::operator()(int i, int j) {
    if (__in_bounds(i, j)) return data[i * n_s + j];
    throw NumericsError("Matrix index out of bounds");
}
const double& Matrix::operator()(int i, int j) const {
    if (__in_bounds(i, j)) return data[i * n_s + j];
    throw NumericsError("Matrix index out of bounds");
}

int Matrix::extend_rows(int i) {
    m = m + i;
    if (m > m_s) {
        // increase the number of rows by m_e
        while (m > m_s) m_s += m_e;
        data.resize(m_s * n_s);
        m_e = m < 5 ? m : 5;
    }
    return i;
}
int Matrix::extend_columns(int j) {
    int n_old = n;
    int n_d_old = n_s - n;
    int n_s_old = n_s;
    n = n + j;
    int c = 0;
    if (n > n_s) {
        // increase the number of columns by n_e
        while (n > n_s) n_s += n_e;
        data.resize(m_s * n_s);

        // shift existing data to new positions
        int n_d = n_s - n_old;
        int row = m;
        auto it = data.rbegin() + (m_s - m) * n_s;
        auto jt = data.rbegin() + m_s * (n_s - n_s_old) + (m_s - m) * n_s_old;
        while (row > 0) {
            it += n_d;
            jt += n_d_old;
            for (int col = n_old; col > 0; col--) {
                std::iter_swap(it++, jt++);
            }
            row--;
        }
        n_e = n < 20 ? n : 20;
    }
    return j;
}
int Matrix::extend_rows(Matrix& other) {
    if (other.n != n) return -1;
    int c = extend_rows(other.m);
    for (int i = 0; i < other.m; i++) {
        for (int j = 0; j < n; j++) {
            this->set(i+m-other.m, j, other(i, j));
        }
    }
    return c;
}
int Matrix::extend_columns(Matrix& other) {
    if (other.m != m) return -1;
    int c = extend_columns(other.n);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < other.n; j++) {
            this->set(i, j+n-other.n, other(i, j));
        }
    }
    return c;
}

std::string Matrix::__print_matrix() const {
    std::string repr = "[";
    for (int i = 0; i < m; i++) {
        repr += "[ ";
        for (int j = 0; j < n; j++) {
            repr += Frac(data[i * n_s + j]).to_string() + ", ";
        }
        repr += "]\n";
    }
    repr += "]";
    return repr;
}

std::string Matrix::__print_full_matrix() const {
    std::string repr = "[";
    for (int i = 0; i < m_s; i++) {
        repr += "[ ";
        for (int j = 0; j < n_s; j++) {
            repr += Frac(data[i * n_s + j]).to_string() + ", ";
        }
        repr += "]\n";
    }
    repr += "]";
    return repr;
}