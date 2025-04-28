#pragma once
#include "nickel/common/transform.hpp"

namespace nickel {

struct Bone {
    Transform m_transform;
    Transform m_origin_trans;
    Transform m_global_transform;
};

}