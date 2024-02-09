#pragma once

#include "graphics/rhi/common.hpp"
#include "graphics/rhi/vk/pch.hpp"

namespace nickel::rhi::vulkan {

#define CASE(a, b) \
    case (a):      \
        return (b);

inline vk::BufferUsageFlags BufferUsage2Vk(BufferUsage usage) {
    uint32_t bits = 0;
    if (static_cast<uint32_t>(usage) |
        static_cast<uint32_t>(BufferUsage::MapRead)) {
        bits |= static_cast<uint32_t>(vk::BufferUsageFlagBits::eTransferSrc);
    }
    if (static_cast<uint32_t>(usage) |
        static_cast<uint32_t>(BufferUsage::CopySrc)) {
        bits |= static_cast<uint32_t>(vk::BufferUsageFlagBits::eTransferSrc);
    }
    if (static_cast<uint32_t>(usage) |
        static_cast<uint32_t>(BufferUsage::MapWrite)) {
        bits |= static_cast<uint32_t>(vk::BufferUsageFlagBits::eTransferDst);
    }
    if (static_cast<uint32_t>(usage) |
        static_cast<uint32_t>(BufferUsage::CopyDst)) {
        bits |= static_cast<uint32_t>(vk::BufferUsageFlagBits::eTransferDst);
    }
    if (static_cast<uint32_t>(usage) |
        static_cast<uint32_t>(BufferUsage::Vertex)) {
        bits |= static_cast<uint32_t>(vk::BufferUsageFlagBits::eVertexBuffer);
    }
    if (static_cast<uint32_t>(usage) |
        static_cast<uint32_t>(BufferUsage::Index)) {
        bits |= static_cast<uint32_t>(vk::BufferUsageFlagBits::eIndexBuffer);
    }
    if (static_cast<uint32_t>(usage) |
        static_cast<uint32_t>(BufferUsage::Uniform)) {
        bits |= static_cast<uint32_t>(vk::BufferUsageFlagBits::eUniformBuffer);
    }
    if (static_cast<uint32_t>(usage) |
        static_cast<uint32_t>(BufferUsage::Index)) {
        bits |= static_cast<uint32_t>(vk::BufferUsageFlagBits::eIndirectBuffer);
    }
    if (static_cast<uint32_t>(usage) |
        static_cast<uint32_t>(BufferUsage::Storage)) {
        bits |= static_cast<uint32_t>(vk::BufferUsageFlagBits::eStorageBuffer);
    }
    if (static_cast<uint32_t>(usage) |
        static_cast<uint32_t>(BufferUsage::QueryResolve)) {
        // TODO
    }
    return static_cast<vk::BufferUsageFlags>(bits);
}

inline vk::Filter Filter2Vk(Filter filter) {
    switch (filter) {
        case Filter::Nearest:
            return vk::Filter::eNearest;
        case Filter::Linear:
            return vk::Filter::eLinear;
    }
}

inline vk::SamplerAddressMode SamplerAddressMode2Vk(SamplerAddressMode mode) {
    switch (mode) {
        case SamplerAddressMode::ClampToEdge:
            return vk::SamplerAddressMode::eClampToEdge;
        case SamplerAddressMode::Repeat:
            return vk::SamplerAddressMode::eRepeat;
        case SamplerAddressMode::MirrorRepeat:
            return vk::SamplerAddressMode::eMirroredRepeat;
    }
}

inline vk::CompareOp CompareOp2Vk(CompareOp op) {
    vk::ImageCreateInfo info;
    switch (op) {
        case CompareOp::Never:
            return vk::CompareOp::eNever;
        case CompareOp::Less:
            return vk::CompareOp::eLess;
        case CompareOp::Equal:
            return vk::CompareOp::eEqual;
        case CompareOp::LessEqual:
            return vk::CompareOp::eLessOrEqual;
        case CompareOp::Greater:
            return vk::CompareOp::eGreater;
        case CompareOp::NotEqual:
            return vk::CompareOp::eNotEqual;
        case CompareOp::GreaterEqual:
            return vk::CompareOp::eGreaterOrEqual;
        case CompareOp::Always:
            return vk::CompareOp::eAlways;
            break;
    }
}

inline vk::ImageType TextureType2Vk(TextureType type) {
    switch (type) {
        CASE(TextureType::Dim1, vk::ImageType::e1D);
        CASE(TextureType::Dim2, vk::ImageType::e2D);
        CASE(TextureType::Dim3, vk::ImageType::e3D);
    }
}

inline vk::ImageUsageFlags TextureUsage2Vk(TextureUsage usage) {
    uint32_t flags = 0;
    switch (usage) {
        case TextureUsage::CopySrc:
            flags |=
                static_cast<uint32_t>(vk::ImageUsageFlagBits::eTransferSrc);
            break;
        case TextureUsage::CopyDst:
            flags |=
                static_cast<uint32_t>(vk::ImageUsageFlagBits::eTransferDst);
            break;
        case TextureUsage::TextureBinding:
            flags |= static_cast<uint32_t>(vk::ImageUsageFlagBits::eSampled);
            break;
        case TextureUsage::StorageBinding:
            flags |= static_cast<uint32_t>(vk::ImageUsageFlagBits::eStorage);
            break;
        case TextureUsage::RenderAttachment:
            flags |= static_cast<uint32_t>(
                vk::ImageUsageFlagBits::eColorAttachment |
                vk::ImageUsageFlagBits::eDepthStencilAttachment);
            break;
    }
    return static_cast<vk::ImageUsageFlags>(flags);
}

inline vk::SampleCountFlagBits SampleCount2Vk(SampleCount count) {
    switch (count) {
        CASE(SampleCount::Count1, vk::SampleCountFlagBits::e1);
        CASE(SampleCount::Count2, vk::SampleCountFlagBits::e2);
        CASE(SampleCount::Count4, vk::SampleCountFlagBits::e4);
        CASE(SampleCount::Count8, vk::SampleCountFlagBits::e8);
        CASE(SampleCount::Count16, vk::SampleCountFlagBits::e16);
        CASE(SampleCount::Count32, vk::SampleCountFlagBits::e32);
        CASE(SampleCount::Count64, vk::SampleCountFlagBits::e64);
    }
}

inline vk::Format Format2Vk(Format f) {
    switch (f) {
        CASE(Format::R8_UNORM, vk::Format::eR8Unorm);
        CASE(Format::R8_SNORM, vk::Format::eR8Snorm);
        CASE(Format::R8_UINT, vk::Format::eR8Uint);
        CASE(Format::R8_SINT, vk::Format::eR8Sint);

        CASE(Format::R16_UINT, vk::Format::eR16Uint);
        CASE(Format::R16_SINT, vk::Format::eR16Sint);
        CASE(Format::R16_FLOAT, vk::Format::eR16Sfloat);
        CASE(Format::RG8_UNORM, vk::Format::eR8G8Unorm);
        CASE(Format::RG8_SNORM, vk::Format::eR8G8Snorm);
        CASE(Format::RG8_UINT, vk::Format::eR8G8Uint);
        CASE(Format::RG8_SINT, vk::Format::eR8G8Sint);

        CASE(Format::R32_UINT, vk::Format::eR32Uint);
        CASE(Format::R32_SINT, vk::Format::eR32Sint);
        CASE(Format::R32_FLOAT, vk::Format::eR32Sfloat);
        CASE(Format::RG16_UINT, vk::Format::eR16G16Uint);
        CASE(Format::RG16_SINT, vk::Format::eR16G16Sint);
        CASE(Format::RG16_FLOAT, vk::Format::eR16G16Sfloat);
        CASE(Format::RGBA8_UNORM, vk::Format::eR8G8B8A8Unorm);
        CASE(Format::RGBA8_UNORM_SRGB, vk::Format::eR8G8B8A8Srgb);
        CASE(Format::RGBA8_SNORM, vk::Format::eR8G8B8A8Snorm);
        CASE(Format::RGBA8_UINT, vk::Format::eR8G8B8A8Uint);
        CASE(Format::RGBA8_SINT, vk::Format::eR8G8B8A8Sint);
        CASE(Format::BGRA8_UNORM, vk::Format::eB8G8R8A8Unorm);
        CASE(Format::BGRA8_UNORM_SRGB, vk::Format::eB8G8R8A8Srgb);

        // NOTE: these packed format's order is reversed. Do we need reverse
        // order when create image?
        CASE(Format::RGB9E5_UFLOAT, vk::Format::eE5B9G9R9UfloatPack32);
        CASE(Format::RGB10A2_UINT, vk::Format::eA2B10G10R10UintPack32);
        CASE(Format::RGB10A2_UNORM, vk::Format::eA2B10G10R10UnormPack32);
        CASE(Format::RG11B10_UFLOAT, vk::Format::eB10G11R11UfloatPack32);

        CASE(Format::RG32_UINT, vk::Format::eR32G32Uint);
        CASE(Format::RG32_SINT, vk::Format::eR32G32Sint);
        CASE(Format::RG32_FLOAT, vk::Format::eR32G32Sfloat);
        CASE(Format::RGBA16_UINT, vk::Format::eR16G16B16A16Uint);
        CASE(Format::RGBA16_SINT, vk::Format::eR16G16B16A16Sint);
        CASE(Format::RGBA16_FLOAT, vk::Format::eR16G16B16A16Sfloat);

        CASE(Format::RGBA32_UINT, vk::Format::eR32G32B32A32Uint);
        CASE(Format::RGBA32_SINT, vk::Format::eR32G32B32A32Sint);
        CASE(Format::RGBA32_FLOAT, vk::Format::eR32G32B32A32Sfloat);

        CASE(Format::STENCIL8, vk::Format::eS8Uint);
        CASE(Format::DEPTH16_UNORM, vk::Format::eD16Unorm);
        CASE(Format::DEPTH24_PLUS, vk::Format::eD24UnormS8Uint);  // doubt
        CASE(Format::DEPTH24_PLUS_STENCIL8,
             vk::Format::eX8D24UnormPack32);  // doubt
        CASE(Format::DEPTH32_FLOAT, vk::Format::eD32Sfloat);

        CASE(Format::DEPTH32_FLOAT_STENCIL8, vk::Format::eD32SfloatS8Uint);

        CASE(Format::BC1_RGBA_UNORM, vk::Format::eBc1RgbaUnormBlock);
        CASE(Format::BC1_RGBA_UNORM_SRGB, vk::Format::eBc1RgbaSrgbBlock);
        CASE(Format::BC2_RGBA_UNORM, vk::Format::eBc2UnormBlock);
        CASE(Format::BC2_RGBA_UNORM_SRGB, vk::Format::eBc2SrgbBlock);
        CASE(Format::BC3_RGBA_UNORM, vk::Format::eBc3UnormBlock);
        CASE(Format::BC3_RGBA_UNORM_SRGB, vk::Format::eBc3SrgbBlock);
        CASE(Format::BC4_R_UNORM, vk::Format::eBc4UnormBlock);
        CASE(Format::BC4_R_SNORM, vk::Format::eBc4SnormBlock);
        CASE(Format::BC5_RG_UNORM, vk::Format::eBc5UnormBlock);
        CASE(Format::BC5_RG_SNORM, vk::Format::eBc5SnormBlock);
        CASE(Format::BC6H_RGB_UFLOAT, vk::Format::eBc6HUfloatBlock);
        CASE(Format::BC6H_RGB_FLOAT, vk::Format::eBc6HSfloatBlock);
        CASE(Format::BC7_RGBA_UNORM, vk::Format::eBc7UnormBlock);
        CASE(Format::BC7_RGBA_UNORM_SRGB, vk::Format::eBc7SrgbBlock);

        CASE(Format::ETC2_RGB8_UNORM, vk::Format::eEtc2R8G8B8UnormBlock);
        CASE(Format::ETC2_RGB8_UNORM_SRGB, vk::Format::eEtc2R8G8B8SrgbBlock);
        CASE(Format::ETC2_RGB8A1_UNORM, vk::Format::eEtc2R8G8B8A1UnormBlock);
        CASE(Format::ETC2_RGB8A1_UNORM_SRGB,
             vk::Format::eEtc2R8G8B8A1SrgbBlock);
        CASE(Format::ETC2_RGBA8_UNORM, vk::Format::eEtc2R8G8B8A8UnormBlock);
        CASE(Format::ETC2_RGBA8_UNORM_SRGB, vk::Format::eEtc2R8G8B8A8SrgbBlock);
        CASE(Format::EAC_R11_UNORM, vk::Format::eEacR11UnormBlock);
        CASE(Format::EAC_R11_SNORM, vk::Format::eEacR11SnormBlock);
        CASE(Format::EAC_RG11_UNORM, vk::Format::eEacR11G11UnormBlock);
        CASE(Format::EAC_RG11_SNORM, vk::Format::eEacR11G11SnormBlock);

        CASE(Format::ASTC_4X4_UNORM, vk::Format::eAstc4x4UnormBlock);
        CASE(Format::ASTC_4X4_UNORM_SRGB, vk::Format::eAstc4x4SrgbBlock);
        CASE(Format::ASTC_5X4_UNORM, vk::Format::eAstc5x4UnormBlock);
        CASE(Format::ASTC_5X4_UNORM_SRGB, vk::Format::eAstc5x4SrgbBlock);
        CASE(Format::ASTC_5X5_UNORM, vk::Format::eAstc5x5UnormBlock);
        CASE(Format::ASTC_5X5_UNORM_SRGB, vk::Format::eAstc5x5SrgbBlock);
        CASE(Format::ASTC_6X5_UNORM, vk::Format::eAstc6x5UnormBlock);
        CASE(Format::ASTC_6X5_UNORM_SRGB, vk::Format::eAstc6x5SrgbBlock);
        CASE(Format::ASTC_6X6_UNORM, vk::Format::eAstc6x6UnormBlock);
        CASE(Format::ASTC_6X6_UNORM_SRGB, vk::Format::eAstc6x6SrgbBlock);
        CASE(Format::ASTC_8X5_UNORM, vk::Format::eAstc8x5UnormBlock);
        CASE(Format::ASTC_8X5_UNORM_SRGB, vk::Format::eAstc8x5SrgbBlock);

        CASE(Format::ASTC_8X6_UNORM, vk::Format::eAstc8x6UnormBlock);
        CASE(Format::ASTC_8X6_UNORM_SRGB, vk::Format::eAstc8x6SrgbBlock);
        CASE(Format::ASTC_8X8_UNORM, vk::Format::eAstc8x8UnormBlock);
        CASE(Format::ASTC_8X8_UNORM_SRGB, vk::Format::eAstc8x8SrgbBlock);
        CASE(Format::ASTC_10X5_UNORM, vk::Format::eAstc10x5UnormBlock);
        CASE(Format::ASTC_10X5_UNORM_SRGB, vk::Format::eAstc10x5SrgbBlock);
        CASE(Format::ASTC_10X6_UNORM, vk::Format::eAstc10x6UnormBlock);
        CASE(Format::ASTC_10X6_UNORM_SRGB, vk::Format::eAstc10x6SrgbBlock);
        CASE(Format::ASTC_10X8_UNORM, vk::Format::eAstc10x8UnormBlock);
        CASE(Format::ASTC_10X8_UNORM_SRGB, vk::Format::eAstc10x8SrgbBlock);
        CASE(Format::ASTC_10X10_UNORM, vk::Format::eAstc10x10UnormBlock);
        CASE(Format::ASTC_10X10_UNORM_SRGB, vk::Format::eAstc10x10SrgbBlock);
        CASE(Format::ASTC_12X10_UNORM, vk::Format::eAstc12x10UnormBlock);
        CASE(Format::ASTC_12X10_UNORM_SRGB, vk::Format::eAstc12x10SrgbBlock);
        CASE(Format::ASTC_12X12_UNORM, vk::Format::eAstc12x12UnormBlock);
        CASE(Format::ASTC_12X12_UNORM_SRGB, vk::Format::eAstc12x12SrgbBlock);
        default:
            return vk::Format::eUndefined;
    }
}

inline Format FormatFromVk(vk::Format f) {
    switch (f) {
        CASE(vk::Format::eR8Unorm, Format::R8_UNORM);
        CASE(vk::Format::eR8Snorm, Format::R8_SNORM);
        CASE(vk::Format::eR8Uint, Format::R8_UINT);
        CASE(vk::Format::eR8Sint, Format::R8_SINT);

        CASE(vk::Format::eR16Uint, Format::R16_UINT);
        CASE(vk::Format::eR16Sint, Format::R16_SINT);
        CASE(vk::Format::eR16Sfloat, Format::R16_FLOAT);
        CASE(vk::Format::eR8G8Unorm, Format::RG8_UNORM);
        CASE(vk::Format::eR8G8Snorm, Format::RG8_SNORM);
        CASE(vk::Format::eR8G8Uint, Format::RG8_UINT);
        CASE(vk::Format::eR8G8Sint, Format::RG8_SINT);

        CASE(vk::Format::eR32Uint, Format::R32_UINT);
        CASE(vk::Format::eR32Sint, Format::R32_SINT);
        CASE(vk::Format::eR32Sfloat, Format::R32_FLOAT);
        CASE(vk::Format::eR16G16Uint, Format::RG16_UINT);
        CASE(vk::Format::eR16G16Sint, Format::RG16_SINT);
        CASE(vk::Format::eR16G16Sfloat, Format::RG16_FLOAT);
        CASE(vk::Format::eR8G8B8A8Unorm, Format::RGBA8_UNORM);
        CASE(vk::Format::eR8G8B8A8Srgb, Format::RGBA8_UNORM_SRGB);
        CASE(vk::Format::eR8G8B8A8Snorm, Format::RGBA8_SNORM);
        CASE(vk::Format::eR8G8B8A8Uint, Format::RGBA8_UINT);
        CASE(vk::Format::eR8G8B8A8Sint, Format::RGBA8_SINT);
        CASE(vk::Format::eB8G8R8A8Unorm, Format::BGRA8_UNORM);
        CASE(vk::Format::eB8G8R8A8Srgb, Format::BGRA8_UNORM_SRGB);

        // NOTE: these packed format's order is reversed. Do we need reverse
        // order when create image?
        CASE(vk::Format::eE5B9G9R9UfloatPack32, Format::RGB9E5_UFLOAT);
        CASE(vk::Format::eA2B10G10R10UintPack32, Format::RGB10A2_UINT);
        CASE(vk::Format::eA2B10G10R10UnormPack32, Format::RGB10A2_UNORM);
        CASE(vk::Format::eB10G11R11UfloatPack32, Format::RG11B10_UFLOAT);

        CASE(vk::Format::eR32G32Uint, Format::RG32_UINT);
        CASE(vk::Format::eR32G32Sint, Format::RG32_SINT);
        CASE(vk::Format::eR32G32Sfloat, Format::RG32_FLOAT);
        CASE(vk::Format::eR16G16B16A16Uint, Format::RGBA16_UINT);
        CASE(vk::Format::eR16G16B16A16Sint, Format::RGBA16_SINT);
        CASE(vk::Format::eR16G16B16A16Sfloat, Format::RGBA16_FLOAT);

        CASE(vk::Format::eR32G32B32A32Uint, Format::RGBA32_UINT);
        CASE(vk::Format::eR32G32B32A32Sint, Format::RGBA32_SINT);
        CASE(vk::Format::eR32G32B32A32Sfloat, Format::RGBA32_FLOAT);

        CASE(vk::Format::eS8Uint, Format::STENCIL8);
        CASE(vk::Format::eD16Unorm, Format::DEPTH16_UNORM);
        CASE(vk::Format::eD24UnormS8Uint, Format::DEPTH24_PLUS);  // doubt
        CASE(vk::Format::eX8D24UnormPack32,
             Format::DEPTH24_PLUS_STENCIL8);  // doubt
        CASE(vk::Format::eD32Sfloat, Format::DEPTH32_FLOAT);

        CASE(vk::Format::eD32SfloatS8Uint, Format::DEPTH32_FLOAT_STENCIL8);

        CASE(vk::Format::eBc1RgbaUnormBlock, Format::BC1_RGBA_UNORM);
        CASE(vk::Format::eBc1RgbaSrgbBlock, Format::BC1_RGBA_UNORM_SRGB);
        CASE(vk::Format::eBc2UnormBlock, Format::BC2_RGBA_UNORM);
        CASE(vk::Format::eBc2SrgbBlock, Format::BC2_RGBA_UNORM_SRGB);
        CASE(vk::Format::eBc3UnormBlock, Format::BC3_RGBA_UNORM);
        CASE(vk::Format::eBc3SrgbBlock, Format::BC3_RGBA_UNORM_SRGB);
        CASE(vk::Format::eBc4UnormBlock, Format::BC4_R_UNORM);
        CASE(vk::Format::eBc4SnormBlock, Format::BC4_R_SNORM);
        CASE(vk::Format::eBc5UnormBlock, Format::BC5_RG_UNORM);
        CASE(vk::Format::eBc5SnormBlock, Format::BC5_RG_SNORM);
        CASE(vk::Format::eBc6HUfloatBlock, Format::BC6H_RGB_UFLOAT);
        CASE(vk::Format::eBc6HSfloatBlock, Format::BC6H_RGB_FLOAT);
        CASE(vk::Format::eBc7UnormBlock, Format::BC7_RGBA_UNORM);
        CASE(vk::Format::eBc7SrgbBlock, Format::BC7_RGBA_UNORM_SRGB);

        CASE(vk::Format::eEtc2R8G8B8UnormBlock, Format::ETC2_RGB8_UNORM);
        CASE(vk::Format::eEtc2R8G8B8SrgbBlock, Format::ETC2_RGB8_UNORM_SRGB);
        CASE(vk::Format::eEtc2R8G8B8A1UnormBlock, Format::ETC2_RGB8A1_UNORM);
        CASE(vk::Format::eEtc2R8G8B8A1SrgbBlock,
             Format::ETC2_RGB8A1_UNORM_SRGB);
        CASE(vk::Format::eEtc2R8G8B8A8UnormBlock, Format::ETC2_RGBA8_UNORM);
        CASE(vk::Format::eEtc2R8G8B8A8SrgbBlock, Format::ETC2_RGBA8_UNORM_SRGB);
        CASE(vk::Format::eEacR11UnormBlock, Format::EAC_R11_UNORM);
        CASE(vk::Format::eEacR11SnormBlock, Format::EAC_R11_SNORM);
        CASE(vk::Format::eEacR11G11UnormBlock, Format::EAC_RG11_UNORM);
        CASE(vk::Format::eEacR11G11SnormBlock, Format::EAC_RG11_SNORM);

        CASE(vk::Format::eAstc4x4UnormBlock, Format::ASTC_4X4_UNORM);
        CASE(vk::Format::eAstc4x4SrgbBlock, Format::ASTC_4X4_UNORM_SRGB);
        CASE(vk::Format::eAstc5x4UnormBlock, Format::ASTC_5X4_UNORM);
        CASE(vk::Format::eAstc5x4SrgbBlock, Format::ASTC_5X4_UNORM_SRGB);
        CASE(vk::Format::eAstc5x5UnormBlock, Format::ASTC_5X5_UNORM);
        CASE(vk::Format::eAstc5x5SrgbBlock, Format::ASTC_5X5_UNORM_SRGB);
        CASE(vk::Format::eAstc6x5UnormBlock, Format::ASTC_6X5_UNORM);
        CASE(vk::Format::eAstc6x5SrgbBlock, Format::ASTC_6X5_UNORM_SRGB);
        CASE(vk::Format::eAstc6x6UnormBlock, Format::ASTC_6X6_UNORM);
        CASE(vk::Format::eAstc6x6SrgbBlock, Format::ASTC_6X6_UNORM_SRGB);
        CASE(vk::Format::eAstc8x5UnormBlock, Format::ASTC_8X5_UNORM);
        CASE(vk::Format::eAstc8x5SrgbBlock, Format::ASTC_8X5_UNORM_SRGB);

        CASE(vk::Format::eAstc8x6UnormBlock, Format::ASTC_8X6_UNORM);
        CASE(vk::Format::eAstc8x6SrgbBlock, Format::ASTC_8X6_UNORM_SRGB);
        CASE(vk::Format::eAstc8x8UnormBlock, Format::ASTC_8X8_UNORM);
        CASE(vk::Format::eAstc8x8SrgbBlock, Format::ASTC_8X8_UNORM_SRGB);
        CASE(vk::Format::eAstc10x5UnormBlock, Format::ASTC_10X5_UNORM);
        CASE(vk::Format::eAstc10x5SrgbBlock, Format::ASTC_10X5_UNORM_SRGB);
        CASE(vk::Format::eAstc10x6UnormBlock, Format::ASTC_10X6_UNORM);
        CASE(vk::Format::eAstc10x6SrgbBlock, Format::ASTC_10X6_UNORM_SRGB);
        CASE(vk::Format::eAstc10x8UnormBlock, Format::ASTC_10X8_UNORM);
        CASE(vk::Format::eAstc10x8SrgbBlock, Format::ASTC_10X8_UNORM_SRGB);
        CASE(vk::Format::eAstc10x10UnormBlock, Format::ASTC_10X10_UNORM);
        CASE(vk::Format::eAstc10x10SrgbBlock, Format::ASTC_10X10_UNORM_SRGB);
        CASE(vk::Format::eAstc12x10UnormBlock, Format::ASTC_12X10_UNORM);
        CASE(vk::Format::eAstc12x10SrgbBlock, Format::ASTC_12X10_UNORM_SRGB);
        CASE(vk::Format::eAstc12x12UnormBlock, Format::ASTC_12X12_UNORM);
        CASE(vk::Format::eAstc12x12SrgbBlock, Format::ASTC_12X12_UNORM_SRGB);
        default:
            return Format::Undefined;
    }
}

inline vk::ImageAspectFlags TextureAspect2Vk(TextureAspect aspect) {
    switch (aspect) {
        CASE(TextureAspect::All, vk::ImageAspectFlagBits::eColor |
                                     vk::ImageAspectFlagBits::eDepth |
                                     vk::ImageAspectFlagBits::eStencil);
        CASE(TextureAspect::DepthOnly, vk::ImageAspectFlagBits::eDepth);
        CASE(TextureAspect::StencilOnly, vk::ImageAspectFlagBits::eStencil);
    }
}

inline vk::ImageViewType TextureViewType2Vk(TextureViewType type) {
    switch (type) {
        CASE(TextureViewType::Dim1, vk::ImageViewType::e1D);
        CASE(TextureViewType::Dim2, vk::ImageViewType::e2D);
        CASE(TextureViewType::Dim3, vk::ImageViewType::e3D);
        CASE(TextureViewType::Dim2Array, vk::ImageViewType::e2DArray);
        CASE(TextureViewType::Cube, vk::ImageViewType::eCube);
        CASE(TextureViewType::CubeArray, vk::ImageViewType::eCubeArray);
    }
}

inline vk::ShaderStageFlags ShaderStage2Vk(ShaderStage stage) {
    uint32_t result = 0;
    uint32_t numeric = static_cast<uint32_t>(stage);
    if (numeric & static_cast<uint32_t>(ShaderStage::Vertex)) {
        result |= static_cast<uint32_t>(vk::ShaderStageFlagBits::eVertex);
    }
    if (numeric & static_cast<uint32_t>(ShaderStage::Fragment)) {
        result |= static_cast<uint32_t>(vk::ShaderStageFlagBits::eFragment);
    }
    if (numeric & static_cast<uint32_t>(ShaderStage::Compute)) {
        result |= static_cast<uint32_t>(vk::ShaderStageFlagBits::eCompute);
    }
    return static_cast<vk::ShaderStageFlags>(result);
}

inline vk::PrimitiveTopology Topology2Vk(Topology t) {
    switch (t) {
        CASE(Topology::LineList, vk::PrimitiveTopology::eLineList);
        CASE(Topology::LineStrip, vk::PrimitiveTopology::eLineStrip);
        CASE(Topology::PointList, vk::PrimitiveTopology::ePointList);
        CASE(Topology::TriangleList, vk::PrimitiveTopology::eTriangleList);
        CASE(Topology::TriangleStrip, vk::PrimitiveTopology::eTriangleStrip);
        CASE(Topology::TriangleFan, vk::PrimitiveTopology::eTriangleFan);
    }
}

inline vk::CullModeFlagBits CullMode2Vk(CullMode mode) {
    if (mode == CullMode::None) {
        return vk::CullModeFlagBits::eNone;
    }
    if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(CullMode::Front)) {
        return vk::CullModeFlagBits::eFront;
    }
    if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(CullMode::Back)) {
        return vk::CullModeFlagBits::eBack;
    }
    if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(CullMode::Back) &&
        static_cast<uint32_t>(mode) & static_cast<uint32_t>(CullMode::Front)) {
        return vk::CullModeFlagBits::eFrontAndBack;
    }

    return vk::CullModeFlagBits::eNone;
}

