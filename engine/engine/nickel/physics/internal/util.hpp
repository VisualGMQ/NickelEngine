#pragma once

#include "nickel/common/math/math.hpp"
#include "nickel/common/transform.hpp"
#include "nickel/physics/internal/pch.hpp"

namespace nickel::physics {

inline physx::PxVec2 Vec2ToPhysX(const Vec2& v) {
    return physx::PxVec2(v.x, v.y);
}

inline physx::PxVec3 Vec3ToPhysX(const Vec3& v) {
    return physx::PxVec3(v.x, v.y, v.z);
}

inline physx::PxVec4 Vec4ToPhysX(const Vec4& v) {
    return physx::PxVec4(v.x, v.y, v.z, v.w);
}

inline physx::PxQuat QuatToPhysX(const Quat& q) {
    return physx::PxQuat(q.v.x, q.v.y, q.v.z, q.w);
}

inline physx::PxTransform TransformToPhysX(const Transform& t) {
    return physx::PxTransform(Vec3ToPhysX(t.p), QuatToPhysX(t.q));
}

inline Vec2 Vec2FromPhysX(const physx::PxVec2& v) {
    return Vec2(v.x, v.y);
}

inline Vec3 Vec3FromPhysX(const physx::PxVec3& v) {
    return Vec3(v.x, v.y, v.z);
}

inline Vec4 Vec4FromPhysX(const physx::PxVec4& v) {
    return Vec4(v.x, v.y, v.z, v.w);
}

inline Quat QuatFromPhysX(const physx::PxQuat& q) {
    return Quat{Vec3(q.x, q.y, q.z), q.w};
}

inline Transform TransformFromPhysX(const physx::PxTransform& t) {
    return Transform{
        Vec3FromPhysX(t.p), {1, 1, 1},
         QuatFromPhysX(t.q)
    };
}

}  // namespace nickel::physics