#pragma once
#include "common/assert.hpp"
#include "glad/glad.h"
#include "graphics/rhi/common.hpp"


namespace nickel::rhi::gl4 {

#define CASE(a, b) \
    case a:        \
        return b;

inline GLenum ShaderStage2GL(ShaderStage stage) {
    switch (stage) {
        CASE(ShaderStage::Vertex, GL_VERTEX_SHADER);
        CASE(ShaderStage::Compute, GL_COMPUTE_SHADER);
        CASE(ShaderStage::Fragment, GL_FRAGMENT_SHADER);
    }
}

inline GLenum Filter2GL(Filter filter) {
    switch (filter) {
        CASE(Filter::Nearest, GL_NEAREST);
        CASE(Filter::Linear, GL_LINEAR);
    }
}

inline GLenum SamplerAddressMode2GL(SamplerAddressMode mode) {
    switch (mode) {
        CASE(SamplerAddressMode::ClampToEdge, GL_CLAMP_TO_EDGE);
        CASE(SamplerAddressMode::Repeat, GL_REPEAT);
        CASE(SamplerAddressMode::MirrorRepeat, GL_MIRRORED_REPEAT);
    }
}

inline GLenum CompareOp2GL(CompareOp op) {
    switch (op) {
        CASE(CompareOp::Never, GL_NEVER);
        CASE(CompareOp::Less, GL_LESS);
        CASE(CompareOp::Equal, GL_EQUAL);
        CASE(CompareOp::LessEqual, GL_LEQUAL);
        CASE(CompareOp::Greater, GL_GREATER);
        CASE(CompareOp::NotEqual, GL_NOTEQUAL);
        CASE(CompareOp::GreaterEqual, GL_GEQUAL);
        CASE(CompareOp::Always, GL_ALWAYS);
    }
}

inline GLenum PolygonMode2GL(PolygonMode mode) {
    switch (mode) {
        CASE(PolygonMode::Line, GL_LINE);
        CASE(PolygonMode::Fill, GL_FILL);
        CASE(PolygonMode::Point, GL_POINT);
    }
}

inline GLenum FrontFace2GL(FrontFace ff) {
    switch (ff) {
        case FrontFace::CW:
            return GL_CW;
        case FrontFace::CCW:
            return GL_CCW;
    }
}

inline GLenum CullMode2GL(Flags<CullMode> mode) {
    if (mode & CullMode::Front && mode & CullMode::Back) {
        return GL_FRONT_AND_BACK;
    }

    switch (static_cast<CullMode>(mode)) {
        case CullMode::None:
            return GL_NONE;
        case CullMode::Back:
            return GL_BACK;
        case CullMode::Front:
            return GL_FRONT;
    }
}

inline GLenum StencilOpEnum2GL(StencilOp op) {
    switch (op) {
        case StencilOp::Keep:
            return GL_KEEP;
        case StencilOp::Zero:
            return GL_ZERO;
        case StencilOp::Replace:
            return GL_REPLACE;
        case StencilOp::IncrementAndClamp:
            return GL_INCR;
        case StencilOp::IncrementAndWrap:
            return GL_INCR_WRAP;
        case StencilOp::DecrementAndClamp:
            return GL_DECR;
        case StencilOp::DecrementAndWrap:
            return GL_DECR_WRAP;
        case StencilOp::Invert:
            return GL_INVERT;
    }
}

/*
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
*/

inline GLenum Topology2GL(Topology type) {
    switch (type) {
        case Topology::PointList:
            return GL_POINTS;
        case Topology::LineStrip:
            return GL_LINE_STRIP;
        case Topology::LineList:
            return GL_LINES;
        case Topology::TriangleList:
            return GL_TRIANGLES;
        case Topology::TriangleStrip:
            return GL_TRIANGLE_STRIP;
        case Topology::TriangleFan:
            return GL_TRIANGLE_FAN;
        default:
            Assert(false, "invaid primitive type");
            return -1;
    }
}

inline GLenum BlendOp2GL(BlendOp op) {
    switch (op) {
        case BlendOp::Add:
            return GL_FUNC_ADD;
        case BlendOp::Subtract:
            return GL_FUNC_SUBTRACT;
        case BlendOp::ReverseSubtract:
            return GL_FUNC_REVERSE_SUBTRACT;
        case BlendOp::Min:
            return GL_MIN;
        case BlendOp::Max:
            return GL_MAX;
    }
}

inline GLenum BlendFactor2GL(BlendFactor op) {
    switch (op) {
        case BlendFactor::Zero:
            return GL_ZERO;
        case BlendFactor::One:
            return GL_ONE;
        case BlendFactor::SrcColor:
            return GL_SRC_COLOR;
        case BlendFactor::OneMinusSrcColor:
            return GL_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DstColor:
            return GL_DST_COLOR;
        case BlendFactor::OneMinusDstColor:
            return GL_ONE_MINUS_DST_COLOR;
        case BlendFactor::SrcAlpha:
            return GL_SRC_ALPHA;
        case BlendFactor::OneMinusSrcAlpha:
            return GL_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DstAlpha:
            return GL_DST_ALPHA;
        case BlendFactor::OneMinusDstAlpha:
            return GL_ONE_MINUS_DST_ALPHA;
        case BlendFactor::ConstantColor:
            return GL_CONSTANT_ALPHA;
        case BlendFactor::OneMinusConstantColor:
            return GL_ONE_MINUS_CONSTANT_COLOR;
        case BlendFactor::ConstantAlpha:
            return GL_CONSTANT_ALPHA;
        case BlendFactor::OneMinusConstantAlpha:
            return GL_ONE_MINUS_CONSTANT_ALPHA;
        case BlendFactor::SrcAlphaSaturate:
            return GL_SRC_ALPHA_SATURATE;
    }
}

inline GLenum BufferUsageFlag2GL(Flags<BufferUsage> flags) {
    if (flags & static_cast<uint32_t>(BufferUsage::Vertex)) {
        return GL_ARRAY_BUFFER;
    }
    if (flags & static_cast<uint32_t>(BufferUsage::Index)) {
        return GL_ELEMENT_ARRAY_BUFFER;
    }
    if (flags & static_cast<uint32_t>(BufferUsage::CopyDst)) {
        return GL_COPY_READ_BUFFER;
    }
    if (flags & static_cast<uint32_t>(BufferUsage::CopySrc)) {
        return GL_COPY_WRITE_BUFFER;
    }
    if (flags & static_cast<uint32_t>(BufferUsage::Uniform)) {
        return GL_UNIFORM_BUFFER;
    }
    if (flags & static_cast<uint32_t>(BufferUsage::Storage)) {
        return GL_SHADER_STORAGE_BUFFER;
    }
    if (flags & static_cast<uint32_t>(BufferUsage::Indirect)) {
        /*
        return GL_DRAW_INDIRECT_BUFFER;
        return GL_DISPATCH_INDIRECT_BUFFER;
        */
        // NOTE: opengl separate indirect buffer in two, must unify with Vulkan
        Assert(false, "don't support indirect buffer in GL currently");
    }
    if (flags & static_cast<uint32_t>(BufferUsage::QueryResolve)) {
        return GL_QUERY_BUFFER;
    }

    return GL_ARRAY_BUFFER;
}

inline GLenum TextureFormat2GL(TextureFormat fmt) {
    switch (fmt) {
        case TextureFormat::R8_UNORM:
        case TextureFormat::R8_SNORM:
        case TextureFormat::R8_UINT:
        case TextureFormat::R8_SINT:
            return GL_RED;
        case TextureFormat::R16_UINT:
        case TextureFormat::R16_SINT:
        case TextureFormat::R16_FLOAT:
        case TextureFormat::RG8_UNORM:
        case TextureFormat::RG8_SNORM:
        case TextureFormat::RG8_UINT:
        case TextureFormat::RG8_SINT:
            return GL_RG;
        case TextureFormat::R32_UINT:
        case TextureFormat::R32_SINT:
        case TextureFormat::R32_FLOAT:
        case TextureFormat::RG16_UINT:
        case TextureFormat::RG16_SINT:
        case TextureFormat::RG16_FLOAT:
            return GL_RGB;
        case TextureFormat::RGBA8_UNORM:
        case TextureFormat::RGBA8_UNORM_SRGB:
        case TextureFormat::RGBA8_SNORM:
        case TextureFormat::RGBA8_UINT:
        case TextureFormat::RGBA8_SINT:
        case TextureFormat::BGRA8_UNORM:
        case TextureFormat::BGRA8_UNORM_SRGB:
        case TextureFormat::RGB9E5_UFLOAT:
        case TextureFormat::RGB10A2_UINT:
        case TextureFormat::RGB10A2_UNORM:
        case TextureFormat::RG11B10_UFLOAT:
        case TextureFormat::RG32_UINT:
        case TextureFormat::RG32_SINT:
        case TextureFormat::RG32_FLOAT:
        case TextureFormat::RGBA16_UINT:
        case TextureFormat::RGBA16_SINT:
        case TextureFormat::RGBA16_FLOAT:
        case TextureFormat::RGBA32_UINT:
        case TextureFormat::RGBA32_SINT:
        case TextureFormat::RGBA32_FLOAT:
            return GL_RGBA;
        case TextureFormat::STENCIL8:
            return GL_STENCIL_INDEX;
        case TextureFormat::DEPTH16_UNORM:
        case TextureFormat::DEPTH24_PLUS:
        case TextureFormat::DEPTH32_FLOAT:
            return GL_DEPTH_COMPONENT;
        case TextureFormat::DEPTH24_PLUS_STENCIL8:
        case TextureFormat::DEPTH32_FLOAT_STENCIL8:
            return GL_DEPTH_STENCIL;
        case TextureFormat::BC1_RGBA_UNORM:
        case TextureFormat::BC1_RGBA_UNORM_SRGB:
        case TextureFormat::BC2_RGBA_UNORM:
        case TextureFormat::BC2_RGBA_UNORM_SRGB:
        case TextureFormat::BC3_RGBA_UNORM:
        case TextureFormat::BC3_RGBA_UNORM_SRGB:
        case TextureFormat::BC4_R_UNORM:
        case TextureFormat::BC4_R_SNORM:
        case TextureFormat::BC5_RG_UNORM:
        case TextureFormat::BC5_RG_SNORM:
        case TextureFormat::BC6H_RGB_UFLOAT:
        case TextureFormat::BC6H_RGB_FLOAT:
        case TextureFormat::BC7_RGBA_UNORM:
        case TextureFormat::BC7_RGBA_UNORM_SRGB:
        case TextureFormat::ETC2_RGB8_UNORM:
        case TextureFormat::ETC2_RGB8_UNORM_SRGB:
        case TextureFormat::ETC2_RGB8A1_UNORM:
        case TextureFormat::ETC2_RGB8A1_UNORM_SRGB:
        case TextureFormat::ETC2_RGBA8_UNORM:
        case TextureFormat::ETC2_RGBA8_UNORM_SRGB:
        case TextureFormat::EAC_R11_UNORM:
        case TextureFormat::EAC_R11_SNORM:
        case TextureFormat::EAC_RG11_UNORM:
        case TextureFormat::EAC_RG11_SNORM:
        case TextureFormat::ASTC_4X4_UNORM:
        case TextureFormat::ASTC_4X4_UNORM_SRGB:
        case TextureFormat::ASTC_5X4_UNORM:
        case TextureFormat::ASTC_5X4_UNORM_SRGB:
        case TextureFormat::ASTC_5X5_UNORM:
        case TextureFormat::ASTC_5X5_UNORM_SRGB:
        case TextureFormat::ASTC_6X5_UNORM:
        case TextureFormat::ASTC_6X5_UNORM_SRGB:
        case TextureFormat::ASTC_6X6_UNORM:
        case TextureFormat::ASTC_6X6_UNORM_SRGB:
        case TextureFormat::ASTC_8X5_UNORM:
        case TextureFormat::ASTC_8X5_UNORM_SRGB:
        case TextureFormat::ASTC_8X6_UNORM:
        case TextureFormat::ASTC_8X6_UNORM_SRGB:
        case TextureFormat::ASTC_8X8_UNORM:
        case TextureFormat::ASTC_8X8_UNORM_SRGB:
        case TextureFormat::ASTC_10X5_UNORM:
        case TextureFormat::ASTC_10X5_UNORM_SRGB:
        case TextureFormat::ASTC_10X6_UNORM:
        case TextureFormat::ASTC_10X6_UNORM_SRGB:
        case TextureFormat::ASTC_10X8_UNORM:
        case TextureFormat::ASTC_10X8_UNORM_SRGB:
        case TextureFormat::ASTC_10X10_UNORM:
        case TextureFormat::ASTC_10X10_UNORM_SRGB:
        case TextureFormat::ASTC_12X10_UNORM:
        case TextureFormat::ASTC_12X10_UNORM_SRGB:
        case TextureFormat::ASTC_12X12_UNORM:
        case TextureFormat::ASTC_12X12_UNORM_SRGB:
            return GL_RGBA;
        default:
            return GL_RGBA;
    }
}

#undef CASE

}  // namespace nickel::rhi::gl4