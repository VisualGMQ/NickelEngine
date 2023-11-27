#pragma once

#include "core/gogl.hpp"
#include "core/cgmath.hpp"

namespace nickel {

struct Vertex final {
    cgmath::Vec2 position;
    cgmath::Vec2 texcoord;
    cgmath::Vec4 color;

    static Vertex FromPosition(const cgmath::Vec2& position) {
        return {position};
    }

    static Vertex FromPosColor(const cgmath::Vec2& position,
                               const cgmath::Vec4& color) {
        return {position, {}, color};
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