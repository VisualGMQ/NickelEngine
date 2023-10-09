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

// function fwd

template <typename T>
bool IsLineParallel(const Line<T>& l1, const Line<T>& l2, T tol = 0.0001);

template <typename T>
std::optional<std::pair<T, T>> LineIntersect(const Line<T>&, const Line<T>&,
                                             T tol = 0.0001);

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

// intersection check

/**
 * @brief quickly check whether lines intersect(without get intersection)
 * @note this is a quick check, if you want to get intersection same time, use
 * `LineIntersect()`
 */
template <typename T>
bool IsLineIntersect(const Line<T>& l1, const Line<T>& l2,
                     T tol = 0.0001) {
    return !IsLineParallel(l1, l2, tol);
}

/**
 * @brief quick check whether segments intersect(without get intersection)
 */
template <typename T>
bool IsSegIntersect(const Segment<T>& l1, const Segment<T>& l2) {
    auto p1 = l1.p + l1.dir * l1.len;
    auto p2 = l2.p + l2.dir * l2.len;

    return (l1.p - l2.p).Cross(l2.dir) * (p1 - l2.p).Cross(l2.dir) <= 0 &&
           (l2.p - l1.p).Cross(l1.dir) * (p2 - l1.p).Cross(l1.dir) <= 0;
}

/**
 * @brief check whether rays intersect(without get intersection)
 * @note this is a convient method. If you need, use `LineIntersect()` and check
 * the return params yourself
 */
template <typename T>
bool IsRayIntersect(const Ray<T>& l1, const Ray<T>& l2) {
    if (auto result = LineIntersect(l1, l2); result) {
        auto&& [param1, param2] = result.value();
        return param1 >= 0 && param2 >= 0;
    }
    return false;
}

// intersection

/**
 * @brief get intersection between point and line, get param in l1 & l2
 * optionally
 *
 * @tparam T
 * @param l1 line1
 * @param l2 line2
 * @param tol tolerance
 * @return std::optional<std::pair<T, T>> the param on l1 and l2
 */
template <typename T>
std::optional<std::pair<T, T>> LineIntersect(const Line<T>& l1,
                                             const Line<T>& l2,
                                             T tol) {
    auto det = l1.dir.Cross(-l2.dir);
    if (cgmath::IsSameValue<T>(det, 0, tol)) {
        return std::nullopt;
    }

    auto diffP = l2.p - l1.p;
    auto param1 = diffP.Cross(-l2.dir) / det;
    auto param2 = l1.dir.Cross(diffP) / det;

    return std::pair<T, T>(param1, param2);
}

// misc

template <typename T>
bool IsLineParallel(const Line<T>& l1, const Line<T>& l2, T tol) {
    return cgmath::IsSameValue<T>(l1.dir.Cross(l2.dir), 0, tol);
}

}  // namespace geom2d

}  // namespace nickel
