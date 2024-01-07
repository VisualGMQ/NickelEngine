#pragma once

#include "rhi/gl/shader.hpp"
#include "rhi/pipeline.hpp"
#include "rhi/vertex_layout.hpp"

namespace nickel::rhi::gl {

inline GLenum FrontFace2GL(FrontFace ff) {
    switch (ff) {
        case FrontFace::CW:
            return GL_CW;
        case FrontFace::CCW:
            return GL_CCW;
    }
}

inline GLenum PolygonMode2Vk(PolygonMode mode) {
    switch (mode) {
        case PolygonMode::Point:
            return GL_POINT;
        case PolygonMode::Line:
            return GL_LINE;
        case PolygonMode::Fill:
            return GL_FILL;
            break;
    }
}

inline GLenum CullMode2Vk(CullMode mode) {
    switch (mode) {
        case CullMode::None:
            return GL_NONE;
        case CullMode::Back:
            return GL_BACK;
        case CullMode::Front:
            return GL_FRONT;
        case CullMode::Both:
            return GL_FRONT_AND_BACK;
    }
}

inline GLenum CompareOp2GL(CompareOp op) {
    switch (op) {
        case CompareOp::Never:
            return GL_NEVER;
        case CompareOp::Less:
            return GL_LESS;
        case CompareOp::LessEqual:
            return GL_LEQUAL;
        case CompareOp::Equal:
            return GL_EQUAL;
        case CompareOp::GreaterEqual:
            return GL_GEQUAL;
        case CompareOp::Greater:
            return GL_GREATER;
        case CompareOp::Always:
            return GL_ALWAYS;
    }
}

inline GLenum StencilOpEnum2GL(StencilOpEnum op) {
    switch (op) {
        case StencilOpEnum::Keep:
            return GL_KEEP;
        case StencilOpEnum::Zero:
            return GL_ZERO;
        case StencilOpEnum::Replace:
            return GL_REPLACE;
        case StencilOpEnum::Increament:
            return GL_INCR;
        case StencilOpEnum::IncreamentWrap:
            return GL_INCR_WRAP;
        case StencilOpEnum::Decreament:
            return GL_DECR;
        case StencilOpEnum::DecreamentWrap:
            return GL_DECR_WRAP;
        case StencilOpEnum::Invert:
            return GL_INVERT;
    }
}

inline GLenum LogicOp2GL(LogicOp op) {
    switch (op) {
        case LogicOp::Clear:
            return GL_CLEAR;
        case LogicOp::And:
            return GL_AND;
        case LogicOp::AndReverse:
            return GL_AND_REVERSE;
        case LogicOp::AndInverted:
            return GL_AND_INVERTED;
        case LogicOp::Copy:
            return GL_COPY;
        case LogicOp::NoOp:
            return GL_NOOP;
        case LogicOp::Xor:
            return GL_XOR;
        case LogicOp::Or:
            return GL_OR;
        case LogicOp::Nor:
            return GL_NOR;
        case LogicOp::Equivalent:
            return GL_EQUIV;
        case LogicOp::Invert:
            return GL_INVERT;
        case LogicOp::OrReverse:
            return GL_OR_REVERSE;
        case LogicOp::CopyInverted:
            return GL_COPY_INVERTED;
        case LogicOp::OrInverted:
            return GL_OR_INVERTED;
        case LogicOp::Nand:
            return GL_NAND;
        case LogicOp::Set:
            return GL_SET;
    }
}

class GraphicsPipeline : public rhi::GraphicsPipeline {
public:
    GraphicsPipeline(const VertexLayout& vertexLayout,
                     const InputAssemblyState& assemblyState,
                     const std::vector<ShaderModule*>& shaders,
                     const ViewportState& viewportState,
                     const RasterizationState& rasterState,
                     const DepthStencilState& depthStencilState,
                     const MultisampleState& multisampleState,
                     const ColorBlendState& colorBlendState)
        : vertexLayout_{vertexLayout},
          assemblyState_{assemblyState},
          shaders_{shaders},
          viewportState_(viewportState),
          rasterState_(rasterState),
          depthStencilState_(depthStencilState),
          multisampleState_(multisampleState),
          colorBlendState_(colorBlendState) {}

private:
    VertexLayout vertexLayout_;
    InputAssemblyState assemblyState_;
    std::vector<ShaderModule*> shaders_;
    ViewportState viewportState_;
    RasterizationState rasterState_;
    DepthStencilState depthStencilState_;
    MultisampleState multisampleState_;
    ColorBlendState colorBlendState_;
};

}  // namespace nickel::rhi::gl