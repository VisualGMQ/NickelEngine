#pragma once

#include "common/math/matrix.hpp"
#include "common/math/smatrix.hpp"

namespace nickel {

template <typename T, bool IsConst = true>
class MatrixView {
public:
    using ElemType = T;

    template <bool OtherIsConst>
    MatrixView(const MatrixView<T, OtherIsConst>& o)
    requires(IsConst)
        : datas_{o.datas_},
          colBeg_{o.colBeg_},
          colLen_{o.colLen_},
          rowBeg_{o.rowBeg_},
          rowLen_{o.rowLen_},
          col_{o.col_},
          row_{o.row_} {}

    MatrixView(const MatrixView<T, false>& o)
    requires(!IsConst)
        : datas_{o.datas_},
          colBeg_{o.colBeg_},
          colLen_{o.colLen_},
          rowBeg_{o.rowBeg_},
          rowLen_{o.rowLen_},
          col_{o.col_},
          row_{o.row_} {}

    MatrixView(const Matrix<T>& m)
    requires(IsConst)
        : datas_{m.Ptr()},
          colBeg_{0},
          colLen_{m.ColNum()},
          rowBeg_{0},
          rowLen_{m.RowNum()},
          col_{m.ColNum()},
          row_{m.RowNum()} {}

    MatrixView(Matrix<T>& m)
    requires(!IsConst)
        : datas_{m.Ptr()},
          colBeg_{0},
          colLen_{m.ColNum()},
          rowBeg_{0},
          rowLen_{m.RowNum()},
          col_{m.ColNum()},
          row_{m.RowNum()} {}

    template <size_t Col, size_t Row>
    MatrixView(const SMatrix<ElemType, Col, Row>& m)
    requires(IsConst)
        : datas_{m.Ptr()},
          colBeg_{0},
          colLen_{Col},
          rowBeg_{0},
          rowLen_{Row},
          col_{Col},
          row_{Row} {}

    template <size_t Col, size_t Row>
    MatrixView(SMatrix<ElemType, Col, Row>& m)
    requires(!IsConst)
        : datas_{m.Ptr()},
          colBeg_{0},
          colLen_{Col},
          rowBeg_{0},
          rowLen_{Row},
          col_{Col},
          row_{Row} {}

    MatrixView(const Matrix<T>& m, size_t colBeg, size_t colLen, size_t rowBeg,
               size_t rowLen)
    requires(IsConst)
        : datas_{m.Ptr()},
          colBeg_{colBeg},
          colLen_{colLen},
          rowBeg_{rowBeg},
          rowLen_{rowLen},
          col_{m.ColNum()},
          row_{m.RowNum()} {
        assert(colBeg + colLen <= m.ColNum() && rowBeg + rowLen <= m.RowNum());
    }

    MatrixView(Matrix<T>& m, size_t colBeg, size_t colLen, size_t rowBeg,
               size_t rowLen)
    requires(!IsConst)
        : datas_{m.Ptr()},
          colBeg_{colBeg},
          colLen_{colLen},
          rowBeg_{rowBeg},
          rowLen_{rowLen},
          col_{m.ColNum()},
          row_{m.RowNum()} {
        assert(colBeg + colLen <= m.ColNum() && rowBeg + rowLen <= m.RowNum());
    }

    template <size_t Col, size_t Row>
    MatrixView(const SMatrix<T, Col, Row>& m, size_t colBeg, size_t colLen,
               size_t rowBeg, size_t rowLen)
    requires(IsConst)
        : datas_{m.Ptr()},
          colBeg_{colBeg},
          colLen_{colLen},
          rowBeg_{rowBeg},
          rowLen_{rowLen},
          col_{Col},
          row_{Row} {
        assert(colBeg + colLen <= m.ColNum() && rowBeg + rowLen <= m.RowNum());
    }

    template <size_t Col, size_t Row>
    MatrixView(SMatrix<T, Col, Row>& m, size_t colBeg, size_t colLen,
               size_t rowBeg, size_t rowLen)
    requires(!IsConst)
        : datas_{m.Ptr()},
          colBeg_{colBeg},
          colLen_{colLen},
          rowBeg_{rowBeg},
          rowLen_{rowLen},
          col_{Col},
          row_{Row} {
        assert(colBeg + colLen <= m.ColNum() && rowBeg + rowLen <= m.RowNum());
    }

    template <size_t Len>
    MatrixView(SVector<T, Len>& v, size_t beg, size_t len)
        : datas_{v.Ptr()},
          colBeg_{beg},
          colLen_{len},
          rowBeg_{0},
          rowLen_{1} {
        assert(colBeg_ + colLen_ <= Len);
    }

    MatrixView(const MatrixView&) = default;
    MatrixView(MatrixView&&) = default;

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
        return Column<T, IsConst>(*(datas_ + row_ * (idx + colBeg_) + rowBeg_),
                                  rowLen_);
    }

    auto operator[](size_t idx) noexcept
    requires(!IsConst)
    {
        assert(idx < colLen_);
        return Column<T, IsConst>(*(datas_ + row_ * (idx + colBeg_) + rowBeg_),
                                  rowLen_);
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
    size_t col_, row_;
};

}  // namespace nickel