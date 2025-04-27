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

// reference to 'Game Engine Architecture 3th'
Transform Transform::FromMat(const Mat44& mat) {
    Vec3 x_axis{mat[0][0], mat[0][1], mat[0][2]};
    Vec3 y_axis{mat[1][0], mat[1][1], mat[1][2]};
    Vec3 z_axis{mat[2][0], mat[2][1], mat[2][2]};
    
    Transform trans;
    trans.p = {mat[3][0], mat[3][1], mat[3][2]};
    trans.scale.x = Length(x_axis);
    trans.scale.y = Length(y_axis);
    trans.scale.z = Length(z_axis);

    if (trans.scale.x == 0 || trans.scale.y == 0 || trans.scale.z == 0) {
        return trans;
    }

    x_axis /= trans.scale.x; 
    y_axis /= trans.scale.y; 
    z_axis /= trans.scale.z; 
    Mat33 rotate_mat = Mat33::FromCol(x_axis.x, x_axis.y, x_axis.z,
                                      y_axis.x, y_axis.y, y_axis.z,
                                      z_axis.x, z_axis.y, z_axis.z);
    float trace = rotate_mat[0][0] + rotate_mat[1][1] + rotate_mat[2][2];

    if (trace > 0.0f) {
        float s = std::sqrt(trace + 1.0f);
        trans.q.w = s * 0.5f;
        float t = 0.5f / s;
        trans.q.v.x = (rotate_mat[1][2] - rotate_mat[2][1]) * t;
        trans.q.v.y = (rotate_mat[2][0] - rotate_mat[0][2]) * t;
        trans.q.v.z = (rotate_mat[0][1] - rotate_mat[1][0]) * t;
    } else {
        int i = 0;
        if (rotate_mat[1][1] > rotate_mat[0][0]) i = 1;
        if (rotate_mat[2][2] > rotate_mat[i][i]) i = 2;
        constexpr int NEXT[3] = {1, 2, 0};
        int j = NEXT[i];
        int k = NEXT[j];

        float s = std::sqrt(rotate_mat[j][i] - rotate_mat[j][j] + rotate_mat[k][k] + 1.0f);
        trans.q.Ptr()[i] = s * 0.5f;
        float t;
        if (s != 0.0)
            t = 0.5f / s;
        else
            t = s;

        trans.q.w = (rotate_mat[j][k] - rotate_mat[k][j]) * t;
        trans.q.Ptr()[j] = (rotate_mat[i][j] + rotate_mat[j][i]) * t;
        trans.q.Ptr()[k] = (rotate_mat[i][k] + rotate_mat[k][i]) * t;
    }
    
    return trans;
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
