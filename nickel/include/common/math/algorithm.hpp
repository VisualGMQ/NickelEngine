#pragma once

#include "common/math/matrix.hpp"
#include "common/math/view.hpp"
#include <iostream>
#include <optional>
#include <utility>


namespace nickel {

template <typename T, bool IsConst>
auto operator+(MatrixView<T, IsConst> m1, MatrixView<T, IsConst> m2) noexcept {
    assert(m1.RowNum() == m2.RowNum());
    assert(m1.ColNum() == m2.ColNum());
    Matrix m(m1.ColNum(), m1.RowNum());
    for (int c = 0; c < m1.ColNum(); c++) {
        for (int r = 0; r < m2.RowNum(); r++) {
            m[c][r] = m1[c][r] + m2[c][r];
        }
    }
    return m;
}

template <typename T, bool IsConst>
auto operator-(MatrixView<T, IsConst> m1, MatrixView<T, IsConst> m2) noexcept {
    assert(m1.RowNum() == m2.RowNum());
    assert(m1.ColNum() == m2.ColNum());
    Matrix m(m1.ColNum(), m1.RowNum());
    for (int c = 0; c < m1.ColNum(); c++) {
        for (int r = 0; r < m2.RowNum(); r++) {
            m[c][r] = m1[c][r] - m2[c][r];
        }
    }
    return m;
}

template <typename T, bool IsConst>
auto operator*(MatrixView<T, IsConst> m1, double value) noexcept {
    Matrix m(m1.ColNum(), m1.RowNum());
    for (int c = 0; c < m1.ColNum(); c++) {
        for (int r = 0; r < m1.RowNum(); r++) {
            m[c][r] = m1[c][r] * value;
        }
    }
    return m;
}

template <typename T, bool IsConst>
auto operator*(double value, MatrixView<T, IsConst> m1) noexcept {
    return m1 * value;
}

template <typename T, bool IsConst>
auto operator/(MatrixView<T, IsConst> m1, double value) noexcept {
    Matrix m(m1.ColNum(), m1.RowNum());
    for (int c = 0; c < m1.ColNum(); c++) {
        for (int r = 0; r < m1.RowNum(); r++) {
            m[c][r] = m1[c][r] / value;
        }
    }
    return m;
}

template <typename T, bool IsConst>
std::ostream& operator<<(std::ostream& o, MatrixView<T, IsConst> view) {
    o << "[";
    for (int r = 0; r < view.RowNum(); r++) {
        for (int c = 0; c < view.ColNum(); c++) {
            o << view[c][r] << " ";
        }
        o << std::endl;
    }
    o << "]";
    return o;
}

//
// @brief A_m_n x B_n_r
//
template <typename T, bool IsConst>
auto operator*(MatrixView<T, IsConst> m1, MatrixView<T, IsConst> m2) {
    assert(m1.ColNum() == m2.RowNum());
    auto M = m1.RowNum();
    auto N = m1.ColNum();
    auto R = m2.ColNum();
    Matrix<T> mat(R, M);
    for (int m = 0; m < M; m++) {
        for (int r = 0; r < R; r++) {
            double sum = 0;
            for (int n = 0; n < N; n++) {
                sum += m1[n][m] * m2[r][n];
            }
            mat[r][m] = sum;
        }
    }
    return mat;
}

template <typename T, bool IsConst = true>
auto Transpose(MatrixView<T, IsConst> m) {
    Matrix mat(m.RowNum(), m.ColNum());
    for (int c = 0; c < m.ColNum(); c++) {
        for (int r = 0; r < m.RowNum(); r++) {
            mat[r][c] = m[c][r];
        }
    }
    return mat;
}

template <typename T, bool IsConst = true>
double Det(MatrixView<T, IsConst> m) {
    assert(m.IsSquare());
    auto len = m.RowNum();
    if (len == 1) {
        return m[0][0];
    }
    if (len == 2) {
        return m[0][0] * m[1][1] - m[0][1] * m[1][0];
    }
    if (len == 3) {
        return m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] +
               m[1][0] * m[2][1] * m[0][2] -
               (m[2][0] * m[1][1] * m[0][2] + m[1][0] * m[0][1] * m[2][2] +
                m[0][0] * m[2][1] * m[1][2]);
    }

    double result = 0;
    for (int c = 0; c < len; c++) {
        Matrix<T> temp(len - 1, len - 1);
        for (int idx = 0; idx < len; idx++) {
            if (idx == c) continue;
            for (int r = 1; r < len; r++) {
                temp[idx < c ? idx : idx - 1][r - 1] = m[idx][r];
            }
        }
        result += ((c % 2 == 0) ? 1 : -1) * m[c][0] * Det(MatrixView{temp});
    }
    return result;
}

template <typename T, bool IsConst = true>
auto Cofactor(MatrixView<T, IsConst> m, size_t col, size_t row) {
    Matrix<T> mat(m.ColNum() - 1, m.RowNum() - 1);
    for (int c = 0; c < m.ColNum(); c++) {
        if (c == col) continue;
        for (int r = 0; r < m.RowNum(); r++) {
            if (r == row) continue;
            mat[c < col ? c : c - 1][r < row ? r : r - 1] = m[c][r];
        }
    }
    return mat;
}

