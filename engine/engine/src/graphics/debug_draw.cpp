#include "nickel/graphics/debug_draw.hpp"

#include "nickel/nickel.hpp"

namespace nickel::graphics {

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
    vertices.resize(indices.size());
    for (auto idx : indices) {
        auto p = points[idx];
        vertices.push_back(Vertex{p, color});
    }
    graphics_ctx.DrawLineList(vertices);
}

}  // namespace nickel::graphics