inline vk::PolygonMode PolygonMode2Vk(PolygonMode mode) {
    switch (mode) {
        CASE(PolygonMode::Line, vk::PolygonMode::eLine);
        CASE(PolygonMode::Fill, vk::PolygonMode::eFill);
        CASE(PolygonMode::Point, vk::PolygonMode::ePoint);
    }
}

inline vk::FrontFace FrontFace2Vk(FrontFace face) {
    switch (face) {
        CASE(FrontFace::CCW, vk::FrontFace::eCounterClockwise);
        CASE(FrontFace::CW, vk::FrontFace::eClockwise);
    }
}

inline vk::BlendOp BlendOp2Vk(BlendOp op) {
    switch (op) {
        CASE(BlendOp::Add, vk::BlendOp::eAdd);
        CASE(BlendOp::Subtract, vk::BlendOp::eSubtract);
        CASE(BlendOp::ReverseSubtract, vk::BlendOp::eReverseSubtract);
        CASE(BlendOp::Min, vk::BlendOp::eMin);
        CASE(BlendOp::Max, vk::BlendOp::eMax);
    }
}

inline vk::BlendFactor BlendFactor2Vk(BlendFactor factor) {
    switch (factor) {
        CASE(BlendFactor::eZero, vk::BlendFactor::eZero);
        CASE(BlendFactor::eOne, vk::BlendFactor::eOne);
        CASE(BlendFactor::eSrcColor, vk::BlendFactor::eSrcColor);
        CASE(BlendFactor::eOneMinusSrcColor,
             vk::BlendFactor::eOneMinusSrcColor);
        CASE(BlendFactor::eDstColor, vk::BlendFactor::eDstColor);
        CASE(BlendFactor::eOneMinusDstColor,
             vk::BlendFactor::eOneMinusDstColor);
        CASE(BlendFactor::eSrcAlpha, vk::BlendFactor::eSrcAlpha);
        CASE(BlendFactor::eOneMinusSrcAlpha,
             vk::BlendFactor::eOneMinusSrcAlpha);
        CASE(BlendFactor::eDstAlpha, vk::BlendFactor::eDstAlpha);
        CASE(BlendFactor::eOneMinusDstAlpha,
             vk::BlendFactor::eOneMinusDstAlpha);
        CASE(BlendFactor::eConstantColor, vk::BlendFactor::eConstantColor);
        CASE(BlendFactor::eOneMinusConstantColor,
             vk::BlendFactor::eOneMinusConstantColor);
        CASE(BlendFactor::eConstantAlpha, vk::BlendFactor::eConstantAlpha);
        CASE(BlendFactor::eOneMinusConstantAlpha,
             vk::BlendFactor::eOneMinusConstantAlpha);
        CASE(BlendFactor::eSrcAlphaSaturate,
             vk::BlendFactor::eSrcAlphaSaturate);
    }
}

