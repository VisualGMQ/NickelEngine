#pragma once

#include "nickel/common/math/algorithm.hpp"
#include <cassert>
#include <concepts>

namespace nickel {

// vectors

template <typename Derive, typename ElemType, size_t Len>
requires std::is_standard_layout_v<ElemType>
class SVectorBase {
public:
    constexpr size_t ElemCount() const noexcept { return Len; }

    static constexpr size_t ColLen = 1;
    static constexpr size_t RowLen = Len;

    auto operator[](size_t idx) const noexcept {
        NICKEL_ASSERT(idx < Len);
        return Ptr()[idx];
    }

    ElemType& operator[](size_t idx) noexcept {
        NICKEL_ASSERT(idx < Len);
        return Ptr()[idx];
    }

    ElemType* Ptr() noexcept { return (ElemType*)this; }

    const ElemType* Ptr() const noexcept { return (ElemType*)this; }

    Derive& operator+=(const SVectorBase& o) noexcept {
        MatrixAdd<ElemType, true>((Derive&)*this, (Derive&)o, (Derive&)*this);
        return (Derive&)*this;
    }

    Derive& operator-=(const SVectorBase& o) noexcept {
        MatrixMinus<ElemType, true>((Derive&)*this, (Derive&)o, (Derive&)*this);
        return (Derive&)*this;
    }

    Derive& operator*=(float value) noexcept {
        MatrixMul<ElemType, true>((Derive&)*this, value, (Derive&)*this);
        return (Derive&)*this;
    }

    Derive& operator*=(const SVectorBase& o) noexcept {
        MatrixMulEach<ElemType, true>((Derive&)*this, (Derive&)o,
                                      (Derive&)*this);
        return (Derive&)*this;
    }

    Derive& operator/=(float value) noexcept {
        MatrixDiv<ElemType, true>((Derive&)*this, value, (Derive&)*this);
        return (Derive&)*this;
    }

    Derive& operator/=(const SVectorBase& o) noexcept {
        MatrixDivEach<ElemType, true>((Derive&)*this, (Derive&)o,
                                      (Derive&)*this);
        return (Derive&)*this;
    }

    Derive operator-() const noexcept {
        const Derive& derive = (const Derive&)*this;
        Derive tmp = derive;
        MatrixNeg<ElemType, true>(tmp, tmp);
        return tmp;
    }

    constexpr size_t ColNum() const noexcept { return 1; }

    constexpr size_t RowNum() const noexcept { return Len; }
};

template <typename T, size_t Len>
class SVector : public SVectorBase<SVector<T, Len>, T, Len> {
public:
    using ElemType = T;
    static constexpr size_t ElemCount = Len;

    SVector() {
        for (size_t i = 0; i < Len; ++i) {
            datas_[i] = 0;
        }
    }

    template <typename U>
    requires(std::convertible_to<U, ElemType>)
    explicit SVector(U elem) {
        for (size_t i = 0; i < Len; ++i) {
            datas_[i] = elem;
        }
    }

    template <typename... Us>
    requires(std::convertible_to<Us, ElemType> && ...)
    explicit SVector(Us... elems) {
        ElemType datas[] = {static_cast<ElemType>(elems)...};
        memcpy(&datas_, &datas, sizeof(datas));
        for (size_t i = sizeof...(Us); i < Len; i++) {
            datas_[i] = 0;
        }
    }

private:
    ElemType datas_[ElemCount];
};

template <typename T>
class SVector<T, 2> : public SVectorBase<SVector<T, 2>, T, 2> {
public:
    using ElemType = T;
    static constexpr size_t ElemCount = 2;

    union {
        ElemType x, s, w;
    };

    union {
        ElemType y, r, h;
    };

    SVector(T x, T y) : x{x}, y{y} {}

    SVector() : x{ElemType{}}, y{ElemType{}} {}

    explicit SVector(T value) : x{value}, y{value} {}
};

template <typename T>
class SVector<T, 3> : public SVectorBase<SVector<T, 3>, T, 3> {
public:
    using ElemType = T;
    static constexpr size_t ElemCount = 3;

    union {
        ElemType x, s, w;
    };

    union {
        ElemType y, r, h;
    };

    union {
        ElemType z, t, l;
    };

    SVector(T x, T y, T z) : x{x}, y{y}, z{z} {}

    SVector() : x{0}, y{0}, z{0} {}

    explicit SVector(T value) : x{value}, y{value}, z{value} {}
};

template <typename T>
class SVector<T, 4> : public SVectorBase<SVector<T, 4>, T, 4> {
public:
    using ElemType = T;
    static constexpr size_t ElemCount = 4;

    union {
        ElemType x, r;
    };

    union {
        ElemType y, g;
    };

    union {
        ElemType z, b;
    };

