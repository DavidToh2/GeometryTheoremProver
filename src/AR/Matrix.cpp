
#include "Matrix.hh"
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
    throw ARInternalError("Matrix index out of bounds");
}
void Matrix::set(int i, int j, double value) {
    if (__in_bounds(i, j)) data[i * n_s + j] = value;
    else throw ARInternalError("Matrix index out of bounds");
}

double& Matrix::operator()(int i, int j) {
    if (__in_bounds(i, j)) return data[i * n_s + j];
    throw ARInternalError("Matrix index out of bounds");
}
const double& Matrix::operator()(int i, int j) const {
    if (__in_bounds(i, j)) return data[i * n_s + j];
    throw ARInternalError("Matrix index out of bounds");
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
    if (other.n != n) return 0;
    int c = extend_rows(other.m);
    for (int i = 0; i < other.m; i++) {
        for (int j = 0; j < n; j++) {
            this->set(i+m-other.m, j, other(i, j));
        }
    }
    return c;
}
int Matrix::extend_columns(Matrix& other) {
    if (other.m != m) return 0;
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




double SparseMatrix::get(int i, int j) const {
    if (__in_bounds(i, j)) {
        for (int idx = 0; idx < s; idx++) {
            if (row_indices[j][idx] == i) {
                return values[j][idx];
            }
        }
        return 0.0;
    }
    throw ARInternalError("SparseMatrix index out of bounds");
}
bool SparseMatrix::set(int i, int j, double value) {
    if (__in_bounds(i, j)) {
        int nearest_empty = -1;
        for (int idx = 0; idx < s; idx++) {
            if (row_indices[j][idx] == i) {
                if (value) {
                    values[j][idx] = value;
                } else {
                    row_indices[j][idx] = -1;
                    values[j][idx] = 0.0;
                }
                return true;
            }
            if (nearest_empty < 0 && row_indices[j][idx] == -1) {
                nearest_empty = idx;
            }
        }
        if (nearest_empty >= 0) {
            if (value) {
                row_indices[j][nearest_empty] = i;
                values[j][nearest_empty] = value;
            }
            return true;
        }
        return false;
    }
    throw ARInternalError("SparseMatrix index out of bounds");
}
int SparseMatrix::extend_rows(int i) {
    m += i;
    return i;
}
int SparseMatrix::extend_columns(int j) {
    n += j;
    row_indices.resize(n);
    values.resize(n);
    for (int col = n - j; col < n; col++) {
        row_indices[col] = std::vector<int>(s, -1);
        values[col] = std::vector<double>(s, 0.0);
    }
    return j;
}
int SparseMatrix::extend_columns(std::map<int, double>& col_data) {
    return this->extend_columns(std::move(col_data));
}
int SparseMatrix::extend_columns(std::map<int, double>&& col_data) {
    row_indices.emplace_back(std::vector<int>(s, -1));
    values.emplace_back(std::vector<double>(s, 0.0));
    int idx = 0;
    for (const auto& [i, v] : col_data) {
        row_indices[n][idx] = i;
        values[n][idx] = v;
        idx++;
        if (idx >= s) break;
    }
    n += 1;
    return 1;
}
int SparseMatrix::extend_columns(SparseMatrix& other) {
    if (other.m > m) return 0;
    if (other.s > s) return 0;
    for (int j = 0; j < other.n; j++) {
        other.row_indices[j].resize(s, -1);
        other.values[j].resize(s, 0.0);
        row_indices.emplace_back(std::move(other.row_indices[j]));
        values.emplace_back(std::move(other.values[j]));
    }
    n += other.n;
    return other.n;
}

std::string SparseMatrix::__print_matrix() const {
    std::string repr = "[";
    for (int i = 0; i < m; i++) {
        repr += "[ ";
        for (int j = 0; j < n; j++) {
            double d = this->get(i, j);
            Frac f = Frac(d);
            repr += f.to_string() + ", ";
        }
        repr += "]\n";
    }
    repr += "]";
    return repr;
}