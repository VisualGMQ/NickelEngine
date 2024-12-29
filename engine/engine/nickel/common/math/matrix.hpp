#pragma once
#include "nickel/common/math/algorithm.hpp"
#include "nickel/common/math/column.hpp"

namespace nickel {

template <typename T>
requires std::is_pod_v<T>
class Matrix {
public:
    using ElemType = T;

    static Matrix Zeros(size_t col, size_t row) { return Matrix{col, row}; }

    static Matrix Identity(size_t col, size_t row) {
        Matrix m{col, row};
        m.Diagonal(1);
        return m;
    }

    static Matrix Ones(size_t col, size_t row) {
        Matrix m{col, row};
        m.Fill(1);
        return m;
    }

    template <typename U>
    requires std::convertible_to<U, ElemType>
    static Matrix FillWith(size_t col, size_t row, U elem) {
        Matrix m(col, row);
        m.Fill(elem);
        return m;
    }

    template <typename... Ts>
    static Matrix FromRow(size_t col, size_t row, Ts... elems) {
        Matrix m(col, row);
        m.SetValuesFromRow(elems...);
        return m;
    }

    template <typename... Ts>
    static Matrix FromCol(size_t col, size_t row, Ts... elems) {
        Matrix m(col, row);
        m.SetValuesFromCol(elems...);
        return m;
    }

    template <typename... Ts>
    requires(std::convertible_to<Ts, ElemType> && ...)
    static Matrix Diag(size_t col, size_t row, Ts... elems) {
        Matrix m(col, row);
        m.Diagonal(elems...);
        return m;
    }

    Matrix() : row_{0}, col_{0} {}

    Matrix(size_t col, size_t row) : row_{row}, col_{col} {
        auto elemCount = ElemCount();
        data_ = new ElemType[elemCount];
        for (size_t i = 0; i < row_; ++i) {
            for (size_t j = 0; j < col_; ++j) {
                (*this)[j][i] = 0;
            }
        }
    }

    Matrix(const Matrix& o) : row_{o.row_}, col_{o.col_} {
        auto elemCount = ElemCount();
        data_ = new ElemType[elemCount];
        memcpy(data_, o.data_, sizeof(ElemType) * elemCount);
    }

    Matrix(Matrix&& o) noexcept : Matrix() { swap(o, *this); }

    ~Matrix() { delete[] data_; }

    Matrix& operator=(Matrix o) noexcept {
        swap(o, *this);
        return *this;
    }

    template <typename... Ts>
    requires((std::convertible_to<Ts, ElemType> && ...) && sizeof...(Ts) >= 1)
    void SetValuesFromRow(Ts... elems) {
        ElemType datas[] = {static_cast<ElemType>(elems)...};
        for (int i = 0; i < std::min(sizeof...(elems), ElemCount()); i++) {
            size_t row = i / col_;
            size_t col = i % col_;
            (*this)[col][row] = datas[i];
        }
    }

    template <typename... Ts>
    requires((std::convertible_to<Ts, ElemType> && ...) && sizeof...(Ts) >= 1)
    void SetValuesFromCol(Ts... elems) {
        ElemType datas[] = {static_cast<ElemType>(elems)...};
        auto ptr = Ptr();
        memcpy(ptr, datas, sizeof(datas));
    }

    template <typename FirstType, typename... OtherTypes>
    void Diagonal(FirstType elem, OtherTypes... elems) {
        auto minLen = std::min(row_, col_);
        constexpr size_t elemCount = sizeof...(elems);
        if constexpr (elemCount == 0) {
            for (int i = 0; i < minLen; i++) {
                (*this)[i][i] = elem;
            }
        } else {
            ElemType datas[] = {static_cast<ElemType>(elem),
                                static_cast<ElemType>(elems)...};
            for (int i = 0; i < std::min(minLen, elemCount); i++) {
                (*this)[i][i] = datas[i];
            }
        }
    }

    template <typename U>
    void Fill(U elem) {
        auto elemCount = ElemCount();
        for (size_t i = 0; i < elemCount; i++) {
            data_[i] = elem;
        }
    }

    Column<ElemType, true> operator[](size_t i) const noexcept {
        return Column<ElemType, true>{*(Ptr() + i * row_), row_};
    }

    Column<ElemType, false> operator[](size_t i) noexcept {
        return Column<ElemType, false>{*(Ptr() + i * row_), row_};
    }

