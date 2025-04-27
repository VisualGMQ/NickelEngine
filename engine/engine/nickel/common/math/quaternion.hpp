#pragma once

#include "nickel/common/math/units.hpp"
#include "nickel/common/math/smatrix.hpp"

namespace nickel {
template <typename T>
requires std::is_floating_point_v<T>
struct Quaternion final {
    SVector<T, 3> v;
    T w = 1;

    static Quaternion Create(const SVector<T, 3>& axis, TRadians<T> radians) {
        auto half = radians * 0.5;
        return {axis * std::sin(half.Value()), std::cos(half.Value())};
    }
    
    Quaternion() = default;

    T* Ptr() { return v.Ptr(); }

    Quaternion(const SVector<T, 3>& v, T w)
        : v{v}, w{w} {
    }

    Quaternion(T x, T y, T z, T w)
        : v{x, y, z}, w{w} {
    }

    Quaternion(const Quaternion&) = default;

    // only for unit quaternion
    Quaternion Conjugate() const {
        NICKEL_ASSERT(std::abs(LengthSqrd() - 1) <= 0.00001,
                      "conjugate only for unit quaternion");
        return {-v, w};
    }

    // only for unit quaternion
    Quaternion Inverse() const {
        NICKEL_ASSERT(std::abs(LengthSqrd() - 1) <= 0.00001,
                      "inverse only for unit quaternion");
        return Conjugate();
    }

    auto LengthSqrd() const { return ::nickel::LengthSqrd(v) + w * w; }

    auto Length() const { return std::sqrt(::nickel::LengthSqrd(v) + w * w); }

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
        return SMatrix<T, 4, 4>::FromCol(
            1 - 2 * (y2 + z2), 2 * (xy + zw), 2 * (xz - yw), 0,
            2 * (xy - zw), 1 - 2 * (x2 + z2), 2 * (yz + xw), 0,
            2 * (xz + yw), 2 * (yz - xw), 1 - 2 * (x2 + y2), 0,
            0, 0, 0, 1
        );
    }
    // clang-format on

    Quaternion& operator*=(const Quaternion& o) noexcept {
        *this = *this * o;
        return *this;
    }
};

template <typename T>
Quaternion<T> operator*(const Quaternion<T>& q1, const Quaternion<T>& q2) {
    return {q1.w * q2.v + q2.w * q1.v + Cross(q1.v, q2.v),
            q1.w * q2.w - Dot(q1.v, q2.v)};
}

template <typename T>
SVector<T, 3> operator*(const Quaternion<T>& q, const SVector<T, 3>& v) {
    return (q * Quaternion<T>{v, 0} * q.Inverse()).v;
}
} // namespace nickel
