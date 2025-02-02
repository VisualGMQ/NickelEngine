#include "nickel/common/transform.hpp"

namespace nickel {

Transform& Transform::operator*=(const Transform& o) {
    p = o.p + o.q * p;
    q = o.q * q;
    return *this;
}

Transform Transform::RelatedBy(const Transform& parent) const {
    Transform result;
    Quat inv_q = parent.q.Inverse();
    result.q = q * inv_q;
    result.p = inv_q * (p - parent.p);
    return result;
}

Transform operator*(const Transform& t1, const Transform& t2) {
    Transform t(t1);
    t *= t2;
    return t;
}

}