    bool operator==(const Matrix& o) const noexcept {
        if (ColNum() != o.ColNum() || RowNum() != o.RowNum()) {
            return false;
        }
        return MatrixEq<T, true>(*this, o);
    }

    bool operator!=(const Matrix& o) const noexcept { return !(*this == o); }

    Matrix operator-() const noexcept {
        Matrix result{ColNum(), RowNum()};
        MatrixNeg<T, true>(*this, result);
        return result;
    }

    Matrix& operator-=(const Matrix& o) noexcept {
        NICKEL_ASSERT(ColNum() == ColNum() && o.RowNum() == o.RowNum());
        MatrixMinus<T, true>(*this, o, *this);
        return *this;
    }

    Matrix& operator+=(const Matrix& o) noexcept {
        NICKEL_ASSERT(ColNum() == ColNum() && o.RowNum() == o.RowNum());
        MatrixAdd<T, true>(*this, o, *this);
        return *this;
    }

    Matrix& operator*=(const Matrix& o) noexcept {
        NICKEL_ASSERT(ColNum() == o.RowNum());
        Matrix result{o.ColNum(), RowNum()};
        MatrixMul<T, true>(*this, o, result);
        *this = std::move(result);
        return *this;
    }

    Matrix& operator*=(float o) noexcept {
        MatrixMul(*this, o, *this);
        return *this;
    }

    Matrix& operator/=(float o) noexcept {
        MatrixDiv(*this, o, *this);
        return *this;
    }

    size_t ElemCount() const noexcept { return row_ * col_; }

    ElemType* Ptr() { return data_; }

    const ElemType* Ptr() const { return data_; }

    auto ColNum() const noexcept { return col_; }

    auto RowNum() const noexcept { return row_; }

    void Resize(size_t col, size_t row) {
        delete data_;
        data_ = new ElemType[col * row];
        row_ = row;
        col_ = col;
    }

private:
    ElemType* data_{};

    size_t row_;
    size_t col_;

    friend void swap(Matrix& m1, Matrix& m2) noexcept {
        using std::swap;
        swap(m1.col_, m2.col_);
        swap(m1.row_, m2.row_);
        swap(m1.data_, m2.data_);
    }
};

// function wrappers

template <typename T>
Matrix<T> operator+(const Matrix<T>& m1, const Matrix<T>& m2) noexcept {
    Matrix<T> m = m1;
    m += m2;
    return m;
}

template <typename T>
Matrix<T> operator-(const Matrix<T>& m1, const Matrix<T>& m2) noexcept {
    Matrix<T> m = m1;
    m -= m2;
    return m;
}

template <typename T>
Matrix<T> operator*(const Matrix<T>& m, double value) noexcept {
    Matrix<T> result = m;
    result *= value;
    return result;
}

template <typename T>
Matrix<T> operator*(float value, const Matrix<T>& m) noexcept {
    return m * value;
}

template <typename T>
Matrix<T> operator/(const Matrix<T>& m, float value) noexcept {
    Matrix<T> result = m;
    result /= value;
    return result;
}

template <typename T>
Matrix<T> operator*(const Matrix<T>& m1, const Matrix<T>& m2) {
    Matrix<T> result = m1;
    result *= m2;
    return result;
}

template <typename T>
Matrix<T> Transpose(const Matrix<T>& m) {
    Matrix<T> matrix{m.RowNum(), m.ColNum()};
    Transpose(m, matrix);
    return m;
}

template <typename T>
double Det(const Matrix<T>& m) {
    return Det(m);
}

template <typename T>
Matrix<T> Cofactor(const Matrix<T>& m, size_t col, size_t row) {
    NICKEL_ASSERT(m.ColNum() > 1 && m.RowNum() > 1);
    Matrix<T> matrix{m.ColNum() - 1, m.RowNum() - 1};
    Cofactor(m, col, row, matrix);
    return matrix;
}

template <typename T>
Matrix<T> Adjoint(const Matrix<T>& m) {
    Matrix<T> matrix{m.ColNum(), m.RowNum()};
    Adjoint(m, matrix);
    return matrix;
}

template <typename T>
Matrix<T> Inverse(const Matrix<T>& m) {
    NICKEL_ASSERT(m.ColNum() == m.RowNum());
    Matrix<T> matrix{m.ColNum(), m.RowNum()};
    Inverse(m, matrix);
    return matrix;
}

template <typename T>
std::ostream& operator<<(std::ostream& o, const Matrix<T>& view) {
    o << "Matrix" << MatrixView{view};
    return o;
}

}  // namespace nickel
