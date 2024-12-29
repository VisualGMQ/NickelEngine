#pragma once
#include "nickel/common/math/column.hpp"

namespace nickel {

// fwd declare

template <typename T>
requires std::is_pod_v<T>
class Matrix;

template <typename T, size_t, size_t>
requires std::is_pod_v<T>
class SMatrix;

template <typename T, size_t>
class SVector;

template <typename T, bool IsConst = true>
class MatrixView {
public:
    using ElemType = T;
    static constexpr bool is_const = IsConst;

    template <bool OtherIsConst>
    MatrixView(const MatrixView<T, OtherIsConst>& o)
    requires(IsConst)
        : datas_{o.datas_},
          colBeg_{o.colBeg_},
          colLen_{o.colLen_},
          rowBeg_{o.rowBeg_},
          rowLen_{o.rowLen_} {}

    MatrixView(const MatrixView<T, false>& o)
    requires(!IsConst)
        : datas_{o.datas_},
          colBeg_{o.colBeg_},
          colLen_{o.colLen_},
          rowBeg_{o.rowBeg_},
          rowLen_{o.rowLen_} {}

    MatrixView(const Matrix<T>& m)
    requires(IsConst)
        : datas_{m.Ptr()},
          colBeg_{0},
          colLen_{m.ColNum()},
          rowBeg_{0},
          rowLen_{m.RowNum()} {}

    MatrixView(Matrix<T>& m)
    requires(!IsConst)
        : datas_{m.Ptr()},
          colBeg_{0},
          colLen_{m.ColNum()},
          rowBeg_{0},
          rowLen_{m.RowNum()} {}

    MatrixView(const Matrix<T>& m, size_t colBeg, size_t colLen, size_t rowBeg,
               size_t rowLen)
    requires(IsConst)
        : datas_{m.Ptr()},
          colBeg_{colBeg},
          colLen_{colLen},
          rowBeg_{rowBeg},
          rowLen_{rowLen} {
        NICKEL_ASSERT(colBeg + colLen <= m.ColNum() &&
                      rowBeg + rowLen <= m.RowNum());
    }

    MatrixView(Matrix<T>& m, size_t colBeg, size_t colLen, size_t rowBeg,
               size_t rowLen)
    requires(!IsConst)
        : datas_{m.Ptr()},
          colBeg_{colBeg},
          colLen_{colLen},
          rowBeg_{rowBeg},
          rowLen_{rowLen} {
        NICKEL_ASSERT(colBeg + colLen <= m.ColNum() &&
                      rowBeg + rowLen <= m.RowNum());
    }

    template <size_t Col, size_t Row>
    MatrixView(SMatrix<T, Col, Row>& m, size_t colBeg, size_t colLen,
               size_t rowBeg, size_t rowLen)
    requires(!IsConst)
        : datas_{m.Ptr()},
          colBeg_{colBeg},
          colLen_{colLen},
          rowBeg_{rowBeg},
          rowLen_{rowLen} {
        NICKEL_ASSERT(colBeg + colLen <= m.ColNum() &&
                      rowBeg + rowLen <= m.RowNum());
    }

    template <size_t Col, size_t Row>
    MatrixView(const SMatrix<T, Col, Row>& m, size_t colBeg, size_t colLen,
               size_t rowBeg, size_t rowLen)
    requires(IsConst)
        : datas_{m.Ptr()},
          colBeg_{colBeg},
          colLen_{colLen},
          rowBeg_{rowBeg},
          rowLen_{rowLen} {
        NICKEL_ASSERT(colBeg + colLen <= m.ColNum() &&
                      rowBeg + rowLen <= m.RowNum());
    }

    template <size_t Col, size_t Row>
    MatrixView(const SMatrix<T, Col, Row>& m)
    requires(IsConst)
        : datas_{m.Ptr()}, colBeg_{0}, colLen_{Col}, rowBeg_{0}, rowLen_{Row} {
        NICKEL_ASSERT(colBeg_ + colLen_ <= m.ColNum() &&
                      rowBeg_ + rowLen_ <= m.RowNum());
    }