    union {
        ElemType w, a;
    };

    SVector() : x{ElemType{}}, y{ElemType{}}, z{ElemType{}}, w{ElemType{}} {}

    SVector(T x, T y, T z, T w) : x{x}, y{y}, z{z}, w{w} {}

    explicit SVector(T value) : x{value}, y{value}, z{value}, w{value} {}
};

// operations

template <typename T, size_t Len>
auto operator+(const SVector<T, Len>& v1, const SVector<T, Len>& v2) noexcept {
    SVector<T, Len> result = v1;
    result += v2;
    return result;
}

template <typename T, size_t Len>
auto operator-(const SVector<T, Len>& v1, const SVector<T, Len>& v2) noexcept {
    SVector<T, Len> result = v1;
    result -= v2;
    return result;
}

template <typename T, size_t Len>
auto operator*(const SVector<T, Len>& v1, const SVector<T, Len>& v2) noexcept {
    SVector<T, Len> result = v1;
    result *= v2;
    return result;
}

template <typename T, typename U, size_t Len>
auto operator*(const SVector<T, Len>& v1, U value) noexcept {
    SVector<T, Len> result = v1;
    result *= value;
    return result;
}

template <typename T, typename U, size_t Len>
auto operator*(U value, const SVector<T, Len>& v) noexcept {
    return v * value;
}

template <typename T, size_t Len>
auto operator/(const SVector<T, Len>& v1, const SVector<T, Len>& v2) noexcept {
    SVector<T, Len> result = v1;
    result /= v2;
    return result;
}

template <typename T, typename U, size_t Len>
auto operator/(const SVector<T, Len>& v1, U value) noexcept {
    SVector<T, Len> result = v1;
    result /= value;
    return result;
}

template <typename T, size_t Len>
bool operator==(const SVector<T, Len>& v1, const SVector<T, Len>& v2) noexcept {
    return MatrixEq<T, true>(v1, v2);
}

template <typename T, size_t Len>
bool operator!=(const SVector<T, Len>& v1, const SVector<T, Len>& v2) noexcept {
    return !(v1 == v2);
}

template <typename T, size_t Len>
std::ostream& operator<<(std::ostream& o, const SVector<T, Len>& view) {
    o << "[" << std::endl;
    for (int i = 0; i < Len; i++) {
        o << view[i] << std::endl;
    }
    o << "]";
    return o;
}

// matrix

template <typename T, size_t Col, size_t Row>
requires std::is_standard_layout_v<T>
class SMatrix {
public:
    using ElemType = T;

    static constexpr size_t ColLen = Col;
    static constexpr size_t RowLen = Row;

    static SMatrix Zeros() { return {}; }

    static SMatrix Identity() {
        SMatrix m;
        m.Diagonal(1);
        return m;
    }

    static SMatrix Ones() {
        SMatrix m;
        m.Fill(1);
        return m;
    }

    template <typename U>
    requires std::convertible_to<U, ElemType>
    static SMatrix FillWith(U elem) {
        SMatrix m;
        m.Fill(elem);
        return m;
    }

    template <typename... Ts>
    static SMatrix FromRow(Ts... elems) {
        SMatrix m;
        m.SetValuesFromRow(elems...);
        return m;
    }

    template <typename... Ts>
    static SMatrix FromCol(Ts... elems) {
        SMatrix m;
        m.SetValuesFromCol(elems...);
        return m;
    }

    template <typename... Ts>
    requires(std::convertible_to<Ts, ElemType> && ...)
    static SMatrix Diag(Ts... elems) {
        SMatrix m;
        m.Diagonal(elems...);
        return m;
    }

    SMatrix() = default;

    SMatrix(const SMatrix& o) : data_{o.data_} {}

    SMatrix(SMatrix&& o) noexcept : data_{o.data_} {}

    SMatrix& operator=(SMatrix&& o) noexcept {
        if (&o != this) {
            data_ = std::move(o.data_);
        }
        return *this;
    }

    SMatrix& operator=(const SMatrix& o) noexcept {
        if (&o != this) {
            data_ = o.data_;
        }
        return *this;
    }

    SMatrix& operator+=(const SMatrix& o) noexcept {
        MatrixAdd<T, true>(*this, o, *this);
        return *this;
    }

    SMatrix& operator-=(const SMatrix& o) noexcept {
        MatrixMinus<T, true>(*this, o, *this);
        return *this;
    }

    SMatrix& operator*=(const SMatrix& o) noexcept {
        SMatrix result;
        MatrixMul<T, true>(*this, o, result);
        *this = std::move(result);
        return *this;
    }

    SMatrix& operator*=(float value) noexcept {
        MatrixMul<T, true>(*this, value, *this);
        return *this;
    }

