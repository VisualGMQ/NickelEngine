#include "nickel/graphics/debug_draw.hpp"
#include "nickel/nickel.hpp"

namespace nickel::graphics {

DebugDrawer::DebugDrawer() {
    {
        GLTFVertexDataLoader loader;
        m_sphere_data =
            loader.Load("engine/assets/models/unit_sphere/unit_sphere.gltf")[0];
    }
    {
        GLTFVertexDataLoader loader;
        m_semi_sphere_data = loader.Load(
            "engine/assets/models/unit_semi_sphere/semi_sphere.gltf")[0];
    }
    {
        GLTFVertexDataLoader loader;
        m_cylinder_data =
            loader.Load("engine/assets/models/unit_cylinder/cylinder.gltf")[0];
    }
}

void DebugDrawer::DrawSphere(const Vec3& center, float radius, const Quat& quat,
                             const Color& color, bool wireframe) {
    auto& graphics_ctx = nickel::Context::GetInst().GetGraphicsContext();
    std::vector<Vertex> vertices;
    vertices.reserve(m_sphere_data.m_points.size());
    for (auto& point : m_sphere_data.m_points) {
        Vertex vertex;
        vertex.m_color = color;
        vertex.m_position = quat * point * radius + center;
        vertices.push_back(vertex);
    }
    graphics_ctx.DrawTriangleList(vertices, m_sphere_data.m_indices, wireframe);
}

void DebugDrawer::DrawCylinder(const Vec3& center, float half_height,
                               float radius, const Quat& quat,
                               const Color& color, bool wireframe) {
    auto& graphics_ctx = nickel::Context::GetInst().GetGraphicsContext();
    std::vector<Vertex> vertices;
    vertices.reserve(m_cylinder_data.m_points.size());
    for (auto& point : m_cylinder_data.m_points) {
        Vertex vertex;
        vertex.m_color = color;
        vertex.m_position = quat * Vec3{point.x * radius, point.y * half_height,
                                        point.z * radius} +
                            center;
        vertices.push_back(vertex);
    }
    graphics_ctx.DrawTriangleList(vertices, m_sphere_data.m_indices, wireframe);
}

void DebugDrawer::DrawCapsule(const Vec3& center, float half_height,
                              float radius, const Quat& quat,
                              const Color& color, bool wireframe) {
    auto& graphics_ctx = nickel::Context::GetInst().GetGraphicsContext();
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    vertices.reserve(m_semi_sphere_data.m_points.size() * 2 +
                     m_cylinder_data.m_points.size());
    indices.reserve(m_semi_sphere_data.m_indices.size() * 2 +
                    m_cylinder_data.m_indices.size());

    // top semi-sphere
    for (auto& point : m_semi_sphere_data.m_points) {
        Vertex vertex;
        vertex.m_color = color;
        vertex.m_position =
            quat * point * radius + center + Vec3{0, 0, half_height};
        vertices.push_back(vertex);
    }
    for (auto idx : m_semi_sphere_data.m_indices) {
        indices.push_back(idx);
    }

    // bottom semi-sphere
    for (auto& point : m_semi_sphere_data.m_points) {
        Vertex vertex;
        vertex.m_color = color;
        vertex.m_position = quat * point * Vec3{radius, -radius, radius} +
                            center - Vec3{0, 0, half_height};
        vertices.push_back(vertex);
    }
    size_t old_size = indices.size();
    for (auto idx : m_semi_sphere_data.m_indices) {
        indices.push_back(idx + old_size);
    }

    // cylinder
    for (auto& point : m_cylinder_data.m_points) {
        Vertex vertex;
        vertex.m_color = color;
        vertex.m_position = quat * Vec3{point.x * radius, point.y * half_height,
                                        point.z * radius} +
                            center;
        vertices.push_back(vertex);
    }
    old_size = indices.size();
    for (auto idx : m_cylinder_data.m_indices) {
        indices.push_back(idx + old_size);
    }

    graphics_ctx.DrawTriangleList(vertices, indices, wireframe);
}

void DebugDrawer::DrawLine(const Vec3& p1, const Vec3& p2, const Color& color1,
                           const Color& color2) {
    auto& graphics_ctx = nickel::Context::GetInst().GetGraphicsContext();
    std::array vertices = {
        Vertex{p1, color1},
        Vertex{p2, color2}
    };
    graphics_ctx.DrawLineList(vertices);
}

void DebugDrawer::DrawBox(const Vec3& center, const Vec3& half_size,
                          const Quat& rotation, const Color& color) {
    auto& graphics_ctx = nickel::Context::GetInst().GetGraphicsContext();

    std::array<Vertex, 8> vertices = {
        Vertex{rotation * Vec3{half_size.x, half_size.y, half_size.z} + center,
               color},
        Vertex{  rotation * Vec3{-half_size.x, half_size.y, half_size.z} + center,
               color},
        Vertex{
               rotation * Vec3{-half_size.x, half_size.y, -half_size.z} + center,
               color},
        Vertex{  rotation * Vec3{half_size.x, half_size.y, -half_size.z} + center,
               color},
        Vertex{  rotation * Vec3{half_size.x, -half_size.y, half_size.z} + center,
               color},
        Vertex{
               rotation * Vec3{-half_size.x, -half_size.y, half_size.z} + center,
               color},
        Vertex{
               rotation * Vec3{-half_size.x, -half_size.y, -half_size.z} + center,
               color},
        Vertex{
               rotation * Vec3{half_size.x, -half_size.y, -half_size.z} + center,
               color},
    };

    std::array<Vertex, 12 * 2> lines = {
        // top
        vertices[0],
        vertices[1],
        vertices[1],
        vertices[2],
        vertices[2],
        vertices[3],
        vertices[3],
        vertices[0],

        // side
        vertices[0],
        vertices[4],
        vertices[1],
        vertices[5],
        vertices[2],
        vertices[6],
        vertices[3],
        vertices[7],

        // bottom
        vertices[4],
        vertices[5],
        vertices[5],
        vertices[6],
        vertices[6],
        vertices[7],
        vertices[7],
        vertices[4],
    };
    graphics_ctx.DrawLineList(lines);
}

void DebugDrawer::DrawTriangleMesh(std::span<Vec3> points,
                                   std::span<uint32_t> indices,
                                   const Color& color) {
    auto& graphics_ctx = nickel::Context::GetInst().GetGraphicsContext();
    std::vector<Vertex> vertices;
    vertices.resize(points.size());
    std::ranges::transform(points, vertices.begin(),
                           [=](const Vec3& p) { return Vertex{p, color}; });
    graphics_ctx.DrawTriangleList(vertices, indices);
}

void DebugDrawer::DrawTriangleMesh(std::span<Vec3> points,
                                   std::span<uint16_t> indices,
                                   const Color& color) {
    auto& graphics_ctx = nickel::Context::GetInst().GetGraphicsContext();
    std::vector<Vertex> vertices;
    std::vector<uint32_t> u32_indices;
    u32_indices.resize(indices.size());
    vertices.resize(points.size());
    std::ranges::copy(indices, u32_indices.begin());
    std::ranges::transform(points, vertices.begin(),
                           [=](const Vec3& p) { return Vertex{p, color}; });
    graphics_ctx.DrawTriangleList(vertices, u32_indices);
}

}  // namespace nickel::graphics