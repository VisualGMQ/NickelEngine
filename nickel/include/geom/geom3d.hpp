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

}  // namespace geom3d

}  // namespace nickel