inline vk::StencilOp StencilOp2Vk(StencilOp op) {
    switch (op) {
        CASE(StencilOp::Keep, vk::StencilOp::eKeep);
        CASE(StencilOp::Zero, vk::StencilOp::eZero);
        CASE(StencilOp::Replace, vk::StencilOp::eReplace);
        CASE(StencilOp::IncrementAndClamp, vk::StencilOp::eIncrementAndClamp);
        CASE(StencilOp::DecrementAndClamp, vk::StencilOp::eDecrementAndClamp);
        CASE(StencilOp::Invert, vk::StencilOp::eInvert);
        CASE(StencilOp::IncrementAndWrap, vk::StencilOp::eIncrementAndWrap);
        CASE(StencilOp::DecrementAndWrap, vk::StencilOp::eDecrementAndWrap);
    }
}

inline vk::AttachmentLoadOp AttachmentLoadOp2Vk(AttachmentLoadOp op) {
    switch (op) {
        CASE(AttachmentLoadOp::Clear, vk::AttachmentLoadOp::eClear);
        CASE(AttachmentLoadOp::Load, vk::AttachmentLoadOp::eLoad);
    }
}

inline vk::AttachmentStoreOp AttachmentStoreOp2Vk(AttachmentStoreOp op) {
    switch (op) {
        CASE(AttachmentStoreOp::Store, vk::AttachmentStoreOp::eStore);
        CASE(AttachmentStoreOp::Discard, vk::AttachmentStoreOp::eDontCare);
    }
}

