#pragma once
#include "nickel/common/assert.hpp"
#include "nickel/common/flags.hpp"
#include "nickel/graphics/enums.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

#define GET_BIT(flags, src, dst) (flags & src ? dst : 0)

constexpr VkImageAspectFlagBits ImageAspect2Vk(Flags<ImageAspect> aspect) {
    return static_cast<VkImageAspectFlagBits>(
        GET_BIT(aspect, ImageAspect::Color, VK_IMAGE_ASPECT_COLOR_BIT) |
        GET_BIT(aspect, ImageAspect::Depth, VK_IMAGE_ASPECT_DEPTH_BIT) |
        GET_BIT(aspect, ImageAspect::Stencil, VK_IMAGE_ASPECT_STENCIL_BIT));
}

#define CASE(a, b) \
    case (a):      \
        return (b);

inline Flags<VkBufferUsageFlagBits> BufferUsage2Vk(BufferUsage usage) {
    uint32_t bits = 0;
    if (static_cast<uint32_t>(usage) &
        static_cast<uint32_t>(BufferUsage::CopySrc)) {
        bits |= static_cast<uint32_t>(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    }
    if (static_cast<uint32_t>(usage) &
        static_cast<uint32_t>(BufferUsage::CopyDst)) {
        bits |= static_cast<uint32_t>(VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    }
    if (static_cast<uint32_t>(usage) &
        static_cast<uint32_t>(BufferUsage::Vertex)) {
        bits |= static_cast<uint32_t>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    }
    if (static_cast<uint32_t>(usage) &
        static_cast<uint32_t>(BufferUsage::Index)) {
        bits |= static_cast<uint32_t>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    }
    if (static_cast<uint32_t>(usage) &
        static_cast<uint32_t>(BufferUsage::Uniform)) {
        bits |= static_cast<uint32_t>(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    }
    if (static_cast<uint32_t>(usage) &
        static_cast<uint32_t>(BufferUsage::Index)) {
        bits |= static_cast<uint32_t>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    }
    if (static_cast<uint32_t>(usage) &
        static_cast<uint32_t>(BufferUsage::Storage)) {
        bits |= static_cast<uint32_t>(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    }
    return bits;
}

inline VkFilter Filter2Vk(Filter filter) {
    switch (filter) {
        CASE(Filter::Nearest, VK_FILTER_NEAREST)
        CASE(Filter::Linear, VK_FILTER_LINEAR)
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkSamplerAddressMode SamplerAddressMode2Vk(SamplerAddressMode mode) {
    switch (mode) {
        CASE(SamplerAddressMode::ClampToEdge,
             VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
        CASE(SamplerAddressMode::Repeat, VK_SAMPLER_ADDRESS_MODE_REPEAT)
        CASE(SamplerAddressMode::MirrorRepeat,
             VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT);
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkCompareOp CompareOp2Vk(CompareOp op) {
    switch (op) {
        CASE(CompareOp::Never, VK_COMPARE_OP_NEVER)
        CASE(CompareOp::Less, VK_COMPARE_OP_LESS)
        CASE(CompareOp::Equal, VK_COMPARE_OP_EQUAL)
        CASE(CompareOp::LessEqual, VK_COMPARE_OP_LESS_OR_EQUAL)
        CASE(CompareOp::Greater, VK_COMPARE_OP_GREATER)
        CASE(CompareOp::NotEqual, VK_COMPARE_OP_NOT_EQUAL)
        CASE(CompareOp::GreaterEqual, VK_COMPARE_OP_GREATER_OR_EQUAL)
        CASE(CompareOp::Always, VK_COMPARE_OP_ALWAYS)
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkImageType ImageType2Vk(ImageType type) {
    switch (type) {
        CASE(ImageType::Dim1, VK_IMAGE_TYPE_1D);
        CASE(ImageType::Dim2, VK_IMAGE_TYPE_2D);
        CASE(ImageType::Dim3, VK_IMAGE_TYPE_3D);
    }

    NICKEL_CANT_REACH();
    return {};
}

inline Flags<VkImageUsageFlagBits> ImageUsage2Vk(Flags<ImageUsage> usage) {
    uint32_t flags = 0;
    if (usage & ImageUsage::Sampled) {
        flags |= static_cast<uint32_t>(VK_IMAGE_USAGE_SAMPLED_BIT);
    }
    if (usage & ImageUsage::CopySrc) {
        flags |= static_cast<uint32_t>(VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
    }
    if (usage & ImageUsage::CopyDst) {
        flags |= static_cast<uint32_t>(VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    }
    if (usage & ImageUsage::StorageBinding) {
        flags |= static_cast<uint32_t>(VK_IMAGE_USAGE_STORAGE_BIT);
    }
    if (usage & ImageUsage::ColorAttachment) {
        flags |= static_cast<uint32_t>(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    }
    if (usage & ImageUsage::DepthStencilAttachment) {
        flags |=
            static_cast<uint32_t>(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

    return flags;
}

inline VkSampleCountFlagBits SampleCount2Vk(SampleCount count) {
    switch (count) {
        CASE(SampleCount::Count1, VK_SAMPLE_COUNT_1_BIT);
        CASE(SampleCount::Count2, VK_SAMPLE_COUNT_2_BIT);
        CASE(SampleCount::Count4, VK_SAMPLE_COUNT_4_BIT);
        CASE(SampleCount::Count8, VK_SAMPLE_COUNT_8_BIT);
        CASE(SampleCount::Count16, VK_SAMPLE_COUNT_16_BIT);
        CASE(SampleCount::Count32, VK_SAMPLE_COUNT_32_BIT);
        CASE(SampleCount::Count64, VK_SAMPLE_COUNT_64_BIT);
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkFormat ImageFormat2Vk(ImageFormat f) {
    return static_cast<VkFormat>(f);
}

inline VkImageViewType ImageViewType2Vk(ImageViewType type) {
    switch (type) {
        CASE(ImageViewType::Dim1, VK_IMAGE_VIEW_TYPE_1D);
        CASE(ImageViewType::Dim2, VK_IMAGE_VIEW_TYPE_2D);
        CASE(ImageViewType::Dim3, VK_IMAGE_VIEW_TYPE_3D);
        CASE(ImageViewType::Dim2Array, VK_IMAGE_VIEW_TYPE_2D_ARRAY);
        CASE(ImageViewType::Cube, VK_IMAGE_VIEW_TYPE_CUBE);
        CASE(ImageViewType::CubeArray, VK_IMAGE_VIEW_TYPE_CUBE_ARRAY);
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkShaderStageFlags ShaderStage2Vk(Flags<ShaderStage> stage) {
    uint32_t result = 0;
    if (stage & ShaderStage::Vertex) {
        result |= static_cast<uint32_t>(VK_SHADER_STAGE_VERTEX_BIT);
    }
    if (stage & ShaderStage::Fragment) {
        result |= static_cast<uint32_t>(VK_SHADER_STAGE_FRAGMENT_BIT);
    }
    if (stage & ShaderStage::Compute) {
        result |= static_cast<uint32_t>(VK_SHADER_STAGE_COMPUTE_BIT);
    }
    if (stage & ShaderStage::Geometry) {
        result |= static_cast<uint32_t>(VK_SHADER_STAGE_GEOMETRY_BIT);
    }
    return result;
}

inline VkPrimitiveTopology Topology2Vk(Topology t) {
    switch (t) {
        CASE(Topology::LineList, VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
        CASE(Topology::LineStrip, VK_PRIMITIVE_TOPOLOGY_LINE_STRIP);
        CASE(Topology::PointList, VK_PRIMITIVE_TOPOLOGY_POINT_LIST);
        CASE(Topology::TriangleList, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        CASE(Topology::TriangleStrip, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);
        CASE(Topology::TriangleFan, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN);
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkCullModeFlagBits CullMode2Vk(CullMode mode) {
    if (mode == CullMode::None) {
        return VK_CULL_MODE_NONE;
    }
    if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(CullMode::Front)) {
        return VK_CULL_MODE_FRONT_BIT;
    }
    if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(CullMode::Back)) {
        return VK_CULL_MODE_BACK_BIT;
    }
    if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(CullMode::Back) &&
        static_cast<uint32_t>(mode) & static_cast<uint32_t>(CullMode::Front)) {
        return VK_CULL_MODE_FRONT_AND_BACK;
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkPolygonMode PolygonMode2Vk(PolygonMode mode) {
    switch (mode) {
        CASE(PolygonMode::Line, VK_POLYGON_MODE_LINE);
        CASE(PolygonMode::Fill, VK_POLYGON_MODE_FILL);
        CASE(PolygonMode::Point, VK_POLYGON_MODE_POINT);
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkFrontFace FrontFace2Vk(FrontFace face) {
    switch (face) {
        CASE(FrontFace::CCW, VK_FRONT_FACE_COUNTER_CLOCKWISE);
        CASE(FrontFace::CW, VK_FRONT_FACE_CLOCKWISE);
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkBlendOp BlendOp2Vk(BlendOp op) {
    switch (op) {
        CASE(BlendOp::Add, VK_BLEND_OP_ADD);
        CASE(BlendOp::Subtract, VK_BLEND_OP_SUBTRACT);
        CASE(BlendOp::ReverseSubtract, VK_BLEND_OP_REVERSE_SUBTRACT);
        CASE(BlendOp::Min, VK_BLEND_OP_MIN);
        CASE(BlendOp::Max, VK_BLEND_OP_MAX);
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkBlendFactor BlendFactor2Vk(BlendFactor factor) {
    switch (factor) {
        CASE(BlendFactor::Zero, VK_BLEND_FACTOR_ZERO);
        CASE(BlendFactor::One, VK_BLEND_FACTOR_ONE);
        CASE(BlendFactor::SrcColor, VK_BLEND_FACTOR_SRC_ALPHA);
        CASE(BlendFactor::OneMinusSrcColor,
             VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA);
        CASE(BlendFactor::DstColor, VK_BLEND_FACTOR_DST_COLOR);
        CASE(BlendFactor::OneMinusDstColor,
             VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR);
        CASE(BlendFactor::SrcAlpha, VK_BLEND_FACTOR_SRC_ALPHA);
        CASE(BlendFactor::OneMinusSrcAlpha,
             VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA);
        CASE(BlendFactor::DstAlpha, VK_BLEND_FACTOR_DST_ALPHA);
        CASE(BlendFactor::OneMinusDstAlpha,
             VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA);
        CASE(BlendFactor::ConstantColor, VK_BLEND_FACTOR_CONSTANT_COLOR);
        CASE(BlendFactor::OneMinusConstantColor,
             VK_BLEND_FACTOR_CONSTANT_COLOR);
        CASE(BlendFactor::ConstantAlpha, VK_BLEND_FACTOR_CONSTANT_COLOR);
        CASE(BlendFactor::OneMinusConstantAlpha,
             VK_BLEND_FACTOR_CONSTANT_COLOR);
        CASE(BlendFactor::SrcAlphaSaturate, VK_BLEND_FACTOR_SRC_ALPHA_SATURATE);
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkStencilOp StencilOp2Vk(StencilOp op) {
    switch (op) {
        CASE(StencilOp::Keep, VK_STENCIL_OP_KEEP);
        CASE(StencilOp::Zero, VK_STENCIL_OP_ZERO);
        CASE(StencilOp::Replace, VK_STENCIL_OP_REPLACE);
        CASE(StencilOp::IncrementAndClamp, VK_STENCIL_OP_INCREMENT_AND_CLAMP);
        CASE(StencilOp::DecrementAndClamp, VK_STENCIL_OP_DECREMENT_AND_CLAMP);
        CASE(StencilOp::Invert, VK_STENCIL_OP_INVERT);
        CASE(StencilOp::IncrementAndWrap, VK_STENCIL_OP_INCREMENT_AND_WRAP);
        CASE(StencilOp::DecrementAndWrap, VK_STENCIL_OP_DECREMENT_AND_WRAP);
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkAttachmentLoadOp AttachmentLoadOp2Vk(AttachmentLoadOp op) {
    switch (op) {
        CASE(AttachmentLoadOp::Clear, VK_ATTACHMENT_LOAD_OP_CLEAR);
        CASE(AttachmentLoadOp::Load, VK_ATTACHMENT_LOAD_OP_LOAD);
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkAttachmentStoreOp AttachmentStoreOp2Vk(AttachmentStoreOp op) {
    switch (op) {
        CASE(AttachmentStoreOp::Store, VK_ATTACHMENT_STORE_OP_STORE);
        CASE(AttachmentStoreOp::DontCare, VK_ATTACHMENT_STORE_OP_DONT_CARE);
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkIndexType IndexType2Vk(IndexType type) {
    switch (type) {
        CASE(IndexType::Uint16, VK_INDEX_TYPE_UINT16);
        CASE(IndexType::Uint32, VK_INDEX_TYPE_UINT32);
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkFormat VertexFormat2Vk(VertexFormat fmt) {
    switch (fmt) {
        CASE(VertexFormat::Uint8x2, VK_FORMAT_R8G8_UINT);
        CASE(VertexFormat::Uint8x4, VK_FORMAT_R8G8B8A8_UINT);
        CASE(VertexFormat::Sint8x2, VK_FORMAT_R8G8_SINT);
        CASE(VertexFormat::Sint8x4, VK_FORMAT_R8G8B8A8_SINT);
        CASE(VertexFormat::Unorm8x2, VK_FORMAT_R8G8_UNORM);
        CASE(VertexFormat::Unorm8x4, VK_FORMAT_R8G8B8A8_UNORM);
        CASE(VertexFormat::Snorm8x2, VK_FORMAT_R8G8_SNORM);
        CASE(VertexFormat::Snorm8x4, VK_FORMAT_R8G8B8A8_SNORM);
        CASE(VertexFormat::Uint16x2, VK_FORMAT_R16G16_UINT);
        CASE(VertexFormat::Uint16x4, VK_FORMAT_R16G16B16A16_UINT);
        CASE(VertexFormat::Sint16x2, VK_FORMAT_R16G16_SINT);
        CASE(VertexFormat::Sint16x4, VK_FORMAT_R16G16B16A16_SINT);
        CASE(VertexFormat::Unorm16x2, VK_FORMAT_R16G16_UNORM);
        CASE(VertexFormat::Unorm16x4, VK_FORMAT_R16G16B16A16_SINT);
        CASE(VertexFormat::Snorm16x2, VK_FORMAT_R16G16_SNORM);
        CASE(VertexFormat::Snorm16x4, VK_FORMAT_R16G16B16A16_SINT);
        CASE(VertexFormat::Float16x2, VK_FORMAT_R16G16_SFLOAT);
        CASE(VertexFormat::Float16x4, VK_FORMAT_R16G16B16A16_SFLOAT);
        CASE(VertexFormat::Float32, VK_FORMAT_R32_SFLOAT);
        CASE(VertexFormat::Float32x2, VK_FORMAT_R32G32_SFLOAT);
        CASE(VertexFormat::Float32x3, VK_FORMAT_R32G32B32_SFLOAT);
        CASE(VertexFormat::Float32x4, VK_FORMAT_R32G32B32A32_SFLOAT);
        CASE(VertexFormat::Uint32, VK_FORMAT_R32_UINT);
        CASE(VertexFormat::Uint32x2, VK_FORMAT_R32G32_UINT);
        CASE(VertexFormat::Uint32x3, VK_FORMAT_R32G32B32_UINT);
        CASE(VertexFormat::Uint32x4, VK_FORMAT_R32G32B32A32_UINT);
        CASE(VertexFormat::Sint32, VK_FORMAT_R32_SINT);
        CASE(VertexFormat::Sint32x2, VK_FORMAT_R32G32_SINT);
        CASE(VertexFormat::Sint32x3, VK_FORMAT_R32G32B32_SINT);
        CASE(VertexFormat::Sint32x4, VK_FORMAT_R32G32B32A32_SINT);
    }

    NICKEL_CANT_REACH();
    return {};
}

inline Flags<VkColorComponentFlagBits> ColorWriteMask2Vk(ColorWriteMask mask) {
    if (mask == ColorWriteMask::All) {
        return VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_B_BIT |
               VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_R_BIT;
    }

    Flags<VkColorComponentFlagBits> flags;
    if (static_cast<uint32_t>(mask) &
        static_cast<uint32_t>(ColorWriteMask::Alpha)) {
        flags |= VK_COLOR_COMPONENT_A_BIT;
    }
    if (static_cast<uint32_t>(mask) &
        static_cast<uint32_t>(ColorWriteMask::Green)) {
        flags |= VK_COLOR_COMPONENT_B_BIT;
    }
    if (static_cast<uint32_t>(mask) &
        static_cast<uint32_t>(ColorWriteMask::Blue)) {
        flags |= VK_COLOR_COMPONENT_G_BIT;
    }
    if (static_cast<uint32_t>(mask) &
        static_cast<uint32_t>(ColorWriteMask::Red)) {
        flags |= VK_COLOR_COMPONENT_R_BIT;
    }
    return flags;
}

inline Flags<VkShaderStageFlagBits> ShaderStageType2Vk(ShaderStageType type) {
    switch (type) {
        CASE(ShaderStageType::Vertex, VK_SHADER_STAGE_VERTEX_BIT)
        CASE(ShaderStageType::TessellationControl,
             VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT)
        CASE(ShaderStageType::TessellationEvaluation,
             VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT)
        CASE(ShaderStageType::Geometry, VK_SHADER_STAGE_GEOMETRY_BIT)
        CASE(ShaderStageType::Fragment, VK_SHADER_STAGE_FRAGMENT_BIT)
        CASE(ShaderStageType::Compute, VK_SHADER_STAGE_COMPUTE_BIT)
        CASE(ShaderStageType::AllGraphics, VK_SHADER_STAGE_ALL_GRAPHICS)
        CASE(ShaderStageType::All, VK_SHADER_STAGE_ALL)
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkBorderColor BorderColor2Vk(BorderColor color) {
    switch (color) {
        CASE(BorderColor::FloatTransparentBlack,
             VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK);
        CASE(BorderColor::IntTransparentBlack,
             VK_BORDER_COLOR_INT_TRANSPARENT_BLACK);
        CASE(BorderColor::FloatOpaqueBlack, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK);
        CASE(BorderColor::IntOpaqueBlack, VK_BORDER_COLOR_INT_OPAQUE_BLACK);
        CASE(BorderColor::FloatOpaqueWhite, VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE);
        CASE(BorderColor::IntOpaqueWhite, VK_BORDER_COLOR_INT_OPAQUE_WHITE);
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkSamplerMipmapMode SamplerMipmapMode2Vk(SamplerMipmapMode mode) {
    switch (mode) {
        CASE(SamplerMipmapMode::Nearest, VK_SAMPLER_MIPMAP_MODE_NEAREST);
        CASE(SamplerMipmapMode::Linear, VK_SAMPLER_MIPMAP_MODE_LINEAR);
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkDescriptorType BindGroupEntryType2Vk(BindGroupEntryType type) {
    switch (type) {
        CASE(BindGroupEntryType::Sampler, VK_DESCRIPTOR_TYPE_SAMPLER);
        CASE(BindGroupEntryType::CombinedImageSampler,
             VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        CASE(BindGroupEntryType::SampledImage,
             VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
        CASE(BindGroupEntryType::StorageImage,
             VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
        CASE(BindGroupEntryType::UniformTexelBuffer,
             VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER);
        CASE(BindGroupEntryType::StorageTexelBuffer,
             VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER);
        CASE(BindGroupEntryType::UniformBuffer,
             VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        CASE(BindGroupEntryType::StorageBuffer,
             VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
        CASE(BindGroupEntryType::UniformBufferDynamic,
             VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);
        CASE(BindGroupEntryType::StoragesBufferDynamic,
             VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC);
        CASE(BindGroupEntryType::InputAttachment,
             VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);
        CASE(BindGroupEntryType::InlineUniformBlock,
             VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);
    }

    NICKEL_CANT_REACH();
    return {};
}

#undef GET_BIT
#undef CASE

}  // namespace nickel::graphics