template <typename T, bool IsConst = true>
auto Adjoint(MatrixView<T, IsConst> m) {
    Matrix mat(m.ColNum(), m.RowNum());
    for (int c = 0; c < m.ColNum(); c++) {
        for (int r = 0; r < m.RowNum(); r++) {
            mat[c][r] = (((c + r) % 2 == 0) ? 1 : -1) * Det(Cofactor(m, c, r));
        }
    }
    return mat;
}

/**
 * @brief please make sure m has inverse matrix
 */
template <typename T, bool IsConst>
auto Inverse(MatrixView<T, IsConst> m) {
    return Adjoint(m) / Det(m);
}

enum class ReducedRowEchelonFormPolicy {
    DontStop,
    StopWhenAllZeroElems,
};

template <typename T>
void RowEchelonForm(MatrixView<T, false> m) {
    size_t minNum = std::min(m.RowNum(), m.ColNum());
    for (int i = 0; i < minNum; i++) {
        // rearrange
        size_t swapRow = i;
        for (int r = i + 1; r < m.RowNum(); r++) {
            if (std::abs(m[i][i]) < std::abs(m[i][r])) {
                swapRow = r;
            }
        }

        for (int c = 0; c < m.ColNum(); c++) {
            std::swap(m[c][i], m[c][swapRow]);
        }

        // gaussian elimination
        for (int r = i + 1; r < m.RowNum(); r++) {
            double value = m[i][r] / m[i][i];
            for (int k = i; k < m.ColNum(); k++) {
                m[k][r] -= value * m[k][i];
            }
        }
    }
}

/**
 * @brief require m is RowEchelonForm
 * @return bool  whether the algorithm is completed
 */
template <typename T>
bool ReducedRowEchelonForm(MatrixView<T, false> m,
                           ReducedRowEchelonFormPolicy policy) {
    for (int r = m.RowNum() - 1; r >= 0; r--) {
        int lastNozeroIdx = 0;
        while (lastNozeroIdx < m.ColNum() && m[lastNozeroIdx][r] == 0) {
            lastNozeroIdx++;
        }
        if (lastNozeroIdx == m.ColNum()) {
            if (policy == ReducedRowEchelonFormPolicy::StopWhenAllZeroElems) {
                return false;
            }
            continue;
        }

        for (int nr = r - 1; nr >= 0; nr--) {
            double value = m[lastNozeroIdx][nr] / m[lastNozeroIdx][r];
            for (int c = lastNozeroIdx; c < m.ColNum(); c++) {
                m[c][nr] -= value * m[c][r];
            }
        }
        double fstNozeroElem = m[lastNozeroIdx][r];
        for (int c = lastNozeroIdx; c < m.ColNum(); c++) {
            m[c][r] /= fstNozeroElem;
        }
    }
    return true;
}

/**
 * @brief solve linear equations by Gaussian-Jordan-Elimination inplace
 * @note m is Augmented matrix
 */
template <typename T>
std::optional<Matrix<T>> Solve(MatrixView<T, false> m) {
    if (m.ColNum() != m.RowNum() + 1) {
        return std::nullopt;
    }

    RowEchelonForm(m);
    if (!ReducedRowEchelonForm(
            m, ReducedRowEchelonFormPolicy::StopWhenAllZeroElems)) {
        return std::nullopt;
    }

    Matrix<T> result{1, m.RowNum()};
    for (int i = 0; i < m.RowNum(); i++) {
        result[0][i] = m[m.ColNum() - 1][i];
    }
    return result;
}

template <typename T>
std::optional<Matrix<T>> Solve(MatrixView<T, true> m) {
    if (m.ColNum() != m.RowNum() + 1) {
        return std::nullopt;
    }

    auto mat = m.Clone();
    return Solve(MatrixView<T, false>(mat));
}

// wrappers

template <typename T>
Matrix<T> operator+(const Matrix<T>& m1, const Matrix<T>& m2) noexcept {
    return MatrixView(m1) + MatrixView(m2);
}

template <typename T>
Matrix<T> operator-(const Matrix<T>& m1, const Matrix<T>& m2) noexcept {
    return MatrixView(m1) - MatrixView(m2);
}

template <typename T>
Matrix<T> operator*(const Matrix<T>& m1, double value) noexcept {
    return MatrixView(m1) * value;
}

template <typename T>
Matrix<T> operator*(double value, const Matrix<T>& m1) noexcept {
    return m1 * value;
}

template <typename T>
Matrix<T> operator/(const Matrix<T>& m1, double value) noexcept {
    return MatrixView(m1) / value;
}

template <typename T>
Matrix<T> operator*(const Matrix<T>& m1, const Matrix<T>& m2) {
    return MatrixView{m1} * MatrixView{m2};
}

template <typename T>
Matrix<T> Transpose(const Matrix<T>& m) {
    return Transpose(MatrixView{m});
}

template <typename T>
double Det(const Matrix<T>& m) {
    return Det(MatrixView(m));
}

template <typename T>
Matrix<T> Cofactor(const Matrix<T>& m, size_t col, size_t row) {
    return Cofactor(MatrixView(m), col, row);
}

template <typename T>
Matrix<T> Adjoint(const Matrix<T>& m) {
    return Adjoint(MatrixView(m));
}

template <typename T>
Matrix<T> Inverse(const Matrix<T>& m) {
    return Inverse(MatrixView(m));
}

template <typename T>
std::ostream& operator<<(std::ostream& o, const Matrix<T>& view) {
    o << MatrixView{view};
    return o;
}

}  // namespace nickel