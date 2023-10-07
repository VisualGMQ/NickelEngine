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

// intersection

/**
 * @brief get intersection between two lines
 */
template <typename T>
std::optional<cgmath::Vec<T, 3>> LineIntersect(const Line<T>& l1,
                                               const Line<T>& l2,
                                               T sinTol = 0.0001) {
    auto dist = std::sqrt(LinePtDistSqrd(l1, l2.p));
    auto sinTheta = (l1.dir.Cross(l2.dir)).Length();

    if (cgmath::IsSameValue(sinTheta, 0, tol)) {
        return std::nullopt;
    } else {
        auto len = dist / sinTheta;
        return l2.p + l2.dir * len;
    }
}

}  // namespace geom3d

}  // namespace nickel