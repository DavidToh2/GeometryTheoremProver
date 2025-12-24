
#include "Matrix.hh"
#include "Numerics.hh"
#include "Common/Exceptions.hh"

Matrix::Matrix(int m, int n) {
    this->m = m;
    this->n = n;
    this->m_s = (m < 20) ? 20 : m;
    this->n_s = (n < 20) ? 20 : n;
    this->m_e = (m < 20) ? m : 20;
    this->n_e = (n < 20) ? n : 20;
    this->data = std::vector<Frac>(m_s * n_s, Frac());
}


Frac Matrix::get(int i, int j) const {
    if (__in_bounds(i, j)) return data[i * m_s + j];
    throw NumericsError("Matrix index out of bounds");
}
void Matrix::set(int i, int j, Frac value) {
    if (__in_bounds(i, j)) data[i * m_s + j] = value;
    else throw NumericsError("Matrix index out of bounds");
}

Frac& Matrix::operator()(int i, int j) {
    if (__in_bounds(i, j)) return data[i * m_s + j];
    throw NumericsError("Matrix index out of bounds");
}
const Frac& Matrix::operator()(int i, int j) const {
    if (__in_bounds(i, j)) return data[i * m_s + j];
    throw NumericsError("Matrix index out of bounds");
}

bool Matrix::extend_rows(int i) {
    m = m + i;
    if (m > m_s) {
        // increase the number of rows by m_e
        m_s = m_s + m_e;
        data.resize(m_s * n_s);
        m_e = m < 20 ? m : 20;
    }
    return true;
}
bool Matrix::extend_columns(int j) {
    n = n + j;
    if (n > n_s) {
        // increase the number of columns by n_e
        n_s = n_s + n_e;
        data.resize(m_s * n_s);

        // shift existing data to new positions
        int row = m, n_s_old = n_s - n_e;
        auto it = data.begin() + m * n_s_old;
        auto jt = data.end();
        while (row > 0) {
            jt -= n_e;
            for (int col = n_s_old - 1; col >= 0; col--) {
                std::swap(*(--it), *(--jt));
            }
            row--;
        }
        n_e = n < 20 ? n : 20;
    }
    return true;
}
bool Matrix::extend_rows(Matrix& other) {
    if (other.n != n) return false;
    extend_rows(other.m);
    auto pos = data.begin() + (m - other.m) * n_s;
    data.insert(pos, other.data.begin(), other.data.begin() + other.m * n_s);
    return true;
}