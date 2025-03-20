#pragma once
#include "nickel/common/math/math.hpp"

namespace nickel {

struct Transform {
    Vec3 p;
    Vec3 scale{1, 1, 1};
    Quat q;

    Transform& operator*=(const Transform& parent);
    Transform RelatedBy(const Transform& parent) const;

    Mat44 ToMat() const;
};

Transform operator*(const Transform& t1, const Transform& t2);
Vec3 operator*(const Transform& t, const Vec3& p);

}