#pragma once
#include "nickel/common/math/math.hpp"

namespace nickel {

struct Transform {
    Vec3 p;
    Quat q;

    Transform& operator*=(const Transform& parent);
    Transform RelatedBy(const Transform& parent) const;
};

Transform operator*(const Transform& t1, const Transform& t2);

}