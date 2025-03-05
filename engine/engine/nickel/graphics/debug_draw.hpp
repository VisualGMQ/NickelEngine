#pragma once
#include "nickel/common/math/math.hpp"

namespace nickel::graphics {

class DebugDrawer {
public:
    void DrawLine(const Vec3& p1, const Vec3& p2, const Color& color1,
              const Color& color2);
    void DrawBox(const Vec3& center, const Vec3& half_size,
                 const Quat& rotation, const Color& color);
    void DrawTriangleMesh(std::span<Vec3> vertices, std::span<uint32_t> indices,
                          const Color& color);
    void DrawTriangleMesh(std::span<Vec3> vertices, std::span<uint16_t> indices,
                          const Color& color);
};

}  // namespace nickel::graphics