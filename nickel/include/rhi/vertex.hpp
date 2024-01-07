#pragma once

#include "core/cgmath.hpp"
#include "rhi/vertex_layout.hpp"

namespace nickel::rhi {

struct Vertex final {
    cgmath::Vec3 position;
    cgmath::Vec2 texcoord;
    cgmath::Vec4 color;

    static Vertex FromPosition(const cgmath::Vec2& position) {
        return {cgmath::Vec3{position.x, position.y, 0}, };
    }

    static Vertex FromPosColor(const cgmath::Vec2& position,
                               const cgmath::Vec4& color) {
        return {cgmath::Vec3{position.x, position.y, 0}, {}, color};
    }

    static const VertexLayout& Layout() {
        static VertexLayout layout = VertexLayout::CreateFromTypes({
            Attribute::Type::Vec3,
            Attribute::Type::Vec2,
            Attribute::Type::Vec4,
        });

        return layout;
    }
};

}  // namespace nickel