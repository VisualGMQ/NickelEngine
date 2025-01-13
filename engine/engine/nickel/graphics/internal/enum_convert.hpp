#pragma once
#include "nickel/common/assert.hpp"
#include "nickel/common/flags.hpp"
#include "nickel/graphics/enums.hpp"
#include "nickel/internal/pch.hpp"
#include <type_traits>

namespace nickel::graphics {

#define CASE(a, b) \
    case (a):      \
        return (b);

#define TRY_SET_BIT(src, dst) \
    if (flags & (src)) bits |= (dst);

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

inline VkSampleCountFlags SampleCount2Vk(SampleCount count) {
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

inline VkFormat Format2Vk(Format f) {
    return static_cast<VkFormat>(f);
}

inline Format VkFormat2Format(VkFormat f) {
    return static_cast<Format>(f);
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
        CASE(AttachmentLoadOp::DontCare, VK_ATTACHMENT_LOAD_OP_DONT_CARE);
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

inline VkShaderStageFlags ShaderStage2Vk(Flags<ShaderStage> flags) {
    if (flags == ShaderStage::All) {
        return VK_SHADER_STAGE_ALL;
    }

    if (flags == ShaderStage::AllGraphics) {
        return VK_SHADER_STAGE_ALL_GRAPHICS;
    }
    
    VkShaderStageFlags bits = 0;
    TRY_SET_BIT(ShaderStage::Vertex, VK_SHADER_STAGE_VERTEX_BIT)
    TRY_SET_BIT(ShaderStage::TessellationControl,
                VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT)
    TRY_SET_BIT(ShaderStage::TessellationEvaluation,
                VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT)
    TRY_SET_BIT(ShaderStage::Geometry, VK_SHADER_STAGE_GEOMETRY_BIT)
    TRY_SET_BIT(ShaderStage::Fragment, VK_SHADER_STAGE_FRAGMENT_BIT)
    TRY_SET_BIT(ShaderStage::Compute, VK_SHADER_STAGE_COMPUTE_BIT)
    return bits;
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

inline VkImageAspectFlags ImageAspect2Vk(Flags<ImageAspect> flags) {
    VkImageAspectFlags bits = 0;
    TRY_SET_BIT(ImageAspect::Color, VK_IMAGE_ASPECT_COLOR_BIT);
    TRY_SET_BIT(ImageAspect::Depth, VK_IMAGE_ASPECT_DEPTH_BIT);
    TRY_SET_BIT(ImageAspect::Stencil, VK_IMAGE_ASPECT_STENCIL_BIT);
    return bits;
}

inline VkBufferUsageFlags BufferUsage2Vk(Flags<BufferUsage> flags) {
    VkBufferUsageFlags bits = 0;
    TRY_SET_BIT(BufferUsage::CopySrc, VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
    TRY_SET_BIT(BufferUsage::CopyDst, VK_BUFFER_USAGE_TRANSFER_DST_BIT)
    TRY_SET_BIT(BufferUsage::Vertex, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
    TRY_SET_BIT(BufferUsage::Index, VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
    TRY_SET_BIT(BufferUsage::Uniform, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
    TRY_SET_BIT(BufferUsage::Index, VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
    TRY_SET_BIT(BufferUsage::Storage, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
    return bits;
}

inline VkImageUsageFlags ImageUsage2Vk(Flags<ImageUsage> flags) {
    VkImageUsageFlags bits = 0;
    TRY_SET_BIT(ImageUsage::Sampled, VK_IMAGE_USAGE_SAMPLED_BIT)
    TRY_SET_BIT(ImageUsage::CopySrc, VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
    TRY_SET_BIT(ImageUsage::CopyDst, VK_IMAGE_USAGE_TRANSFER_DST_BIT)
    TRY_SET_BIT(ImageUsage::StorageBinding, VK_IMAGE_USAGE_STORAGE_BIT)
    TRY_SET_BIT(ImageUsage::ColorAttachment,
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
    TRY_SET_BIT(ImageUsage::DepthStencilAttachment,
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)

    return bits;
}

inline VkColorComponentFlags ColorWriteMask2Vk(Flags<ColorWriteMask> flags) {
    if (flags == ColorWriteMask::All) {
        return VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_B_BIT |
               VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_R_BIT;
    }

    VkColorComponentFlags bits{};
    TRY_SET_BIT(ColorWriteMask::Alpha, VK_COLOR_COMPONENT_A_BIT)
    TRY_SET_BIT(ColorWriteMask::Green, VK_COLOR_COMPONENT_B_BIT)
    TRY_SET_BIT(ColorWriteMask::Blue, VK_COLOR_COMPONENT_G_BIT)
    TRY_SET_BIT(ColorWriteMask::Red, VK_COLOR_COMPONENT_R_BIT)
    return bits;
}

inline VkCullModeFlags CullMode2Vk(Flags<CullMode> mode) {
    if (mode == CullMode::None) {
        return VK_CULL_MODE_NONE;
    }
    if (mode & CullMode::Front) {
        return VK_CULL_MODE_FRONT_BIT;
    }
    if (mode & CullMode::Back) {
        return VK_CULL_MODE_BACK_BIT;
    }
    if (mode & CullMode::Back && mode & CullMode::Front) {
        return VK_CULL_MODE_FRONT_AND_BACK;
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkPipelineStageFlags PipelineStage2Vk(Flags<PipelineStage> flags) {
    VkPipelineStageFlags bits = 0;

    if (flags == PipelineStage::None) {
        return VK_PIPELINE_STAGE_NONE;
    }

    TRY_SET_BIT(PipelineStage::Host, VK_PIPELINE_STAGE_HOST_BIT)
    TRY_SET_BIT(PipelineStage::Transfer, VK_PIPELINE_STAGE_TRANSFER_BIT)
    TRY_SET_BIT(PipelineStage::AllCommands, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT)
    TRY_SET_BIT(PipelineStage::AllGraphics, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT)
    TRY_SET_BIT(PipelineStage::ComputeShader,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT)
    TRY_SET_BIT(PipelineStage::DrawIndirect,
                VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT)
    TRY_SET_BIT(PipelineStage::FragmentShader,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT)
    TRY_SET_BIT(PipelineStage::VertexInput, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT)
    TRY_SET_BIT(PipelineStage::VertexShader,
                VK_PIPELINE_STAGE_VERTEX_SHADER_BIT)
    TRY_SET_BIT(PipelineStage::GeometryShader,
                VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT)
    TRY_SET_BIT(PipelineStage::BottomOfPipe,
                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT)
    TRY_SET_BIT(PipelineStage::ColorAttachmentOutput,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
    TRY_SET_BIT(PipelineStage::EarlyFragmentTests,
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT)
    TRY_SET_BIT(PipelineStage::LateFragmentTests,
                VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT)
    TRY_SET_BIT(PipelineStage::TessellationControlShader,
                VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT)
    TRY_SET_BIT(PipelineStage::TessellationEvaluationShader,
                VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT)
    TRY_SET_BIT(PipelineStage::TopOfPipe, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT)

    return bits;
}

inline VkAccessFlags Access2Vk(Flags<Access> flags) {
    if (flags == Access::None) {
        return VK_ACCESS_NONE;
    }

    VkAccessFlags bits = 0;

    TRY_SET_BIT(Access::HostRead, VK_ACCESS_HOST_READ_BIT)
    TRY_SET_BIT(Access::HostWrite, VK_ACCESS_HOST_WRITE_BIT)
    TRY_SET_BIT(Access::IndexRead, VK_ACCESS_INDEX_READ_BIT)
    TRY_SET_BIT(Access::MemoryRead, VK_ACCESS_MEMORY_READ_BIT)
    TRY_SET_BIT(Access::MemoryWrite, VK_ACCESS_MEMORY_WRITE_BIT)
    TRY_SET_BIT(Access::ShaderRead, VK_ACCESS_SHADER_READ_BIT)
    TRY_SET_BIT(Access::ShaderWrite, VK_ACCESS_SHADER_WRITE_BIT)
    TRY_SET_BIT(Access::TransferRead, VK_ACCESS_TRANSFER_READ_BIT)
    TRY_SET_BIT(Access::TransferWrite, VK_ACCESS_TRANSFER_WRITE_BIT)
    TRY_SET_BIT(Access::UniformRead, VK_ACCESS_UNIFORM_READ_BIT)
    TRY_SET_BIT(Access::ColorAttachmentRead,
                VK_ACCESS_COLOR_ATTACHMENT_READ_BIT)
    TRY_SET_BIT(Access::ColorAttachmentWrite,
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
    TRY_SET_BIT(Access::IndirectCommandRead,
                VK_ACCESS_INDIRECT_COMMAND_READ_BIT)
    TRY_SET_BIT(Access::InputAttachmentRead,
                VK_ACCESS_INPUT_ATTACHMENT_READ_BIT)
    TRY_SET_BIT(Access::VertexAttributeRead,
                VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)
    TRY_SET_BIT(Access::DepthStencilAttachmentRead,
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT)
    TRY_SET_BIT(Access::DepthStencilAttachmentWrite,
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)

    return bits;
}

inline VkDependencyFlags Dependency2Vk(Flags<Dependency> flags) {
    VkDependencyFlags bits = 0;
    TRY_SET_BIT(Dependency::ByRegionBit, VK_DEPENDENCY_BY_REGION_BIT)
    TRY_SET_BIT(Dependency::DeviceGroupBit, VK_DEPENDENCY_DEVICE_GROUP_BIT)
    TRY_SET_BIT(Dependency::ViewLocalBit, VK_DEPENDENCY_VIEW_LOCAL_BIT)
    return bits;
}

inline VkImageLayout ImageLayout2Vk(ImageLayout layout) {
    switch (layout) {
        CASE(ImageLayout::Undefined, VK_IMAGE_LAYOUT_UNDEFINED)
        CASE(ImageLayout::General, VK_IMAGE_LAYOUT_GENERAL)
        CASE(ImageLayout::Preinitialized, VK_IMAGE_LAYOUT_PREINITIALIZED)
        CASE(ImageLayout::PresentSrcKHR, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
        CASE(ImageLayout::AttachmentOptimal,
             VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        CASE(ImageLayout::ReadOnly_optimal, VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL)
        CASE(ImageLayout::ColorAttachmentOptimal,
             VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        CASE(ImageLayout::DepthAttachmentOptimal,
             VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        CASE(ImageLayout::StencilAttachmentOptimal,
             VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL)
        CASE(ImageLayout::TransferDstOptimal,
             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        CASE(ImageLayout::TransferSrcOptimal,
             VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        CASE(ImageLayout::DepthReadOnlyOptimal,
             VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL)
        CASE(ImageLayout::DepthStencilAttachmentOptimal,
             VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        CASE(ImageLayout::DepthStencilReadOnlyOptimal,
             VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        CASE(ImageLayout::ShaderReadOnlyOptimal,
             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        CASE(ImageLayout::StencilReadOnlyOptimal,
             VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL)
        CASE(ImageLayout::DepthAttachmentStencilReadOnlyOptimal,
             VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
        CASE(ImageLayout::DepthReadOnlyStencilAttachmentOptimal,
             VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkPipelineBindPoint PipelineBindPoint2Vk(PipelineBindPoint bind) {
    switch (bind) {
        CASE(PipelineBindPoint::Compute, VK_PIPELINE_BIND_POINT_GRAPHICS)
        CASE(PipelineBindPoint::Graphics, VK_PIPELINE_BIND_POINT_GRAPHICS)
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkImageTiling ImageTiling2Vk(ImageTiling tiling) {
    switch (tiling) {
        CASE(ImageTiling::Linear, VK_IMAGE_TILING_LINEAR)
        CASE(ImageTiling::Optimal, VK_IMAGE_TILING_OPTIMAL)
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkSharingMode SharingMode2Vk(SharingMode mode) {
    switch (mode) {
        CASE(SharingMode::Concurrent, VK_SHARING_MODE_CONCURRENT)
        CASE(SharingMode::Exclusive, VK_SHARING_MODE_EXCLUSIVE)
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkComponentSwizzle ComponentMapping2Vk(ComponentMapping swizzle) {
    switch (swizzle) {
        CASE(ComponentMapping::SwizzleZero, VK_COMPONENT_SWIZZLE_ZERO)
        CASE(ComponentMapping::SwizzleOne, VK_COMPONENT_SWIZZLE_ONE)
        CASE(ComponentMapping::SwizzleIdentity, VK_COMPONENT_SWIZZLE_IDENTITY)
        CASE(ComponentMapping::SwizzleR, VK_COMPONENT_SWIZZLE_R)
        CASE(ComponentMapping::SwizzleG, VK_COMPONENT_SWIZZLE_G)
        CASE(ComponentMapping::SwizzleB, VK_COMPONENT_SWIZZLE_B)
        CASE(ComponentMapping::SwizzleA, VK_COMPONENT_SWIZZLE_A)
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkColorComponentFlags ColorComponent2Vk(Flags<ColorComponent> flags) {
    VkColorComponentFlags bits = 0;
    TRY_SET_BIT(ColorComponent::R, VK_COLOR_COMPONENT_R_BIT)
    TRY_SET_BIT(ColorComponent::G, VK_COLOR_COMPONENT_G_BIT)
    TRY_SET_BIT(ColorComponent::B, VK_COLOR_COMPONENT_B_BIT)
    TRY_SET_BIT(ColorComponent::A, VK_COLOR_COMPONENT_A_BIT)
    return bits;
}

inline VkColorSpaceKHR ImageColorSpace2Vk(ImageColorSpace colorSpace) {
    switch (colorSpace) {
        CASE(ImageColorSpace::DolbyvisionEXT, VK_COLOR_SPACE_DOLBYVISION_EXT)
        CASE(ImageColorSpace::AdobergbLinearEXT,
             VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT)
        CASE(ImageColorSpace::AdobergbNonlinearEXT,
             VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT)
        CASE(ImageColorSpace::Bt709LinearEXT, VK_COLOR_SPACE_BT709_LINEAR_EXT)
        CASE(ImageColorSpace::Bt709NonlinearEXT,
             VK_COLOR_SPACE_BT709_NONLINEAR_EXT)
        CASE(ImageColorSpace::Bt2020LinearEXT, VK_COLOR_SPACE_BT2020_LINEAR_EXT)
        CASE(ImageColorSpace::DisplayNativeAMD,
             VK_COLOR_SPACE_DISPLAY_NATIVE_AMD)
        CASE(ImageColorSpace::Hdr10HlgEXT, VK_COLOR_SPACE_HDR10_HLG_EXT)
        CASE(ImageColorSpace::Hdr10St2084EXT, VK_COLOR_SPACE_HDR10_ST2084_EXT)
        CASE(ImageColorSpace::PassThroughEXT, VK_COLOR_SPACE_PASS_THROUGH_EXT)
        CASE(ImageColorSpace::SrgbNonlinearKHR,
             VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        CASE(ImageColorSpace::DciP3NonlinearEXT,
             VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT)
        CASE(ImageColorSpace::DisplayP3LinearEXT,
             VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT)
        CASE(ImageColorSpace::DisplayP3NonlinearEXT,
             VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT)
        CASE(ImageColorSpace::ExtendedSrgbLinearEXT,
             VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT)
        CASE(ImageColorSpace::ExtendedSrgbNonlinearEXT,
             VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT)
    }

    NICKEL_CANT_REACH();
    return {};
}

inline ImageColorSpace VkColorSpace2ImageColorSpace(
    VkColorSpaceKHR colorSpace) {
    switch (colorSpace) {
        CASE(VK_COLOR_SPACE_DOLBYVISION_EXT, ImageColorSpace::DolbyvisionEXT)
        CASE(VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT,
             ImageColorSpace::AdobergbLinearEXT)
        CASE(VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT,
             ImageColorSpace::AdobergbNonlinearEXT)
        CASE(VK_COLOR_SPACE_BT709_LINEAR_EXT, ImageColorSpace::Bt709LinearEXT)
        CASE(VK_COLOR_SPACE_BT709_NONLINEAR_EXT,
             ImageColorSpace::Bt709NonlinearEXT)
        CASE(VK_COLOR_SPACE_BT2020_LINEAR_EXT, ImageColorSpace::Bt2020LinearEXT)
        CASE(VK_COLOR_SPACE_DISPLAY_NATIVE_AMD,
             ImageColorSpace::DisplayNativeAMD)
        CASE(VK_COLOR_SPACE_HDR10_HLG_EXT, ImageColorSpace::Hdr10HlgEXT)
        CASE(VK_COLOR_SPACE_HDR10_ST2084_EXT, ImageColorSpace::Hdr10St2084EXT)
        CASE(VK_COLOR_SPACE_PASS_THROUGH_EXT, ImageColorSpace::PassThroughEXT)
        CASE(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
             ImageColorSpace::SrgbNonlinearKHR)
        CASE(VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT,
             ImageColorSpace::DciP3NonlinearEXT)
        CASE(VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT,
             ImageColorSpace::DisplayP3LinearEXT)
        CASE(VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT,
             ImageColorSpace::DisplayP3NonlinearEXT)
        CASE(VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT,
             ImageColorSpace::ExtendedSrgbLinearEXT)
        CASE(VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT,
         ImageColorSpace::ExtendedSrgbNonlinearEXT)
        default:
            NICKEL_CANT_REACH();
    }

    NICKEL_CANT_REACH();
    return {};
}

inline VkMemoryPropertyFlags MemoryType2Vk(MemoryType type) {
    switch(type) {
        case MemoryType::CPULocal:
            return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        case MemoryType::Coherence:
            return VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        case MemoryType::GPULocal:
            return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }

    NICKEL_CANT_REACH();
    return {};
}

#undef CASE

}  // namespace nickel::graphics