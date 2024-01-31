#pragma once

#include "graphics/gogl.hpp"
#include "common/cgmath.hpp"

namespace nickel {

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

    static const gogl::BufferLayout& Layout() {
        static gogl::BufferLayout layout = gogl::BufferLayout::CreateFromTypes({
            gogl::Attribute::Type::Vec3,
            gogl::Attribute::Type::Vec2,
            gogl::Attribute::Type::Vec4,
        });

        return layout;
    }
};

}  // namespace nickel