#pragma once

#include "pch.hpp"
#include "rhi/enums.hpp"

namespace nickel::rhi::gl {

inline GLenum FrontFace2GL(FrontFace ff) {
    switch (ff) {
        case FrontFace::CW:
            return GL_CW;
        case FrontFace::CCW:
            return GL_CCW;
    }
}

inline GLenum PolygonMode2GL(PolygonMode mode) {
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

inline GLenum CullMode2GL(CullMode mode) {
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
        case CompareOp::NotEqual:
            return GL_NOTEQUAL;
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

inline GLenum Primitive2GL(PrimitiveType type) {
    switch (type) {
        case PrimitiveType::Points:
            return GL_POINTS;
        case PrimitiveType::LineStrip:
            return GL_LINE_STRIP;
        // case PrimitiveType::LineLoop:
        //     return GL_LINE_LOOP;
        case PrimitiveType::Lines:
            return GL_LINES;
        case PrimitiveType::LineStripAdjacency:
            return GL_LINE_STRIP_ADJACENCY;
        case PrimitiveType::LinesAdjacency:
            return GL_LINES_ADJACENCY;
        case PrimitiveType::TriangleStrip:
            return GL_TRIANGLE_STRIP;
        case PrimitiveType::TriangleFan:
            return GL_TRIANGLE_FAN;
        case PrimitiveType::Triangles:
            return GL_TRIANGLES;
        case PrimitiveType::TriangleStripAdjacency:
            return GL_TRIANGLE_STRIP_ADJACENCY;
        case PrimitiveType::TrianglesAdjacency:
            return GL_TRIANGLES_ADJACENCY;
        case PrimitiveType::Patches:
            return GL_PATCHES;
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
        case BlendFactor::Src1Color:
            return GL_SRC1_COLOR;
        case BlendFactor::OneMinusSrc1Color:
            return GL_ONE_MINUS_SRC1_COLOR;
        case BlendFactor::Src1Alpha:
            return GL_SRC1_ALPHA;
        case BlendFactor::OneMinusSrc1Alpha:
            return GL_ONE_MINUS_SRC1_ALPHA;
    }
}

inline GLenum BufferUsageFlag2GL(BufferUsageFlags flags) {
    if (flags & static_cast<uint32_t>(BufferUsageFlagBits::VertexBuffer)) {
        return GL_ARRAY_BUFFER;
    }
    if (flags & static_cast<uint32_t>(BufferUsageFlagBits::IndexBuffer)) {
        return GL_ELEMENT_ARRAY_BUFFER;
    }
    if (flags & static_cast<uint32_t>(BufferUsageFlagBits::TransferSrc)) {
        return GL_COPY_READ_BUFFER;
    }
    if (flags & static_cast<uint32_t>(BufferUsageFlagBits::TransferSrc)) {
        return GL_COPY_WRITE_BUFFER;
    }
    if (flags & static_cast<uint32_t>(BufferUsageFlagBits::UniformBuffer)) {
        return GL_UNIFORM_BUFFER;
    }
    if (flags & static_cast<uint32_t>(BufferUsageFlagBits::StorageBuffer)) {
        return GL_SHADER_STORAGE_BUFFER;
    }
    if (flags & static_cast<uint32_t>(BufferUsageFlagBits::IndirectBuffer)) {
        /*
        return GL_DRAW_INDIRECT_BUFFER;
        return GL_DISPATCH_INDIRECT_BUFFER;
        */
        // NOTE: opengl separate indirect buffer in two, must unify with Vulkan
        Assert(false, "don't support indirect buffer in GL currently");
    }
    if (flags &
        static_cast<uint32_t>(BufferUsageFlagBits::UniformTexelBuffer)) {
        return GL_PIXEL_PACK_BUFFER;
    }
    if (flags &
        static_cast<uint32_t>(BufferUsageFlagBits::StorageTexelBuffer)) {
        return GL_PIXEL_UNPACK_BUFFER;
    }

    if (flags &
        static_cast<uint32_t>(BufferUsageFlagBits::ShaderDeviceAddress)) {
        // NOTE: maybe use shader storage buffer???
        // return GL_SHADER_STORAGE_BUFFER;
        Assert(false,
               "don't support shader device address buffer in GL currently");
    }

    return GL_ARRAY_BUFFER;
}

inline GLenum ImageViewType2GL(ImageViewType viewType) {
    switch (viewType) {
        case ImageViewType::e1D:
            return GL_TEXTURE_1D;
        case ImageViewType::e2D:
            return GL_TEXTURE_2D;
        case ImageViewType::e3D:
            return GL_TEXTURE_3D;
        case ImageViewType::Cube:
            return GL_TEXTURE_CUBE_MAP;
        case ImageViewType::e1DArray:
            return GL_TEXTURE_1D_ARRAY;
        case ImageViewType::e2DArray:
            return GL_TEXTURE_2D_ARRAY;
        case ImageViewType::CubeArray:
            return GL_TEXTURE_CUBE_MAP_ARRAY;
    }
}

inline GLenum SamplerAddressMode2GL(SamplerAddressMode mode) {
    switch (mode) {
        case SamplerAddressMode::Repeat:
            return GL_REPEAT;
        case SamplerAddressMode::MirroredRepeat:
            return GL_MIRRORED_REPEAT;
        case SamplerAddressMode::ClampToEdge:
            return GL_CLAMP_TO_EDGE;
        case SamplerAddressMode::ClampToBorder:
            return GL_CLAMP_TO_BORDER;
        case SamplerAddressMode::MirrorClampToEdge:
            return GL_MIRROR_CLAMP_TO_EDGE;
        case SamplerAddressMode::MirrorClampToEdgeKHR:
            return GL_MIRROR_CLAMP_TO_EDGE;
    }
};

inline GLenum Filter2GL(Filter filter) {
    switch (filter) {
        case Filter::Nearest:
            return GL_NEAREST;
        case Filter::Linear:
            GL_LINEAR;
        case Filter::CubicEXT:
        case Filter::CubicIMG:
            Assert(false, "opengl don't support filter type");
            return GL_LINEAR;
    }
}

}  // namespace nickel::rhi::gl