    SMatrix& operator/=(float value) noexcept {
        MatrixDiv<T, true>(*this, value, *this);
        return *this;
    }

    template <typename... Ts>
    requires((std::convertible_to<Ts, ElemType> && ...) && sizeof...(Ts) >= 1)
    void SetValuesFromRow(Ts... elems) {
        ElemType datas[] = {static_cast<ElemType>(elems)...};
        for (int i = 0; i < std::min(sizeof...(elems), ElemCount()); i++) {
            size_t row = i / Col;
            size_t col = i % Col;
            operator[](col)[row] = datas[i];
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
        auto minLen = std::min(Col, Row);
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
            *(Ptr() + i) = elem;
        }
    }

    auto operator[](size_t i) const noexcept { return data_[i]; }

    auto& operator[](size_t i) noexcept { return data_[i]; }

    SMatrix operator-() const noexcept {
        SMatrix m = *this;
        MatrixNeg<ElemType, true>(m, m);
        return m;
    }

    constexpr size_t ElemCount() const noexcept { return Col * Row; }

    ElemType* Ptr() { return data_[0].Ptr(); }

    const ElemType* Ptr() const { return data_[0].Ptr(); }

    constexpr auto ColNum() const noexcept { return Col; }

    constexpr auto RowNum() const noexcept { return Row; }

private:
    std::array<SVector<ElemType, Row>, Col> data_;
};

// matrix operations

template <typename T, size_t Col, size_t Row>
bool operator==(const SMatrix<T, Col, Row>& m1,
                const SMatrix<T, Col, Row>& m2) noexcept {
    return MatrixEq<T, true>(m1, m2);
}

template <typename T, size_t Col, size_t Row>
bool operator!=(const SMatrix<T, Col, Row>& m1,
                const SMatrix<T, Col, Row>& m2) noexcept {
    return !(m1 == m2);
}

template <typename T, size_t Col, size_t Row>
SMatrix<T, Col, Row> operator+(const SMatrix<T, Col, Row>& m1,
                               const SMatrix<T, Col, Row>& m2) {
    SMatrix<T, Col, Row> result = m1;
    result += m2;
    return result;
}

template <typename T, size_t Col, size_t Row>
SMatrix<T, Col, Row> operator-(const SMatrix<T, Col, Row>& m1,
                               const SMatrix<T, Col, Row>& m2) {
    SMatrix<T, Col, Row> result = m1;
    result -= m2;
    return result;
}

template <typename T, typename U, size_t Col, size_t Row>
SMatrix<T, Col, Row> operator*(const SMatrix<T, Col, Row>& m, U value) {
    SMatrix<T, Col, Row> result = m;
    result *= value;
    return result;
}

template <typename T, typename U, size_t Col, size_t Row>
SMatrix<T, Col, Row> operator*(U value, const SMatrix<T, Col, Row>& m) {
    return m * value;
}

template <typename T, typename U, size_t Col, size_t Row>
SMatrix<T, Col, Row> operator/(const SMatrix<T, Col, Row>& m, U value) {
    SMatrix<T, Col, Row> result = m;
    result /= value;
    return result;
}

template <typename T, size_t Len, size_t Row, size_t Col>
SMatrix<T, Col, Row> operator*(const SMatrix<T, Len, Row>& m1,
                               const SMatrix<T, Col, Len>& m2) {
    SMatrix<T, Col, Row> result = m1;
    result *= m2;
    return result;
}

template <typename T, size_t Col, size_t Row>
SMatrix<T, Col, Row> MulEach(const SMatrix<T, Col, Row>& m1,
                             const SMatrix<T, Col, Row>& m2) {
    SMatrix<T, Col, Row> result;
    MatrixMulEach<T, true>(m1, m2, result);
    return result;
}

template <typename T, size_t Col, size_t Row>
SMatrix<T, Col, Row> DivEach(const SMatrix<T, Col, Row>& m1,
                             const SMatrix<T, Col, Row>& m2) {
    SMatrix<T, Col, Row> result;
    MatrixDivEach<T, true>(m1, m2, result);
    return result;
}

template <typename T, size_t Col, size_t Row>
SVector<T, Row> operator*(const SMatrix<T, Col, Row>& m,
                          const SVector<T, Col>& v) {
    SVector<T, Row> result;
    MatrixMul(m, v, result);
    return result;
}

template <typename T, size_t Col, size_t Row>
SMatrix<T, Row, Col> Transpose(const SMatrix<T, Col, Row> m) {
    SMatrix<T, Row, Col> result;
    Transpose<T, true>(m, result);
    return result;
}

template <typename T, size_t Col, size_t Row>
float Det(const SMatrix<T, Col, Row> m) {
    return Det<T, true>(m);
}

}  // namespace nickel
