#pragma once
#include "nickel/graphics/gltf.hpp"
#include "nickel/common/math/math.hpp"

namespace nickel::graphics {

class DebugDrawer {
public:
    DebugDrawer();

    void DrawSphere(const Vec3& center, float radius, const Quat& quat,
                    const Color& color,
                    bool wireframe);
    void DrawCylinder(const Vec3& center, float half_height, float radius,
                      const Quat& quat, const Color& color, bool wireframe);
    void DrawCapsule(const Vec3& center, float half_height, float radius,
                     const Quat& quat, const Color& color, bool wireframe);
    void DrawLine(const Vec3& p1, const Vec3& p2, const Color& color1,
              const Color& color2);
    void DrawBox(const Vec3& center, const Vec3& half_size,
                 const Quat& rotation, const Color& color);
    void DrawTriangleMesh(std::span<Vec3> vertices, std::span<uint32_t> indices,
                          const Color& color);
    void DrawTriangleMesh(std::span<Vec3> vertices, std::span<uint16_t> indices,
                          const Color& color);

private:
    GLTFVertexData m_sphere_data;
    GLTFVertexData m_cylinder_data;
    GLTFVertexData m_semi_sphere_data;
};

}  // namespace nickel::graphics