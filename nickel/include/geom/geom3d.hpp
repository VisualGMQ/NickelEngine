#include "core/cgmath.hpp"
#include "geom/basic_geom.hpp"

namespace nickel {

namespace geom3d {

template <typename T>
using AABB = geom::AABB<T, 3>;

template <typename T>
using Cube = geom::AABB<T, 3>;

template <typename T>
using Sphere = geom::Circular<T, 3>;

template <typename T>
using Plane = geom::Plane<T, 3>;

template <typename T>
using Line = geom::Line<T, 3>;

template <typename T>
using Segment = geom::Segment<T, 3>;

template <typename T>
using Ray = geom::Ray<T, 3>;

template <typename T>
using Capsule = geom::Capsule<T, 3>;

// distance

/**
 * @brief get distance between point and line in 3D
 */
template <typename T>
T LinePtDistSqrd(const Line<T>& l, const cgmath::Vec<T, 3>& pt) {
    return (pt - l.p).Cross(l.dir).LengthSqrd();
}

// intersection check

/**
 * @brief quickly check whether lines intersect(without get intersection)
 * @note this is a quick check, if you want to get intersection same time, use
 * `LineIntersect()`
 */
template <typename T>
bool IsLineIntersect(const Line<T>& l1, const Line<T>& l2, T tol = 0.0001) {
    return cgmath::IsSameValue(
        cgmath::MixedProduct(l1.p - l2.p, l1.dir, l2.dir), 0.0, tol);
}

/**
 * @brief quick check whether common planar segments intersected
 * @note this is a quick check, if you want to get intersection same time, use
 * `SegIntersect()`
 */
template <typename T>
bool IsSamePlaneSegIntersect(const Segment<T>& l1, const Segment<T>& l2) {
    auto p1 = l1.p + l1.dir * l1.len;
    auto p2 = l2.p + l2.dir * l2.len;

    return (l1.p - l2.p).Cross(l2.dir).Dot((p1 - l2.p).Cross(l2.dir)) >= 0 &&
           (l2.p - l1.p).Cross(l1.dir).Dot((p2 - l1.p).Cross(l1.dir)) >= 0;
}

/**
 * @brief quick check whether segments intersected(without get intersection)
 * @note this is a quick check, if you want to get intersection same time, use
 * `SegIntersect()`
 * @note * if you ensure segment are in same plane, use
 * `IsSamePlaneSegIntersect()` is more quickly
 */
template <typename T>
bool IsSegIntersect(const Segment<T>& l1, const Segment<T>& l2) {
    return IsLineIntersect(l1, l2) ? IsSamePlaneSegIntersect(l1, l2) : false;
}

// misc

/**
 * @brief get minimal segment between two lines
 * @note the minimal line must be ortho to input lines, and has minimal distance
 *
 * @return std::pair<T, T> the params on l1 and l2
 * @param tol tolerance about line parallel
 */
template <typename T>
std::pair<T, T> MinSegBetweenLines(const Line<T>& l1, const Line<T>& l2,
                                   T tol = 0.0001) {
    auto v = l1.p - l2.p;
    T d = l1.dir.Dot(l2.dir);
    T e = -v.Dot(l1.dir);
    T f = -v.Dot(l2.dir);
    cgmath::Vec<T, 2> a{1, d}, b{-d, -1}, c{e, f};

    auto det = a.Cross(b);
    T t = 0, u = 0;

    if (cgmath::IsSameValue<T>(det, 0, tol)) {
        u = -e / d;
    } else {
        double invDet = 1.0 / det;
        t = c.Cross(b) * invDet;
        u = a.Cross(c) * invDet;
    }

    return {t, u};
}

}  // namespace geom3d

}  // namespace nickel