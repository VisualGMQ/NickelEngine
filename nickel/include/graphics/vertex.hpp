#pragma once

#include "common/cgmath.hpp"
#include "rhi/rhi.hpp"

namespace nickel {

struct Vertex2D final {
    cgmath::Vec3 position;
    cgmath::Vec2 texcoord;
    cgmath::Vec4 color;

    static Vertex2D FromPosition(const cgmath::Vec2& position) {
        return {cgmath::Vec3{position.x, position.y, 0}, };
    }

    static Vertex2D FromPosColor(const cgmath::Vec2& position,
                               const cgmath::Vec4& color) {
        return {cgmath::Vec3{position.x, position.y, 0}, {}, color};
    }

    static const rhi::RenderPipeline::BufferState& Layout() {
        static rhi::RenderPipeline::BufferState state{
            rhi::RenderPipeline::BufferState::StepMode::Vertex,
            (3 + 2 + 4) * 4,
            {
                            {rhi::VertexFormat::Float32x3, 0, 0},
                            {rhi::VertexFormat::Float32x2, 3 * 4, 1},
                            {rhi::VertexFormat::Float32x4, 5 * 4, 2},
                            }
        };

        return state;
    }
};

}  // namespace nickel