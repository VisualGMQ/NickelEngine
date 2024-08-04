#pragma once
#include <cassert>
#include <cstring>
#include <type_traits>
#include <utility>

namespace nickel {

template <typename T, bool IsConst = true>
class Column {
public:
    using ElemType = T;

    Column(ElemType& datas, size_t len)
    requires(!IsConst)
        : datas_{&datas}, len_{len} {}

    Column(const ElemType& datas, size_t len)
    requires(IsConst)
        : datas_{&datas}, len_{len} {}

    auto operator[](size_t idx) const noexcept {
        assert(idx < len_);
        return datas_[idx];
    }

    auto& operator[](size_t idx) noexcept
    requires(!IsConst)
    {
        assert(idx < len_);
        return datas_[idx];
    }

private:
    std::conditional_t<IsConst, const ElemType*, ElemType*> datas_{};
    size_t len_{};
};

template <typename T>
class Matrix {
public:
    using ElemType = T;
    static constexpr size_t SMOElemCount = 4 * 4;

    static Matrix Zeros(size_t col, size_t row) {
        return {};
    }
    
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
    static Matrix Fill(size_t col, size_t row, T elem) {
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

    Matrix(size_t col, size_t row): row_{row}, col_{col} {
        auto elemCount = ElemCount();
        if (elemCount <= SMOElemCount) {
            memset(data_.smo, ElemType{}, sizeof(data_.smo));
        } else {
            data_.elems = new ElemType[elemCount];
            memset(data_.elems, ElemType{}, sizeof(ElemType) * elemCount);
        }
    }

    Matrix(const Matrix& o) : row_{o.row_}, col_{o.col_} {
        auto elemCount = ElemCount();
        if (elemCount <= SMOElemCount) {
            memcpy(data_.smo, o.data_.smo, sizeof(ElemType) * elemCount);
        } else {
            data_.elems = new ElemType[elemCount];
            memcpy(data_.elems, o.data_.elems, sizeof(ElemType) * elemCount);
        }
    }

    Matrix(Matrix&& o) : Matrix() { swap(o, *this); }

    Matrix& operator=(Matrix&& o) {
        swap(o, *this);
        return *this;
    }

    Matrix& operator=(Matrix o) {
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
    void Fill(T elem) {
        auto elemCount = ElemCount();
        memset(Ptr(), elem, sizeof(ElemType) * elemCount);
    }

    Column<ElemType, true> operator[](size_t i) const noexcept {
        return Column<ElemType, true>{*(Ptr() + i * row_), row_};
    }

    Column<ElemType, false> operator[](size_t i) noexcept {
        return Column<ElemType, false>{*(Ptr() + i * row_), row_};
    }

    bool operator==(const Matrix& o) const noexcept {
        if (o.row_ != row_ || o.col_ != col_) {
            return false;
        }

        for (int r = 0; r < row_; r++) {
            for (int c = 0; c < col_; c++) {
                if (operator[](c)[r] != operator[](c)[r]) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const Matrix& o) const noexcept { return !(*this == o); }

    size_t ElemCount() const noexcept { return row_ * col_; }

    ElemType* Ptr() {
        return ElemCount() <= SMOElemCount ? data_.smo : data_.elems;
    }

    const ElemType* Ptr() const {
        return ElemCount() <= SMOElemCount ? data_.smo : data_.elems;
    }

    auto ColNum() const noexcept { return col_; }
    auto RowNum() const noexcept { return row_; }

    ~Matrix() {
        if (ElemCount() > SMOElemCount) {
            delete[] data_.elems;
        }
    }

private:
    union {
        ElemType* elems;
        ElemType smo[SMOElemCount];  // small matrix optimization
    } data_;

    size_t row_;
    size_t col_;
    
    Matrix() : row_{0}, col_{0}, data_{0} {}

    friend void swap(Matrix& m1, Matrix& m2) noexcept {
        using std::swap;
        swap(m1.col_, m2.col_);
        swap(m1.row_, m2.row_);
        swap(m1.data_, m2.data_);
    }
};

}