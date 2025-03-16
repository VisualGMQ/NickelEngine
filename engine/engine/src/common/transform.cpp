#include "nickel/common/transform.hpp"

namespace nickel {

Transform& Transform::operator*=(const Transform& o) {
    p = p + q * (scale * o.p);
    q = q * o.q;
    scale = scale * o.scale;
    return *this;
}

Transform Transform::RelatedBy(const Transform& parent) const {
    NICKEL_ASSERT(parent.scale != Vec3(0, 0, 0));

    Transform result;
    Quat inv_q = parent.q.Inverse();
    result.q = q * inv_q;
    result.p = inv_q * ((p - parent.p) / parent.scale);
    result.scale = scale / parent.scale;
    return result;
}

Mat44 Transform::ToMat() const {
    return CreateTranslation(p) * CreateScale(scale) * q.ToMat();
}

Transform operator*(const Transform& t1, const Transform& t2) {
    Transform t(t1);
    t *= t2;
    return t;
}

Vec3 operator*(const Transform& t, const Vec3& p) {
    return t.p + t.q * (t.scale * p);
}

}  // namespace nickel
