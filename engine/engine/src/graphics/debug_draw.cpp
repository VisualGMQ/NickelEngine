#include "nickel/graphics/debug_draw.hpp"
#include "nickel/nickel.hpp"

namespace nickel::graphics {

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
        Vertex{rotation * Vec3{center.x + half_size.x, center.y + half_size.y,
center.z + half_size.z},
               color},
        Vertex{rotation * Vec3{center.x - half_size.x, center.y + half_size.y,
center.z + half_size.z},
               color},
        Vertex{rotation * Vec3{center.x - half_size.x, center.y + half_size.y,
center.z - half_size.z},
               color},
        Vertex{rotation * Vec3{center.x + half_size.x, center.y + half_size.y,
center.z - half_size.z},
               color},

        Vertex{rotation * Vec3{center.x + half_size.x, center.y - half_size.y,
center.z + half_size.z},
               color},
        Vertex{rotation * Vec3{center.x - half_size.x, center.y - half_size.y,
center.z + half_size.z},
               color},
        Vertex{rotation * Vec3{center.x - half_size.x, center.y - half_size.y,
center.z - half_size.z},
               color},
        Vertex{rotation * Vec3{center.x + half_size.x, center.y - half_size.y,
center.z - half_size.z},
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