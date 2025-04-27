#pragma once
#include "nickel/common/transform.hpp"

namespace nickel {

struct Bone {
    Transform m_transform;
    Transform m_origin_trans;

    std::vector<Bone> m_children;
};

}