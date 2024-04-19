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
        CASE(ShaderStage::Geometry, GL_GEOMETRY_SHADER);
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
    if (flags & BufferUsage::Vertex) {
        return GL_ARRAY_BUFFER;
    }
    if (flags & BufferUsage::Index) {
        return GL_ELEMENT_ARRAY_BUFFER;
    }
    if (flags & BufferUsage::CopyDst) {
        return GL_COPY_READ_BUFFER;
    }
    if (flags & BufferUsage::CopySrc) {
        return GL_COPY_WRITE_BUFFER;
    }
    if (flags & BufferUsage::Uniform) {
        return GL_UNIFORM_BUFFER;
    }
    if (flags & BufferUsage::Storage) {
        return GL_SHADER_STORAGE_BUFFER;
    }
    if (flags & BufferUsage::Indirect) {
        /*
        return GL_DRAW_INDIRECT_BUFFER;
        return GL_DISPATCH_INDIRECT_BUFFER;
        */
        // NOTE: opengl separate indirect buffer in two, must unify with Vulkan
        Assert(false, "don't support indirect buffer in GL currently");
    }
    if (flags & BufferUsage::QueryResolve) {
        return GL_QUERY_BUFFER;
    }

    return GL_ARRAY_BUFFER;
}

inline GLenum TextureFormat2GLInternal(TextureFormat fmt) {
    switch (fmt) {
        case TextureFormat::R8_UNORM:
            return GL_R8;
        case TextureFormat::R8_SNORM:
            return GL_R8_SNORM;
        case TextureFormat::R8_UINT:
            return GL_R8UI;
        case TextureFormat::R8_SINT:
            return GL_R8I;
        case TextureFormat::R16_UINT:
            return GL_R16UI;
        case TextureFormat::R16_SINT:
            return GL_R16I;
        case TextureFormat::R16_FLOAT:
            return GL_R16F;
        case TextureFormat::RG8_UNORM:
            return GL_RG8;
        case TextureFormat::RG8_SNORM:
            return GL_RG8_SNORM;
        case TextureFormat::RG8_UINT:
            return GL_RG8UI;
        case TextureFormat::RG8_SINT:
            return GL_RG8I;
        case TextureFormat::R32_UINT:
            return GL_R32UI;
        case TextureFormat::R32_SINT:
            return GL_R32I;
        case TextureFormat::R32_FLOAT:
            return GL_R32F;
        case TextureFormat::RG16_UINT:
            return GL_RG16UI;
        case TextureFormat::RG16_SINT:
            return GL_RG16I;
        case TextureFormat::RG16_FLOAT:
            return GL_RG16F;
        case TextureFormat::RGBA8_UNORM:
            return GL_RGBA8;
        case TextureFormat::RGBA8_UNORM_SRGB:
            return GL_SRGB8_ALPHA8;
        case TextureFormat::RGBA8_SNORM:
            return GL_RGBA8_SNORM;
        case TextureFormat::RGBA8_UINT:
            return GL_RGBA8UI;
        case TextureFormat::RGBA8_SINT:
            return GL_RGBA8I;
        case TextureFormat::BGRA8_UNORM:
            return GL_BGRA;
        case TextureFormat::BGRA8_UNORM_SRGB:
            return GL_SRGB;
        case TextureFormat::RGB9E5_UFLOAT:
            return GL_RGB9_E5;
        case TextureFormat::RGB10A2_UINT:
            return GL_RGB10_A2UI;
        case TextureFormat::RGB10A2_UNORM:
            return GL_RGB10_A2;
        case TextureFormat::RG11B10_UFLOAT:
            // return GL_B
        case TextureFormat::RG32_UINT:
            return GL_RG32UI;
        case TextureFormat::RG32_SINT:
            return GL_RG32I;
        case TextureFormat::RG32_FLOAT:
            return GL_RG32F;
        case TextureFormat::RGBA16_UINT:
            return GL_RGBA16UI;
        case TextureFormat::RGBA16_SINT:
            return GL_RGBA16I;
        case TextureFormat::RGBA16_FLOAT:
            return GL_RGBA16F;
        case TextureFormat::RGBA32_UINT:
            return GL_RGBA32UI;
        case TextureFormat::RGBA32_SINT:
            return GL_RGBA32I;
        case TextureFormat::RGBA32_FLOAT:
            return GL_RGBA32F;
        case TextureFormat::STENCIL8:
            return GL_STENCIL_COMPONENTS;
        case TextureFormat::DEPTH16_UNORM:
            return GL_DEPTH_COMPONENT16;
        case TextureFormat::DEPTH24_PLUS:
            return GL_DEPTH_COMPONENT24;
        case TextureFormat::DEPTH32_FLOAT:
            return GL_DEPTH_COMPONENT32F;
        case TextureFormat::DEPTH24_PLUS_STENCIL8:
            return GL_DEPTH24_STENCIL8;
        case TextureFormat::DEPTH32_FLOAT_STENCIL8:
            return GL_DEPTH32F_STENCIL8;
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
        case TextureFormat::BGRA8_UNORM:
        case TextureFormat::BGRA8_UNORM_SRGB:
            return GL_BGRA;
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

inline GLenum TextureFormat2GLDataType(TextureFormat fmt) {
    switch (fmt) {
        case TextureFormat::R8_UNORM:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::R8_SNORM:
        case TextureFormat::R8_UINT:
            return GL_UNSIGNED_INT;
        case TextureFormat::R8_SINT:
            return GL_INT;
        case TextureFormat::R16_UINT:
            return GL_UNSIGNED_SHORT;
        case TextureFormat::R16_SINT:
            return GL_SHORT;
        case TextureFormat::R16_FLOAT:
            return GL_HALF_FLOAT;
        case TextureFormat::RG8_UNORM:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::RG8_SNORM:
            return GL_BYTE;
        case TextureFormat::RG8_UINT:
            return GL_UNSIGNED_INT;
        case TextureFormat::RG8_SINT:
            return GL_INT;
        case TextureFormat::R32_UINT:
            return GL_UNSIGNED_INT;
        case TextureFormat::R32_SINT:
            return GL_INT;
        case TextureFormat::R32_FLOAT:
            return GL_FLOAT;
        case TextureFormat::RG16_UINT:
            return GL_UNSIGNED_INT;
        case TextureFormat::RG16_SINT:
            return GL_INT;
        case TextureFormat::RG16_FLOAT:
            return GL_FLOAT;
        case TextureFormat::RGBA8_UNORM:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::RGBA8_UNORM_SRGB:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::RGBA8_SNORM:
            return GL_BYTE;
        case TextureFormat::RGBA8_UINT:
            return GL_UNSIGNED_INT;
        case TextureFormat::RGBA8_SINT:
            return GL_INT;
        case TextureFormat::BGRA8_UNORM:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::BGRA8_UNORM_SRGB:
            return GL_BYTE;
        case TextureFormat::RGB9E5_UFLOAT:
            return GL_FLOAT;
        case TextureFormat::RGB10A2_UINT:
            return GL_UNSIGNED_INT_10_10_10_2;
        case TextureFormat::RGB10A2_UNORM:
            return GL_UNSIGNED_INT_10_10_10_2;
        case TextureFormat::RG11B10_UFLOAT:
            return GL_FLOAT;
        case TextureFormat::RG32_UINT:
            return GL_UNSIGNED_INT;
        case TextureFormat::RG32_SINT:
            return GL_INT;
        case TextureFormat::RG32_FLOAT:
            return GL_FLOAT;
        case TextureFormat::RGBA16_UINT:
            return GL_UNSIGNED_INT;
        case TextureFormat::RGBA16_SINT:
            return GL_INT;
        case TextureFormat::RGBA16_FLOAT:
            return GL_FLOAT;
        case TextureFormat::RGBA32_UINT:
            return GL_UNSIGNED_INT;
        case TextureFormat::RGBA32_SINT:
            return GL_INT;
        case TextureFormat::RGBA32_FLOAT:
            return GL_FLOAT;
        case TextureFormat::STENCIL8:
            return GL_STENCIL_COMPONENTS;
        case TextureFormat::DEPTH16_UNORM:
            return GL_UNSIGNED_SHORT;
        case TextureFormat::DEPTH24_PLUS:
            return GL_UNSIGNED_INT;
        case TextureFormat::DEPTH32_FLOAT:
            return GL_FLOAT;
        case TextureFormat::DEPTH24_PLUS_STENCIL8:
            return GL_UNSIGNED_INT_24_8;
        case TextureFormat::DEPTH32_FLOAT_STENCIL8:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::BC1_RGBA_UNORM:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::BC1_RGBA_UNORM_SRGB:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::BC2_RGBA_UNORM:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::BC2_RGBA_UNORM_SRGB:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::BC3_RGBA_UNORM:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::BC3_RGBA_UNORM_SRGB:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::BC4_R_UNORM:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::BC4_R_SNORM:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::BC5_RG_UNORM:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::BC5_RG_SNORM:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::BC6H_RGB_UFLOAT:
            return GL_FLOAT;
        case TextureFormat::BC6H_RGB_FLOAT:
            return GL_FLOAT;
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
            return GL_UNSIGNED_BYTE;
        default:
            return GL_RGBA;
    }
}

inline GLenum TextureViewType2GL(TextureViewType type) {
    switch (type) {
        case TextureViewType::Dim1:
            return GL_TEXTURE_1D;
        case TextureViewType::Dim2:
            return GL_TEXTURE_2D;
        case TextureViewType::Dim3:
            return GL_TEXTURE_3D;
        case TextureViewType::Dim2Array:
            return GL_TEXTURE_2D_ARRAY;
        case TextureViewType::Cube:
            return GL_TEXTURE_CUBE_MAP;
        case TextureViewType::CubeArray:
            return GL_TEXTURE_CUBE_MAP_ARRAY;
    }
}

inline GLenum GetVertexFormatGLType(VertexFormat fmt) {
    switch (fmt) {
        case VertexFormat::Uint8x2:
        case VertexFormat::Uint8x4:
        case VertexFormat::Unorm8x2:
        case VertexFormat::Unorm8x4:
            return GL_UNSIGNED_BYTE;
        case VertexFormat::Sint8x2:
        case VertexFormat::Sint8x4:
        case VertexFormat::Snorm8x2:
        case VertexFormat::Snorm8x4:
            return GL_BYTE;
        case VertexFormat::Uint16x2:
        case VertexFormat::Uint16x4:
        case VertexFormat::Unorm16x2:
        case VertexFormat::Unorm16x4:
            return GL_UNSIGNED_SHORT;
        case VertexFormat::Sint16x2:
        case VertexFormat::Sint16x4:
        case VertexFormat::Snorm16x2:
        case VertexFormat::Snorm16x4:
            return GL_SHORT;
        case VertexFormat::Float16x2:
        case VertexFormat::Float16x4:
            return GL_HALF_FLOAT;
        case VertexFormat::Float32:
        case VertexFormat::Float32x2:
        case VertexFormat::Float32x3:
        case VertexFormat::Float32x4:
            return GL_FLOAT;
        case VertexFormat::Uint32:
        case VertexFormat::Uint32x2:
        case VertexFormat::Uint32x3:
        case VertexFormat::Uint32x4:
            return GL_UNSIGNED_INT;
        case VertexFormat::Sint32:
        case VertexFormat::Sint32x2:
        case VertexFormat::Sint32x3:
        case VertexFormat::Sint32x4:
            return GL_INT;
        case VertexFormat::Unorm10_10_10_2:
            return GL_UNSIGNED_INT_10_10_10_2;
    }
}

inline uint32_t GetVertexFormatComponentCount(VertexFormat fmt) {
    switch (fmt) {
        case VertexFormat::Float32:
        case VertexFormat::Uint32:
        case VertexFormat::Sint32:
        case VertexFormat::Unorm10_10_10_2:
            return 1;
        case VertexFormat::Uint8x2:
        case VertexFormat::Unorm8x2:
        case VertexFormat::Sint8x2:
        case VertexFormat::Snorm8x2:
        case VertexFormat::Uint16x2:
        case VertexFormat::Unorm16x2:
        case VertexFormat::Sint16x2:
        case VertexFormat::Snorm16x2:
        case VertexFormat::Float16x2:
        case VertexFormat::Float32x2:
        case VertexFormat::Uint32x2:
        case VertexFormat::Sint32x2:
            return 2;
        case VertexFormat::Float32x3:
        case VertexFormat::Uint32x3:
        case VertexFormat::Sint32x3:
            return 3;
        case VertexFormat::Uint8x4:
        case VertexFormat::Unorm8x4:
        case VertexFormat::Sint8x4:
        case VertexFormat::Snorm8x4:
        case VertexFormat::Uint16x4:
        case VertexFormat::Unorm16x4:
        case VertexFormat::Sint16x4:
        case VertexFormat::Snorm16x4:
        case VertexFormat::Float16x4:
        case VertexFormat::Float32x4:
        case VertexFormat::Uint32x4:
        case VertexFormat::Sint32x4:
            return 4;
    }
}

inline bool IsNormalizedVertexFormat(VertexFormat fmt) {
    switch (fmt) {
        case VertexFormat::Uint8x2:
        case VertexFormat::Uint8x4:
        case VertexFormat::Sint8x2:
        case VertexFormat::Sint8x4:
        case VertexFormat::Uint16x2:
        case VertexFormat::Uint16x4:
        case VertexFormat::Sint16x2:
        case VertexFormat::Sint16x4:
        case VertexFormat::Float16x2:
        case VertexFormat::Float16x4:
        case VertexFormat::Float32:
        case VertexFormat::Float32x2:
        case VertexFormat::Float32x3:
        case VertexFormat::Float32x4:
        case VertexFormat::Uint32:
        case VertexFormat::Uint32x2:
        case VertexFormat::Uint32x3:
        case VertexFormat::Uint32x4:
        case VertexFormat::Sint32:
        case VertexFormat::Sint32x2:
        case VertexFormat::Sint32x3:
        case VertexFormat::Sint32x4:
            return false;
        case VertexFormat::Unorm8x2:
        case VertexFormat::Unorm8x4:
        case VertexFormat::Snorm8x2:
        case VertexFormat::Snorm8x4:
        case VertexFormat::Unorm16x2:
        case VertexFormat::Unorm16x4:
        case VertexFormat::Snorm16x2:
        case VertexFormat::Snorm16x4:
        case VertexFormat::Unorm10_10_10_2:
            return true;
    }
}

#undef CASE

}  // namespace nickel::rhi::gl4