    template <size_t Col, size_t Row>
    MatrixView(SMatrix<T, Col, Row>& m)
    requires(!IsConst)
        : datas_{m.Ptr()}, colBeg_{0}, colLen_{Col}, rowBeg_{0}, rowLen_{Row} {
        NICKEL_ASSERT(colBeg_ + colLen_ <= m.ColNum() &&
                      rowBeg_ + rowLen_ <= m.RowNum());
    }

    template <size_t Len>
    MatrixView(const SVector<T, Len>& m)
    requires(IsConst)
        : datas_{m.Ptr()}, colBeg_{0}, colLen_{1}, rowBeg_{0}, rowLen_{Len} {
        NICKEL_ASSERT(colBeg_ + colLen_ <= m.ColNum() &&
                      rowBeg_ + rowLen_ <= m.RowNum());
    }

    template <size_t Len>
    MatrixView(SVector<T, Len>& m)
    requires(!IsConst)
        : datas_{m.Ptr()}, colBeg_{0}, colLen_{1}, rowBeg_{0}, rowLen_{Len} {
        NICKEL_ASSERT(colBeg_ + colLen_ <= m.ColNum() &&
                      rowBeg_ + rowLen_ <= m.RowNum());
    }

    template <size_t Len>
    MatrixView(const SVector<T, Len>& m, size_t colBeg, size_t colLen,
               size_t rowBeg, size_t rowLen)
    requires(IsConst)
        : datas_{m.Ptr()},
          colBeg_{colBeg},
          colLen_{colLen},
          rowBeg_{rowBeg},
          rowLen_{rowLen} {
        NICKEL_ASSERT(colBeg + colLen <= m.ColNum() &&
                      rowBeg + rowLen <= m.RowNum());
    }

    template <size_t Len>
    MatrixView(SVector<T, Len>& m, size_t colBeg, size_t colLen, size_t rowBeg,
               size_t rowLen)
    requires(!IsConst)
        : datas_{m.Ptr()},
          colBeg_{colBeg},
          colLen_{colLen},
          rowBeg_{rowBeg},
          rowLen_{rowLen} {
        NICKEL_ASSERT(colBeg + colLen <= m.ColNum() &&
                      rowBeg + rowLen <= m.RowNum());
    }

    size_t ElemCount() const noexcept { return colLen_ * rowLen_; }

    size_t ColBegin() const noexcept { return colBeg_; }

    size_t RowBegin() const noexcept { return rowBeg_; }

    size_t ColNum() const noexcept { return colLen_; }

    size_t RowNum() const noexcept { return rowLen_; }

    size_t ColEnd() const noexcept { return colBeg_ + colLen_; }

    size_t RowEnd() const noexcept { return rowBeg_ + rowLen_; }

    bool IsSquare() const noexcept { return colLen_ == rowLen_; }

    auto operator[](size_t idx) const noexcept
    requires(IsConst)
    {
        assert(idx < colLen_);
        return Column<T, IsConst>(
            *(datas_ + rowLen_ * (idx + colBeg_) + rowBeg_), rowLen_);
    }

    auto operator[](size_t idx) noexcept
    requires(!IsConst)
    {
        assert(idx < colLen_);
        return Column<T, IsConst>(
            *(datas_ + rowLen_ * (idx + colBeg_) + rowBeg_), rowLen_);
    }

    Matrix<T> Clone() const {
        auto colNum = ColNum(), rowNum = RowNum();
        Matrix<T> m{colNum, rowNum};
        for (int r = 0; r < rowNum; r++) {
            for (int c = 0; c < colNum; c++) {
                m[c][r] = (*this)[colBeg_ + c][rowBeg_ + r];
            }
        }
        return m;
    }

private:
    std::conditional_t<IsConst, const ElemType*, ElemType*> datas_;
    size_t colBeg_, colLen_, rowBeg_, rowLen_;
};

}  // namespace nickel
