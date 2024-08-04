#pragma once

#include <array>
#include <cmath>
#include <type_traits>

namespace nickel {

template <typename T, size_t Len, bool IsConst = true>
class SColumn {
public:
    using ElemType = T;

    SColumn(ElemType& datas)
    requires(!IsConst)
        : datas_{&datas} {}

    SColumn(const ElemType& datas, size_t len)
    requires(IsConst)
        : datas_{&datas} {}

    auto operator[](size_t idx) const noexcept {
        assert(idx < Len);
        return datas_[idx];
    }

    auto& operator[](size_t idx) noexcept
    requires(!IsConst)
    {
        assert(idx < Len);
        return datas_[idx];
    }

private:
    std::conditional_t<IsConst, const ElemType*, ElemType*> datas_{};
};

template <typename T, size_t Col, size_t Row>
class SMatrix {
public:
    using ElemType = T;

    static SMatrix Zeros(size_t col, size_t row) { return {}; }

    static SMatrix Identity(size_t col, size_t row) {
        SMatrix m{col, row};
        m.Diagonal(1);
        return m;
    }

    static SMatrix Ones(size_t col, size_t row) {
        SMatrix m{col, row};
        m.Fill(1);
        return m;
    }

    template <typename U>
    requires std::convertible_to<U, ElemType>
    static SMatrix Fill(size_t col, size_t row, U elem) {
        SMatrix m(col, row);
        m.Fill(elem);
        return m;
    }

    template <typename... Ts>
    static SMatrix FromRow(size_t col, size_t row, Ts... elems) {
        SMatrix m(col, row);
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

    SMatrix(const SMatrix& o) : data_{o.data_} {}

    SMatrix(SMatrix&& o) : SMatrix() { swap(o, *this); }

    SMatrix& operator=(SMatrix&& o) {
        swap(o, *this);
        return *this;
    }

    SMatrix& operator=(SMatrix o) {
        swap(o, *this);
        return *this;
    }

    template <typename... Ts>
    requires((std::convertible_to<Ts, ElemType> && ...) && sizeof...(Ts) >= 1)
    void SetValuesFromRow(Ts... elems) {
        ElemType datas[] = {static_cast<ElemType>(elems)...};
        for (int i = 0; i < std::min(sizeof...(elems), ElemCount()); i++) {
            size_t row = i / Col;
            size_t col = i % Row;
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
        memset(Ptr(), elem, sizeof(ElemType) * elemCount);
    }

    auto operator[](size_t i) const noexcept {
        return SColumn<ElemType, Row, true>{*(Ptr() + i * Row)};
    }

    auto operator[](size_t i) noexcept {
        return SColumn<ElemType, Row, false>{*(Ptr() + i * Row)};
    }

    bool operator==(const SMatrix& o) const noexcept {
        for (int r = 0; r < Row; r++) {
            for (int c = 0; c < Col; c++) {
                if ((*this)[c][r] != (*this)[c][r]) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const SMatrix& o) const noexcept { return !(*this == o); }

    constexpr size_t ElemCount() const noexcept { return Col * Row; }

    ElemType* Ptr() { return data_.data(); }

    const ElemType* Ptr() const { return data_; }

    constexpr auto ColNum() const noexcept { return Col; }

    constexpr auto RowNum() const noexcept { return Row; }

private:
    std::array<ElemType, Row * Col> data_;

    SMatrix() { data_.fill(ElemType{}); }

    friend void swap(SMatrix& m1, SMatrix& m2) noexcept {
        using std::swap;
        swap(m1.data_, m2.data_);
    }
};

namespace internal {

template <typename Vector>
concept is_svector = requires {
    typename Vector::ElemType;
    requires Vector::ElemCount != 0;
    Vector::datas;
};

}

template <typename Derive>
requires internal::is_svector<Derive>

class SVectorBase {
private:
    using elem_type = typename Derive::ElemType;
    static constexpr auto elem_count = Derive::ElemCount;

public:
    template <typename U>
    requires(std::convertible_to<U, elem_type>)
    SVectorBase(U elem) {
        Derive* derive = static_cast<Derive*>(this);
        elem_type data = static_cast<elem_type>(elem);
        memcpy(derive->datas, &data, sizeof(elem_type) * elem_count);
    }

    template <typename... Us>
    requires(std::convertible_to<Us, elem_type> && ...)
    SVectorBase(Us... elems) {
        elem_type datas[] = {static_cast<elem_type>(elems)...};
        Derive* derive = static_cast<Derive*>(this);
        memset(derive->datas, 0, sizeof(elem_type) * elem_count);
        memcpy(derive->datas, &datas, sizeof(datas));
    }

    constexpr size_t ElemCount() const noexcept { return ElemCount; }

    elem_type operator[](size_t idx) const noexcept {
        assert(idx < elem_count);
        Derive* derive = static_cast<Derive*>(this);
        return derive->datas[idx];
    }

    elem_type& operator[](size_t idx) noexcept {
        assert(idx < elem_count);
        Derive* derive = static_cast<Derive*>(this);
        return derive->datas[idx];
    }
};

template <typename T, size_t Len>
class SVector: public SVectorBase<SVector<T, Len>> {
public:
    using ElemType = T;
    static constexpr size_t ElemCount = Len;

private:
    ElemType datas[ElemCount];
};

template <typename T>
class SVector<T, 2> : public SVectorBase<SVector<T, 2>> {
public:
    using ElemType = T;
    static constexpr size_t ElemCount = 2;

    union {
        struct {
            ElemType x, y;
        };

        struct {
            ElemType s, r;
        };

        struct {
            ElemType w, h;
        };

        ElemType datas[ElemCount];
    };
};

template <typename T>
class SVector<T, 3> : public SVectorBase<SVector<T, 3>> {
public:
    using ElemType = T;
    static constexpr size_t ElemCount = 3;

    union {
        struct {
            ElemType x, y, z;
        };

        struct {
            ElemType s, r, t;
        };

        struct {
            ElemType w, h, l;
        };

        ElemType datas[ElemCount];
    };
};

template <typename T>
class SVector<T, 4> : public SVectorBase<SVector<T, 4>> {
public:
    using ElemType = T;
    static constexpr size_t ElemCount = 4;

    union {
        struct {
            ElemType x, y, z, w;
        };

        ElemType datas[ElemCount];
    };
};

}  // namespace nickel