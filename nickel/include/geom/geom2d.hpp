#pragma once

#include "core/cgmath.hpp"
#include "geom/basic_geom.hpp"

namespace nickel {

namespace geom2d {

template <typename T>
using AABB = geom::AABB<T, 2>;

template <typename T>
using Circle = geom::Circular<T, 2>;

template <typename T>
using Plane = geom::Plane<T, 2>;

template <typename T>
using Line = geom::Line<T, 2>;

template <typename T>
using Segment = geom::Segment<T, 2>;

template <typename T>
using Ray = geom::Ray<T, 2>;

template <typename T>
using Capsule = geom::Capsule<T, 2>;

// distance

/**
 * @brief get distance between point and line in 2D
 */
template <typename T>
T LinePtDist(const Line<T>& l, const cgmath::Vec<T, 2>& pt) {
    return (pt - l.p).Cross(l.dir);
}

template <typename T>
T LineDist(const Line<T>& l1, const Line<T>& l2) {
    return LinePtDist(l1, l2.p);
}

// intersection

/**
 * @brief get intersection between point and line, get param in l1 & l2
 * optionally
 *
 * @tparam T
 * @param l1 line1
 * @param l2 line2
 * @param[out] t1 param on line1
 * @param[out] t2 param on line2
 * @param tol tolerance
 * @return std::optional<cgmath::Vec<T, 2>>
 */
template <typename T>
std::optional<cgmath::Vec<T, 2>> LineIntersect(const Line<T>& l1,
                                               const Line<T>& l2,
                                               T* t1 = nullptr, T* t2 = nullptr,
                                               T tol = 0.0001) {
    auto det = l1.dir.Cross(-l2.dir);
    if (cgmath::IsSameValue(det, 0, tol)) {
        return std::nullopt;
    }

    auto diffP = l2.p - l1.p;
    auto param1 = diffP.Cross(-l2.dir) / det;

    if (t1) {
        *t1 = param1;
    }

    if (t2) {
        auto param2 = l1.dir.Cross(diffP) / det;
        *t2 = param2;
    }

    return l1.p + l1.dir * param1;
}

}  // namespace geom2d

}  // namespace nickel
