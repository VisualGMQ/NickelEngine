#pragma once
#include "nickel/common/math/math.hpp"

namespace nickel::graphics {

class DebugDrawer {
public:
    void DrawBox(const Vec3& center, const Vec3& half_size,
                 const Quat& rotation, const Color& color);
};

}