#pragma once

#include "nickel/common/math/smatrix.hpp"

namespace nickel {

template <typename T>
struct Quaternion final {
    SVector<T, 3> v;
    T w;

    Quaternion(const SVector<T, 3>& v, T w) : v{v}, w{w} {}

    Quaternion(T x, T y, T z, T w) : v{x, y, z}, w{w} {}

    // only for unit quaternion
    Quaternion Conjugate() const {
        Assert(std::abs(LengthSqrd() - 1) <= 0.00001,
               "conjugate only for unit quaternion");
        return {-v, w};
    }

    // only for unit quaternion
    Quaternion Inverse() const {
        Assert(std::abs(LengthSqrd() - 1) <= 0.00001,
               "inverse only for unit quaternion");
        return Conjugate();
    }

    auto LengthSqrd() const { return v.LengthSqrd() + w * w; }

    auto Length() const { return std::sqrt(v.LengthSqrd() + w * w); }

    // clang-format off
    SMatrix<T, 4, 4> ToMat() const {
        auto x2 = v.x * v.x;
        auto y2 = v.y * v.y;
        auto z2 = v.z * v.z;
        auto xy = v.x * v.y;
        auto yz = v.y * v.z;
        auto xz = v.x * v.z;
        auto xw = v.x * w;
        auto yw = v.y * w;
        auto zw = v.z * w;
        return SMatrix<T, 4, 4>::FromCol({
            1 - 2 * (y2 + z2),     2 * (xy + zw),     2 * (xz - yw), 0,
               2 * (xy  - zw), 1 - 2 * (x2 + z2),     2 * (yz + xw), 0,
                2 * (xz + yw),     2 * (yz - xw), 1 - 2 * (x2 + y2), 0,
                            0,                 0,                 0, 1
        });
    }

    // clang-format on
};

template <typename T>
Quaternion<T> operator*(const Quaternion<T>& q1, const Quaternion<T>& q2) {
    return {q1.w * q2.v + q2.w * q1.v + q1.v.Cross(q2.v),
            q1.w * q2.w - q1.v.Dot(q2.v)};
}

template <typename T>
Quaternion<T> CreateQuatByRotate(const SVector<T, 3>& axis, T radians) {
    auto half = radians * 0.5;
    return {axis * std::sin(half), std::cos(half)};
}

template <typename T>
SVector<T, 3> RotateByAxis(const SVector<T, 3>& v, const Quaternion<T>& q) {
    return q *
           Quaternion<T>{
               {v.x, v.y, v.z},
               0
    } *
           q.Inverse().v;
}

}  // namespace nickel
