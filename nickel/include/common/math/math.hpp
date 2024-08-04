#pragma once

#include "common/math/algorithm.hpp"
#include "common/math/view.hpp"

namespace nickel {

using MatrixElemType = float;
using DMat = Matrix<MatrixElemType>;
using Mat22 = SMatrix<MatrixElemType, 2, 2>;
using Mat33 = SMatrix<MatrixElemType, 3, 3>;
using Mat44 = SMatrix<MatrixElemType, 4, 4>;
using Vec2 = SVector<MatrixElemType, 2>;
using Vec3 = SVector<MatrixElemType, 3>;
using Vec4 = SVector<MatrixElemType, 4>;

using SubMat = MatrixView<MatrixElemType, false>;
using SubMatView = MatrixView<MatrixElemType, true>;

}  // namespace nickel