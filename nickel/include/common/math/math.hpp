#pragma once

#include "common/math/algorithm.hpp"
#include "common/math/view.hpp"
#include "common/math/units.hpp"
#include "common/math/algorithm.hpp"
#include "common/math/quaternion.hpp"

namespace nickel {

using MathElemType = float;
using DMat = Matrix<MathElemType>;
using Mat22 = SMatrix<MathElemType, 2, 2>;
using Mat33 = SMatrix<MathElemType, 3, 3>;
using Mat44 = SMatrix<MathElemType, 4, 4>;
using Vec2 = SVector<MathElemType, 2>;
using Vec3 = SVector<MathElemType, 3>;
using Vec4 = SVector<MathElemType, 4>;

using SubMat = MatrixView<MathElemType, false>;
using SubMatView = MatrixView<MathElemType, true>;
using Radians = TRadians<MathElemType>;
using Degrees = TDegrees<MathElemType>;

// TODO: temporary name, change to Rect after replace cgmath
using Rect2 = TRect<MathElemType>;

using Quat = Quaternion<MathElemType>;

constexpr auto PI = GenericPI<MathElemType>;

Radians operator "" _rad(long double);
Degrees operator "" _deg(long double);
Radians operator "" _rad(unsigned long long);
Degrees operator "" _deg(unsigned long long);

}  // namespace nickel