inline vk::IndexType IndexType2Vk(IndexType type) {
    switch (type) {
        CASE(IndexType::Uint16, vk::IndexType::eUint16);
        CASE(IndexType::Uint32, vk::IndexType::eUint32);
    }
}

#undef CASE

inline vk::Format GetCompatibleFormat(vk::Format fmt) {
    if (fmt == vk::Format::eR4G4UnormPack8 || fmt == vk::Format::eR8Unorm ||
        fmt == vk::Format::eR8Snorm || fmt == vk::Format::eR8Uscaled ||
        fmt == vk::Format::eR8Sscaled || fmt == vk::Format::eR8Uint ||
        fmt == vk::Format::eR8Sint || fmt == vk::Format::eR8Srgb) {
        return vk::Format::eR8Srgb;
    }

    if (fmt == vk::Format::eA1B5G5R5UnormPack16KHR ||
        fmt == vk::Format::eR10X6UnormPack16 ||
        fmt == vk::Format::eR12X4UnormPack16 ||
        fmt == vk::Format::eA4R4G4B4UnormPack16 ||
        fmt == vk::Format::eA4B4G4R4UnormPack16 ||
        fmt == vk::Format::eR4G4B4A4UnormPack16 ||
        fmt == vk::Format::eB4G4R4A4UnormPack16 ||
        fmt == vk::Format::eR5G6B5UnormPack16 ||
        fmt == vk::Format::eB5G6R5UnormPack16 ||
        fmt == vk::Format::eR5G5B5A1UnormPack16 ||
        fmt == vk::Format::eB5G5R5A1UnormPack16 ||
        fmt == vk::Format::eA1R5G5B5UnormPack16 ||
        fmt == vk::Format::eR8G8Unorm || fmt == vk::Format::eR8G8Snorm ||
        fmt == vk::Format::eR8G8Uscaled || fmt == vk::Format::eR8G8Sscaled ||
        fmt == vk::Format::eR8G8Uint || fmt == vk::Format::eR8G8Sint ||
        fmt == vk::Format::eR8G8Srgb || fmt == vk::Format::eR16Unorm ||
        fmt == vk::Format::eR16Snorm || fmt == vk::Format::eR16Uscaled ||
        fmt == vk::Format::eR16Sscaled || fmt == vk::Format::eR16Uint ||
        fmt == vk::Format::eR16Sint || fmt == vk::Format::eR16Sfloat) {
        return vk::Format::eR16Sfloat;
    }

    if (fmt == vk::Format::eR8G8B8Unorm || fmt == vk::Format::eR8G8B8Snorm ||
        fmt == vk::Format::eR8G8B8Uscaled ||
        fmt == vk::Format::eR8G8B8Sscaled || fmt == vk::Format::eR8G8B8Uint ||
        fmt == vk::Format::eR8G8B8Sint || fmt == vk::Format::eR8G8B8Srgb ||
        fmt == vk::Format::eB8G8R8Unorm || fmt == vk::Format::eB8G8R8Snorm ||
        fmt == vk::Format::eB8G8R8Uscaled ||
        fmt == vk::Format::eB8G8R8Sscaled || fmt == vk::Format::eB8G8R8Uint ||
        fmt == vk::Format::eB8G8R8Sint || fmt == vk::Format::eB8G8R8Srgb) {
        return vk::Format::eB8G8R8Srgb;
    }

    if (fmt == vk::Format::eR10X6G10X6Unorm2Pack16 ||
        fmt == vk::Format::eR12X4G12X4Unorm2Pack16 ||
        fmt == vk::Format::eR16G16S105NV || fmt == vk::Format::eR8G8B8A8Unorm ||
        fmt == vk::Format::eR8G8B8A8Snorm ||
        fmt == vk::Format::eR8G8B8A8Uscaled ||
        fmt == vk::Format::eR8G8B8A8Sscaled ||
        fmt == vk::Format::eR8G8B8A8Uint || fmt == vk::Format::eR8G8B8A8Sint ||
        fmt == vk::Format::eR8G8B8A8Srgb || fmt == vk::Format::eB8G8R8A8Unorm ||
        fmt == vk::Format::eB8G8R8A8Snorm ||
        fmt == vk::Format::eB8G8R8A8Uscaled ||
        fmt == vk::Format::eB8G8R8A8Sscaled ||
        fmt == vk::Format::eB8G8R8A8Uint || fmt == vk::Format::eB8G8R8A8Sint ||
        fmt == vk::Format::eB8G8R8A8Srgb ||
        fmt == vk::Format::eA8B8G8R8UnormPack32 ||
        fmt == vk::Format::eA8B8G8R8SnormPack32 ||
        fmt == vk::Format::eA8B8G8R8UscaledPack32 ||
        fmt == vk::Format::eA8B8G8R8SscaledPack32 ||
        fmt == vk::Format::eA8B8G8R8UintPack32 ||
        fmt == vk::Format::eA8B8G8R8SintPack32 ||
        fmt == vk::Format::eA8B8G8R8SrgbPack32 ||
        fmt == vk::Format::eA2R10G10B10UnormPack32 ||
        fmt == vk::Format::eA2R10G10B10SnormPack32 ||
        fmt == vk::Format::eA2R10G10B10UscaledPack32 ||
        fmt == vk::Format::eA2R10G10B10SscaledPack32 ||
        fmt == vk::Format::eA2R10G10B10UintPack32 ||
        fmt == vk::Format::eA2R10G10B10SintPack32 ||
        fmt == vk::Format::eA2B10G10R10UnormPack32 ||
        fmt == vk::Format::eA2B10G10R10SnormPack32 ||
        fmt == vk::Format::eA2B10G10R10UscaledPack32 ||
        fmt == vk::Format::eA2B10G10R10SscaledPack32 ||
        fmt == vk::Format::eA2B10G10R10UintPack32 ||
        fmt == vk::Format::eA2B10G10R10SintPack32 ||
        fmt == vk::Format::eR16G16Unorm || fmt == vk::Format::eR16G16Snorm ||
        fmt == vk::Format::eR16G16Uscaled ||
        fmt == vk::Format::eR16G16Sscaled || fmt == vk::Format::eR16G16Uint ||
        fmt == vk::Format::eR16G16Sint || fmt == vk::Format::eR16G16Sfloat ||
        fmt == vk::Format::eR32Uint || fmt == vk::Format::eR32Sint ||
        fmt == vk::Format::eR32Sfloat ||
        fmt == vk::Format::eB10G11R11UfloatPack32 ||
        fmt == vk::Format::eE5B9G9R9UfloatPack32) {
        return vk::Format::eR32Sfloat;
    }

    if (fmt == vk::Format::eR16G16B16Unorm ||
        fmt == vk::Format::eR16G16B16Snorm ||
        fmt == vk::Format::eR16G16B16Uscaled ||
        fmt == vk::Format::eR16G16B16Sscaled ||
        fmt == vk::Format::eR16G16B16Uint ||
        fmt == vk::Format::eR16G16B16Sint ||
        fmt == vk::Format::eR16G16B16Sfloat) {
        return vk::Format::eR16G16B16Sfloat;
    }

    if (fmt == vk::Format::eR16G16B16A16Unorm ||
        fmt == vk::Format::eR16G16B16A16Snorm ||
        fmt == vk::Format::eR16G16B16A16Uscaled ||
        fmt == vk::Format::eR16G16B16A16Sscaled ||
        fmt == vk::Format::eR16G16B16A16Uint ||
        fmt == vk::Format::eR16G16B16A16Sint ||
        fmt == vk::Format::eR16G16B16A16Sfloat ||
        fmt == vk::Format::eR32G32Uint || fmt == vk::Format::eR32G32Sint ||
        fmt == vk::Format::eR32G32Sfloat || fmt == vk::Format::eR64Uint ||
        fmt == vk::Format::eR64Sint || fmt == vk::Format::eR64Sfloat) {
        return vk::Format::eR64Sfloat;
    }

    if (fmt == vk::Format::eR32G32B32Uint ||
        fmt == vk::Format::eR32G32B32Sint ||
        fmt == vk::Format::eR32G32B32Sfloat) {
        return vk::Format::eR32G32B32Sfloat;
    }

    if (fmt == vk::Format::eR32G32B32A32Uint ||
        fmt == vk::Format::eR32G32B32A32Sint ||
        fmt == vk::Format::eR32G32B32A32Sfloat ||
        fmt == vk::Format::eR64G64Uint || fmt == vk::Format::eR64G64Sint ||
        fmt == vk::Format::eR64G64Sfloat) {
        return vk::Format::eR64G64Sfloat;
    }

    if (fmt == vk::Format::eR64G64B64Uint ||
        fmt == vk::Format::eR64G64B64Sint ||
        fmt == vk::Format::eR64G64B64Sfloat) {
        return vk::Format::eR64G64B64Sfloat;
    }

    if (fmt == vk::Format::eR64G64B64A64Uint ||
        fmt == vk::Format::eR64G64B64A64Sint ||
        fmt == vk::Format::eR64G64B64A64Sfloat) {
        return vk::Format::eR64G64B64A64Sfloat;
    }

    if (fmt == vk::Format::eBc1RgbUnormBlock ||
        fmt == vk::Format::eBc1RgbSrgbBlock) {
        return vk::Format::eBc1RgbSrgbBlock;
    }

    if (fmt == vk::Format::eBc1RgbaUnormBlock ||
        fmt == vk::Format::eBc1RgbaSrgbBlock) {
        return vk::Format::eBc1RgbaSrgbBlock;
    }

    if (fmt == vk::Format::eBc2UnormBlock || fmt == vk::Format::eBc2SrgbBlock) {
        return vk::Format::eBc2SrgbBlock;
    }

    if (fmt == vk::Format::eBc3UnormBlock || fmt == vk::Format::eBc3SrgbBlock) {
        return vk::Format::eBc3SrgbBlock;
    }

    if (fmt == vk::Format::eBc4UnormBlock ||
        fmt == vk::Format::eBc4SnormBlock) {
        return vk::Format::eBc4SnormBlock;
    }

    if (fmt == vk::Format::eBc5UnormBlock ||
        fmt == vk::Format::eBc5SnormBlock) {
        return vk::Format::eBc5SnormBlock;
    }

    if (fmt == vk::Format::eBc6HUfloatBlock ||
        fmt == vk::Format::eBc6HSfloatBlock) {
        return vk::Format::eBc6HSfloatBlock;
    }

    if (fmt == vk::Format::eBc7UnormBlock || fmt == vk::Format::eBc7SrgbBlock) {
        return vk::Format::eBc7SrgbBlock;
    }

    if (fmt == vk::Format::eEtc2R8G8B8UnormBlock ||
        fmt == vk::Format::eEtc2R8G8B8SrgbBlock) {
        return vk::Format::eEtc2R8G8B8SrgbBlock;
    }

    if (fmt == vk::Format::eEtc2R8G8B8A1UnormBlock ||
        fmt == vk::Format::eEtc2R8G8B8A1SrgbBlock) {
        return vk::Format::eEtc2R8G8B8A1SrgbBlock;
    }

    if (fmt == vk::Format::eEtc2R8G8B8A8UnormBlock ||
        fmt == vk::Format::eEtc2R8G8B8A8SrgbBlock) {
        return vk::Format::eEtc2R8G8B8A8SrgbBlock;
    }

    if (fmt == vk::Format::eEacR11UnormBlock ||
        fmt == vk::Format::eEacR11SnormBlock) {
        return vk::Format::eEacR11SnormBlock;
    }

    if (fmt == vk::Format::eEacR11G11UnormBlock ||
        fmt == vk::Format::eEacR11G11SnormBlock) {
        return vk::Format::eEacR11G11SnormBlock;
    }

    if (fmt == vk::Format::eAstc4x4SfloatBlock ||
        fmt == vk::Format::eAstc4x4UnormBlock ||
        fmt == vk::Format::eAstc4x4SrgbBlock) {
        return vk::Format::eAstc4x4SrgbBlock;
    }

    if (fmt == vk::Format::eAstc5x4SfloatBlock ||
        fmt == vk::Format::eAstc5x4UnormBlock ||
        fmt == vk::Format::eAstc5x4SrgbBlock) {
        return vk::Format::eAstc5x4SrgbBlock;
    }

    if (fmt == vk::Format::eAstc5x5SfloatBlock ||
        fmt == vk::Format::eAstc5x5UnormBlock ||
        fmt == vk::Format::eAstc5x5SrgbBlock) {
        return vk::Format::eAstc5x5SrgbBlock;
    }

    if (fmt == vk::Format::eAstc6x5SfloatBlock ||
        fmt == vk::Format::eAstc6x5UnormBlock ||
        fmt == vk::Format::eAstc6x5SrgbBlock) {
        return vk::Format::eAstc6x5SrgbBlock;
    }

    if (fmt == vk::Format::eAstc6x6SfloatBlock ||
        fmt == vk::Format::eAstc6x6UnormBlock ||
        fmt == vk::Format::eAstc6x6SrgbBlock) {
        return vk::Format::eAstc6x6SrgbBlock;
    }

    if (fmt == vk::Format::eAstc8x5SfloatBlock ||
        fmt == vk::Format::eAstc8x5UnormBlock ||
        fmt == vk::Format::eAstc8x5SrgbBlock) {
        return vk::Format::eAstc8x5SrgbBlock;
    }

    if (fmt == vk::Format::eAstc8x6SfloatBlock ||
        fmt == vk::Format::eAstc8x6UnormBlock ||
        fmt == vk::Format::eAstc8x6SrgbBlock) {
        return vk::Format::eAstc8x6SrgbBlock;
    }

    if (fmt == vk::Format::eAstc8x8SfloatBlock ||
        fmt == vk::Format::eAstc8x8UnormBlock ||
        fmt == vk::Format::eAstc8x8SrgbBlock) {
        return vk::Format::eAstc8x8SrgbBlock;
    }

    if (fmt == vk::Format::eAstc10x5SfloatBlock ||
        fmt == vk::Format::eAstc10x5UnormBlock ||
        fmt == vk::Format::eAstc10x5SrgbBlock) {
        return vk::Format::eAstc10x5SrgbBlock;
    }

    if (fmt == vk::Format::eAstc10x6SfloatBlock ||
        fmt == vk::Format::eAstc10x6UnormBlock ||
        fmt == vk::Format::eAstc10x6SrgbBlock) {
        return vk::Format::eAstc10x6SrgbBlock;
    }

    if (fmt == vk::Format::eAstc10x8SfloatBlock ||
        fmt == vk::Format::eAstc10x8UnormBlock ||
        fmt == vk::Format::eAstc10x8SrgbBlock) {
        return vk::Format::eAstc10x8SrgbBlock;
    }

    if (fmt == vk::Format::eAstc10x10SfloatBlock ||
        fmt == vk::Format::eAstc10x10UnormBlock ||
        fmt == vk::Format::eAstc10x10SrgbBlock) {
        return vk::Format::eAstc10x10SrgbBlock;
    }

    if (fmt == vk::Format::eAstc12x10SfloatBlock ||
        fmt == vk::Format::eAstc12x10UnormBlock ||
        fmt == vk::Format::eAstc12x10SrgbBlock) {
        return vk::Format::eAstc12x10SrgbBlock;
    }

    if (fmt == vk::Format::eAstc12x12SfloatBlock ||
        fmt == vk::Format::eAstc12x12UnormBlock ||
        fmt == vk::Format::eAstc12x12SrgbBlock) {
        return vk::Format::eAstc12x12SrgbBlock;
    }

    if (fmt == vk::Format::ePvrtc12BppSrgbBlockIMG ||
        fmt == vk::Format::ePvrtc12BppUnormBlockIMG) {
        return vk::Format::ePvrtc12BppUnormBlockIMG;
    }

    if (fmt == vk::Format::ePvrtc14BppSrgbBlockIMG ||
        fmt == vk::Format::ePvrtc14BppUnormBlockIMG) {
        return vk::Format::ePvrtc14BppUnormBlockIMG;
    }

    if (fmt == vk::Format::ePvrtc22BppSrgbBlockIMG ||
        fmt == vk::Format::ePvrtc22BppUnormBlockIMG) {
        return vk::Format::ePvrtc22BppUnormBlockIMG;
    }

    if (fmt == vk::Format::ePvrtc24BppSrgbBlockIMG ||
        fmt == vk::Format::ePvrtc24BppUnormBlockIMG) {
        return vk::Format::ePvrtc24BppUnormBlockIMG;
    }

    return fmt;
}

inline vk::Flags<vk::ColorComponentFlagBits> ColorWriteMask2Vk(

    ColorWriteMask mask) {
    if (mask == ColorWriteMask::All) {
        return vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eB |
               vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eR;
    }

    vk::Flags<vk::ColorComponentFlagBits> flags;
    if (static_cast<uint32_t>(mask) &
        static_cast<uint32_t>(ColorWriteMask::Alpha)) {
        flags |= vk::ColorComponentFlagBits::eA;
    }
    if (static_cast<uint32_t>(mask) &
        static_cast<uint32_t>(ColorWriteMask::Green)) {
        flags |= vk::ColorComponentFlagBits::eG;
    }
    if (static_cast<uint32_t>(mask) &
        static_cast<uint32_t>(ColorWriteMask::Blue)) {
        flags |= vk::ColorComponentFlagBits::eB;
    }
    if (static_cast<uint32_t>(mask) &
        static_cast<uint32_t>(ColorWriteMask::Red)) {
        flags |= vk::ColorComponentFlagBits::eR;
    }
    return flags;
}

}  // namespace nickel::rhi::vulkan