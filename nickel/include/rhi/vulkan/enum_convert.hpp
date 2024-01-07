#pragma once

#include "pch.hpp"
#include "rhi/enums.hpp"
#include "rhi/vulkan/util.hpp"

namespace nickel::rhi::vulkan {

#define CASE(a, b) \
    case (a):      \
        return (b);

inline vk::Format Format2Vk(Format f) {
    switch (f) {
        CASE(Format::Undefined, vk::Format::eUndefined);
        CASE(Format::R4G4UnormPack8, vk::Format::eR4G4UnormPack8);
        CASE(Format::R4G4B4A4UnormPack16, vk::Format::eR4G4B4A4UnormPack16);
        CASE(Format::B4G4R4A4UnormPack16, vk::Format::eB4G4R4A4UnormPack16);
        CASE(Format::R5G6B5UnormPack16, vk::Format::eR5G6B5UnormPack16);
        CASE(Format::B5G6R5UnormPack16, vk::Format::eB5G6R5UnormPack16);
        CASE(Format::R5G5B5A1UnormPack16, vk::Format::eR5G5B5A1UnormPack16);
        CASE(Format::B5G5R5A1UnormPack16, vk::Format::eB5G5R5A1UnormPack16);
        CASE(Format::A1R5G5B5UnormPack16, vk::Format::eA1R5G5B5UnormPack16);
        CASE(Format::R8Unorm, vk::Format::eR8Unorm);
        CASE(Format::R8Snorm, vk::Format::eR8Snorm);
        CASE(Format::R8Uscaled, vk::Format::eR8Uscaled);
        CASE(Format::R8Sscaled, vk::Format::eR8Sscaled);
        CASE(Format::R8Uint, vk::Format::eR8Uint);
        CASE(Format::R8Sint, vk::Format::eR8Sint);
        CASE(Format::R8Srgb, vk::Format::eR8Srgb);
        CASE(Format::R8G8Unorm, vk::Format::eR8G8Unorm);
        CASE(Format::R8G8Snorm, vk::Format::eR8G8Snorm);
        CASE(Format::R8G8Uscaled, vk::Format::eR8G8Uscaled);
        CASE(Format::R8G8Sscaled, vk::Format::eR8G8Sscaled);
        CASE(Format::R8G8Uint, vk::Format::eR8G8Uint);
        CASE(Format::R8G8Sint, vk::Format::eR8G8Sint);
        CASE(Format::R8G8Srgb, vk::Format::eR8G8Srgb);
        CASE(Format::R8G8B8Unorm, vk::Format::eR8G8B8Unorm);
        CASE(Format::R8G8B8Snorm, vk::Format::eR8G8B8Snorm);
        CASE(Format::R8G8B8Uscaled, vk::Format::eR8G8B8Uscaled);
        CASE(Format::R8G8B8Sscaled, vk::Format::eR8G8B8Sscaled);
        CASE(Format::R8G8B8Uint, vk::Format::eR8G8B8Uint);
        CASE(Format::R8G8B8Sint, vk::Format::eR8G8B8Sint);
        CASE(Format::R8G8B8Srgb, vk::Format::eR8G8B8Srgb);
        CASE(Format::B8G8R8Unorm, vk::Format::eB8G8R8Unorm);
        CASE(Format::B8G8R8Snorm, vk::Format::eB8G8R8Snorm);
        CASE(Format::B8G8R8Uscaled, vk::Format::eB8G8R8Uscaled);
        CASE(Format::B8G8R8Sscaled, vk::Format::eB8G8R8Sscaled);
        CASE(Format::B8G8R8Uint, vk::Format::eB8G8R8Uint);
        CASE(Format::B8G8R8Sint, vk::Format::eB8G8R8Sint);
        CASE(Format::B8G8R8Srgb, vk::Format::eB8G8R8Srgb);
        CASE(Format::R8G8B8A8Unorm, vk::Format::eR8G8B8A8Unorm);
        CASE(Format::R8G8B8A8Snorm, vk::Format::eR8G8B8A8Snorm);
        CASE(Format::R8G8B8A8Uscaled, vk::Format::eR8G8B8A8Uscaled);
        CASE(Format::R8G8B8A8Sscaled, vk::Format::eR8G8B8A8Sscaled);
        CASE(Format::R8G8B8A8Uint, vk::Format::eR8G8B8A8Uint);
        CASE(Format::R8G8B8A8Sint, vk::Format::eR8G8B8A8Sint);
        CASE(Format::R8G8B8A8Srgb, vk::Format::eR8G8B8A8Srgb);
        CASE(Format::B8G8R8A8Unorm, vk::Format::eB8G8R8A8Unorm);
        CASE(Format::B8G8R8A8Snorm, vk::Format::eB8G8R8A8Snorm);
        CASE(Format::B8G8R8A8Uscaled, vk::Format::eB8G8R8A8Uscaled);
        CASE(Format::B8G8R8A8Sscaled, vk::Format::eB8G8R8A8Sscaled);
        CASE(Format::B8G8R8A8Uint, vk::Format::eB8G8R8A8Uint);
        CASE(Format::B8G8R8A8Sint, vk::Format::eB8G8R8A8Sint);
        CASE(Format::B8G8R8A8Srgb, vk::Format::eB8G8R8A8Srgb);
        CASE(Format::A8B8G8R8UnormPack32, vk::Format::eA8B8G8R8UnormPack32);
        CASE(Format::A8B8G8R8SnormPack32, vk::Format::eA8B8G8R8SnormPack32);
        CASE(Format::A8B8G8R8UscaledPack32, vk::Format::eA8B8G8R8UscaledPack32);
        CASE(Format::A8B8G8R8SscaledPack32, vk::Format::eA8B8G8R8SscaledPack32);
        CASE(Format::A8B8G8R8UintPack32, vk::Format::eA8B8G8R8UintPack32);
        CASE(Format::A8B8G8R8SintPack32, vk::Format::eA8B8G8R8SintPack32);
        CASE(Format::A8B8G8R8SrgbPack32, vk::Format::eA8B8G8R8SrgbPack32);
        CASE(Format::A2R10G10B10UnormPack32,
             vk::Format::eA2R10G10B10UnormPack32);
        CASE(Format::A2R10G10B10SnormPack32,
             vk::Format::eA2R10G10B10SnormPack32);
        CASE(Format::A2R10G10B10UscaledPack32,
             vk::Format::eA2R10G10B10UscaledPack32);
        CASE(Format::A2R10G10B10SscaledPack32,
             vk::Format::eA2R10G10B10SscaledPack32);
        CASE(Format::A2R10G10B10UintPack32, vk::Format::eA2R10G10B10UintPack32);
        CASE(Format::A2R10G10B10SintPack32, vk::Format::eA2R10G10B10SintPack32);
        CASE(Format::A2B10G10R10UnormPack32,
             vk::Format::eA2B10G10R10UnormPack32);
        CASE(Format::A2B10G10R10SnormPack32,
             vk::Format::eA2B10G10R10SnormPack32);
        CASE(Format::A2B10G10R10UscaledPack32,
             vk::Format::eA2B10G10R10UscaledPack32);
        CASE(Format::A2B10G10R10SscaledPack32,
             vk::Format::eA2B10G10R10SscaledPack32);
        CASE(Format::A2B10G10R10UintPack32, vk::Format::eA2B10G10R10UintPack32);
        CASE(Format::A2B10G10R10SintPack32, vk::Format::eA2B10G10R10SintPack32);
        CASE(Format::R16Unorm, vk::Format::eR16Unorm);
        CASE(Format::R16Snorm, vk::Format::eR16Snorm);
        CASE(Format::R16Uscaled, vk::Format::eR16Uscaled);
        CASE(Format::R16Sscaled, vk::Format::eR16Sscaled);
        CASE(Format::R16Uint, vk::Format::eR16Uint);
        CASE(Format::R16Sint, vk::Format::eR16Sint);
        CASE(Format::R16Sfloat, vk::Format::eR16Sfloat);
        CASE(Format::R16G16Unorm, vk::Format::eR16G16Unorm);
        CASE(Format::R16G16Snorm, vk::Format::eR16G16Snorm);
        CASE(Format::R16G16Uscaled, vk::Format::eR16G16Uscaled);
        CASE(Format::R16G16Sscaled, vk::Format::eR16G16Sscaled);
        CASE(Format::R16G16Uint, vk::Format::eR16G16Uint);
        CASE(Format::R16G16Sint, vk::Format::eR16G16Sint);
        CASE(Format::R16G16Sfloat, vk::Format::eR16G16Sfloat);
        CASE(Format::R16G16B16Unorm, vk::Format::eR16G16B16Unorm);
        CASE(Format::R16G16B16Snorm, vk::Format::eR16G16B16Snorm);
        CASE(Format::R16G16B16Uscaled, vk::Format::eR16G16B16Uscaled);
        CASE(Format::R16G16B16Sscaled, vk::Format::eR16G16B16Sscaled);
        CASE(Format::R16G16B16Uint, vk::Format::eR16G16B16Uint);
        CASE(Format::R16G16B16Sint, vk::Format::eR16G16B16Sint);
        CASE(Format::R16G16B16Sfloat, vk::Format::eR16G16B16Sfloat);
        CASE(Format::R16G16B16A16Unorm, vk::Format::eR16G16B16A16Unorm);
        CASE(Format::R16G16B16A16Snorm, vk::Format::eR16G16B16A16Snorm);
        CASE(Format::R16G16B16A16Uscaled, vk::Format::eR16G16B16A16Uscaled);
        CASE(Format::R16G16B16A16Sscaled, vk::Format::eR16G16B16A16Sscaled);
        CASE(Format::R16G16B16A16Uint, vk::Format::eR16G16B16A16Uint);
        CASE(Format::R16G16B16A16Sint, vk::Format::eR16G16B16A16Sint);
        CASE(Format::R16G16B16A16Sfloat, vk::Format::eR16G16B16A16Sfloat);
        CASE(Format::R32Uint, vk::Format::eR32Uint);
        CASE(Format::R32Sint, vk::Format::eR32Sint);
        CASE(Format::R32Sfloat, vk::Format::eR32Sfloat);
        CASE(Format::R32G32Uint, vk::Format::eR32G32Uint);
        CASE(Format::R32G32Sint, vk::Format::eR32G32Sint);
        CASE(Format::R32G32Sfloat, vk::Format::eR32G32Sfloat);
        CASE(Format::R32G32B32Uint, vk::Format::eR32G32B32Uint);
        CASE(Format::R32G32B32Sint, vk::Format::eR32G32B32Sint);
        CASE(Format::R32G32B32Sfloat, vk::Format::eR32G32B32Sfloat);
        CASE(Format::R32G32B32A32Uint, vk::Format::eR32G32B32A32Uint);
        CASE(Format::R32G32B32A32Sint, vk::Format::eR32G32B32A32Sint);
        CASE(Format::R32G32B32A32Sfloat, vk::Format::eR32G32B32A32Sfloat);
        CASE(Format::R64Uint, vk::Format::eR64Uint);
        CASE(Format::R64Sint, vk::Format::eR64Sint);
        CASE(Format::R64Sfloat, vk::Format::eR64Sfloat);
        CASE(Format::R64G64Uint, vk::Format::eR64G64Uint);
        CASE(Format::R64G64Sint, vk::Format::eR64G64Sint);
        CASE(Format::R64G64Sfloat, vk::Format::eR64G64Sfloat);
        CASE(Format::R64G64B64Uint, vk::Format::eR64G64B64Uint);
        CASE(Format::R64G64B64Sint, vk::Format::eR64G64B64Sint);
        CASE(Format::R64G64B64Sfloat, vk::Format::eR64G64B64Sfloat);
        CASE(Format::R64G64B64A64Uint, vk::Format::eR64G64B64A64Uint);
        CASE(Format::R64G64B64A64Sint, vk::Format::eR64G64B64A64Sint);
        CASE(Format::R64G64B64A64Sfloat, vk::Format::eR64G64B64A64Sfloat);
        CASE(Format::B10G11R11UfloatPack32, vk::Format::eB10G11R11UfloatPack32);
        CASE(Format::E5B9G9R9UfloatPack32, vk::Format::eE5B9G9R9UfloatPack32);
        CASE(Format::D16Unorm, vk::Format::eD16Unorm);
        CASE(Format::X8D24UnormPack32, vk::Format::eX8D24UnormPack32);
        CASE(Format::D32Sfloat, vk::Format::eD32Sfloat);
        CASE(Format::S8Uint, vk::Format::eS8Uint);
        CASE(Format::D16UnormS8Uint, vk::Format::eD16UnormS8Uint);
        CASE(Format::D24UnormS8Uint, vk::Format::eD24UnormS8Uint);
        CASE(Format::D32SfloatS8Uint, vk::Format::eD32SfloatS8Uint);
        CASE(Format::Bc1RgbUnormBlock, vk::Format::eBc1RgbUnormBlock);
        CASE(Format::Bc1RgbSrgbBlock, vk::Format::eBc1RgbSrgbBlock);
        CASE(Format::Bc1RgbaUnormBlock, vk::Format::eBc1RgbaUnormBlock);
        CASE(Format::Bc1RgbaSrgbBlock, vk::Format::eBc1RgbaSrgbBlock);
        CASE(Format::Bc2UnormBlock, vk::Format::eBc2UnormBlock);
        CASE(Format::Bc2SrgbBlock, vk::Format::eBc2SrgbBlock);
        CASE(Format::Bc3UnormBlock, vk::Format::eBc3UnormBlock);
        CASE(Format::Bc3SrgbBlock, vk::Format::eBc3SrgbBlock);
        CASE(Format::Bc4UnormBlock, vk::Format::eBc4UnormBlock);
        CASE(Format::Bc4SnormBlock, vk::Format::eBc4SnormBlock);
        CASE(Format::Bc5UnormBlock, vk::Format::eBc5UnormBlock);
        CASE(Format::Bc5SnormBlock, vk::Format::eBc5SnormBlock);
        CASE(Format::Bc6HUfloatBlock, vk::Format::eBc6HUfloatBlock);
        CASE(Format::Bc6HSfloatBlock, vk::Format::eBc6HSfloatBlock);
        CASE(Format::Bc7UnormBlock, vk::Format::eBc7UnormBlock);
        CASE(Format::Bc7SrgbBlock, vk::Format::eBc7SrgbBlock);
        CASE(Format::Etc2R8G8B8UnormBlock, vk::Format::eEtc2R8G8B8UnormBlock);
        CASE(Format::Etc2R8G8B8SrgbBlock, vk::Format::eEtc2R8G8B8SrgbBlock);
        CASE(Format::Etc2R8G8B8A1UnormBlock,
             vk::Format::eEtc2R8G8B8A1UnormBlock);
        CASE(Format::Etc2R8G8B8A1SrgbBlock, vk::Format::eEtc2R8G8B8A1SrgbBlock);
        CASE(Format::Etc2R8G8B8A8UnormBlock,
             vk::Format::eEtc2R8G8B8A8UnormBlock);
        CASE(Format::Etc2R8G8B8A8SrgbBlock, vk::Format::eEtc2R8G8B8A8SrgbBlock);
        CASE(Format::EacR11UnormBlock, vk::Format::eEacR11UnormBlock);
        CASE(Format::EacR11SnormBlock, vk::Format::eEacR11SnormBlock);
        CASE(Format::EacR11G11UnormBlock, vk::Format::eEacR11G11UnormBlock);
        CASE(Format::EacR11G11SnormBlock, vk::Format::eEacR11G11SnormBlock);
        CASE(Format::Astc4x4UnormBlock, vk::Format::eAstc4x4UnormBlock);
        CASE(Format::Astc4x4SrgbBlock, vk::Format::eAstc4x4SrgbBlock);
        CASE(Format::Astc5x4UnormBlock, vk::Format::eAstc5x4UnormBlock);
        CASE(Format::Astc5x4SrgbBlock, vk::Format::eAstc5x4SrgbBlock);
        CASE(Format::Astc5x5UnormBlock, vk::Format::eAstc5x5UnormBlock);
        CASE(Format::Astc5x5SrgbBlock, vk::Format::eAstc5x5SrgbBlock);
        CASE(Format::Astc6x5UnormBlock, vk::Format::eAstc6x5UnormBlock);
        CASE(Format::Astc6x5SrgbBlock, vk::Format::eAstc6x5SrgbBlock);
        CASE(Format::Astc6x6UnormBlock, vk::Format::eAstc6x6UnormBlock);
        CASE(Format::Astc6x6SrgbBlock, vk::Format::eAstc6x6SrgbBlock);
        CASE(Format::Astc8x5UnormBlock, vk::Format::eAstc8x5UnormBlock);
        CASE(Format::Astc8x5SrgbBlock, vk::Format::eAstc8x5SrgbBlock);
        CASE(Format::Astc8x6UnormBlock, vk::Format::eAstc8x6UnormBlock);
        CASE(Format::Astc8x6SrgbBlock, vk::Format::eAstc8x6SrgbBlock);
        CASE(Format::Astc8x8UnormBlock, vk::Format::eAstc8x8UnormBlock);
        CASE(Format::Astc8x8SrgbBlock, vk::Format::eAstc8x8SrgbBlock);
        CASE(Format::Astc10x5UnormBlock, vk::Format::eAstc10x5UnormBlock);
        CASE(Format::Astc10x5SrgbBlock, vk::Format::eAstc10x5SrgbBlock);
        CASE(Format::Astc10x6UnormBlock, vk::Format::eAstc10x6UnormBlock);
        CASE(Format::Astc10x6SrgbBlock, vk::Format::eAstc10x6SrgbBlock);
        CASE(Format::Astc10x8UnormBlock, vk::Format::eAstc10x8UnormBlock);
        CASE(Format::Astc10x8SrgbBlock, vk::Format::eAstc10x8SrgbBlock);
        CASE(Format::Astc10x10UnormBlock, vk::Format::eAstc10x10UnormBlock);
        CASE(Format::Astc10x10SrgbBlock, vk::Format::eAstc10x10SrgbBlock);
        CASE(Format::Astc12x10UnormBlock, vk::Format::eAstc12x10UnormBlock);
        CASE(Format::Astc12x10SrgbBlock, vk::Format::eAstc12x10SrgbBlock);
        CASE(Format::Astc12x12UnormBlock, vk::Format::eAstc12x12UnormBlock);
        CASE(Format::Astc12x12SrgbBlock, vk::Format::eAstc12x12SrgbBlock);
        CASE(Format::G8B8G8R8422Unorm, vk::Format::eG8B8G8R8422Unorm);
        CASE(Format::B8G8R8G8422Unorm, vk::Format::eB8G8R8G8422Unorm);
        CASE(Format::G8B8R83Plane420Unorm, vk::Format::eG8B8R83Plane420Unorm);
        CASE(Format::G8B8R82Plane420Unorm, vk::Format::eG8B8R82Plane420Unorm);
        CASE(Format::G8B8R83Plane422Unorm, vk::Format::eG8B8R83Plane422Unorm);
        CASE(Format::G8B8R82Plane422Unorm, vk::Format::eG8B8R82Plane422Unorm);
        CASE(Format::G8B8R83Plane444Unorm, vk::Format::eG8B8R83Plane444Unorm);
        CASE(Format::R10X6UnormPack16, vk::Format::eR10X6UnormPack16);
        CASE(Format::R10X6G10X6Unorm2Pack16,
             vk::Format::eR10X6G10X6Unorm2Pack16);
        CASE(Format::R10X6G10X6B10X6A10X6Unorm4Pack16,
             vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16);
        CASE(Format::G10X6B10X6G10X6R10X6422Unorm4Pack16,
             vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16);
        CASE(Format::B10X6G10X6R10X6G10X6422Unorm4Pack16,
             vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16);
        CASE(Format::G10X6B10X6R10X63Plane420Unorm3Pack16,
             vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16);
        CASE(Format::G10X6B10X6R10X62Plane420Unorm3Pack16,
             vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16);
        CASE(Format::G10X6B10X6R10X63Plane422Unorm3Pack16,
             vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16);
        CASE(Format::G10X6B10X6R10X62Plane422Unorm3Pack16,
             vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16);
        CASE(Format::G10X6B10X6R10X63Plane444Unorm3Pack16,
             vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16);
        CASE(Format::R12X4UnormPack16, vk::Format::eR12X4UnormPack16);
        CASE(Format::R12X4G12X4Unorm2Pack16,
             vk::Format::eR12X4G12X4Unorm2Pack16);
        CASE(Format::R12X4G12X4B12X4A12X4Unorm4Pack16,
             vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16);
        CASE(Format::G12X4B12X4G12X4R12X4422Unorm4Pack16,
             vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16);
        CASE(Format::B12X4G12X4R12X4G12X4422Unorm4Pack16,
             vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16);
        CASE(Format::G12X4B12X4R12X43Plane420Unorm3Pack16,
             vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16);
        CASE(Format::G12X4B12X4R12X42Plane420Unorm3Pack16,
             vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16);
        CASE(Format::G12X4B12X4R12X43Plane422Unorm3Pack16,
             vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16);
        CASE(Format::G12X4B12X4R12X42Plane422Unorm3Pack16,
             vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16);
        CASE(Format::G12X4B12X4R12X43Plane444Unorm3Pack16,
             vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16);
        CASE(Format::G16B16G16R16422Unorm, vk::Format::eG16B16G16R16422Unorm);
        CASE(Format::B16G16R16G16422Unorm, vk::Format::eB16G16R16G16422Unorm);
        CASE(Format::G16B16R163Plane420Unorm,
             vk::Format::eG16B16R163Plane420Unorm);
        CASE(Format::G16B16R162Plane420Unorm,
             vk::Format::eG16B16R162Plane420Unorm);
        CASE(Format::G16B16R163Plane422Unorm,
             vk::Format::eG16B16R163Plane422Unorm);
        CASE(Format::G16B16R162Plane422Unorm,
             vk::Format::eG16B16R162Plane422Unorm);
        CASE(Format::G16B16R163Plane444Unorm,
             vk::Format::eG16B16R163Plane444Unorm);
        CASE(Format::G8B8R82Plane444Unorm, vk::Format::eG8B8R82Plane444Unorm);
        CASE(Format::G10X6B10X6R10X62Plane444Unorm3Pack16,
             vk::Format::eG10X6B10X6R10X62Plane444Unorm3Pack16);
        CASE(Format::G12X4B12X4R12X42Plane444Unorm3Pack16,
             vk::Format::eG12X4B12X4R12X42Plane444Unorm3Pack16);
        CASE(Format::G16B16R162Plane444Unorm,
             vk::Format::eG16B16R162Plane444Unorm);
        CASE(Format::A4R4G4B4UnormPack16, vk::Format::eA4R4G4B4UnormPack16);
        CASE(Format::A4B4G4R4UnormPack16, vk::Format::eA4B4G4R4UnormPack16);
        CASE(Format::Astc4x4SfloatBlock, vk::Format::eAstc4x4SfloatBlock);
        CASE(Format::Astc5x4SfloatBlock, vk::Format::eAstc5x4SfloatBlock);
        CASE(Format::Astc5x5SfloatBlock, vk::Format::eAstc5x5SfloatBlock);
        CASE(Format::Astc6x5SfloatBlock, vk::Format::eAstc6x5SfloatBlock);
        CASE(Format::Astc6x6SfloatBlock, vk::Format::eAstc6x6SfloatBlock);
        CASE(Format::Astc8x5SfloatBlock, vk::Format::eAstc8x5SfloatBlock);
        CASE(Format::Astc8x6SfloatBlock, vk::Format::eAstc8x6SfloatBlock);
        CASE(Format::Astc8x8SfloatBlock, vk::Format::eAstc8x8SfloatBlock);
        CASE(Format::Astc10x5SfloatBlock, vk::Format::eAstc10x5SfloatBlock);
        CASE(Format::Astc10x6SfloatBlock, vk::Format::eAstc10x6SfloatBlock);
        CASE(Format::Astc10x8SfloatBlock, vk::Format::eAstc10x8SfloatBlock);
        CASE(Format::Astc10x10SfloatBlock, vk::Format::eAstc10x10SfloatBlock);
        CASE(Format::Astc12x10SfloatBlock, vk::Format::eAstc12x10SfloatBlock);
        CASE(Format::Astc12x12SfloatBlock, vk::Format::eAstc12x12SfloatBlock);
        CASE(Format::Pvrtc12BppUnormBlockIMG,
             vk::Format::ePvrtc12BppUnormBlockIMG);
        CASE(Format::Pvrtc14BppUnormBlockIMG,
             vk::Format::ePvrtc14BppUnormBlockIMG);
        CASE(Format::Pvrtc22BppUnormBlockIMG,
             vk::Format::ePvrtc22BppUnormBlockIMG);
        CASE(Format::Pvrtc24BppUnormBlockIMG,
             vk::Format::ePvrtc24BppUnormBlockIMG);
        CASE(Format::Pvrtc12BppSrgbBlockIMG,
             vk::Format::ePvrtc12BppSrgbBlockIMG);
        CASE(Format::Pvrtc14BppSrgbBlockIMG,
             vk::Format::ePvrtc14BppSrgbBlockIMG);
        CASE(Format::Pvrtc22BppSrgbBlockIMG,
             vk::Format::ePvrtc22BppSrgbBlockIMG);
        CASE(Format::Pvrtc24BppSrgbBlockIMG,
             vk::Format::ePvrtc24BppSrgbBlockIMG);
        CASE(Format::R16G16S105NV, vk::Format::eR16G16S105NV);
        CASE(Format::A4B4G4R4UnormPack16EXT,
             vk::Format::eA4B4G4R4UnormPack16EXT);
        CASE(Format::A4R4G4B4UnormPack16EXT,
             vk::Format::eA4R4G4B4UnormPack16EXT);
        CASE(Format::Astc10x10SfloatBlockEXT,
             vk::Format::eAstc10x10SfloatBlockEXT);
        CASE(Format::Astc10x5SfloatBlockEXT,
             vk::Format::eAstc10x5SfloatBlockEXT);
        CASE(Format::Astc10x6SfloatBlockEXT,
             vk::Format::eAstc10x6SfloatBlockEXT);
        CASE(Format::Astc10x8SfloatBlockEXT,
             vk::Format::eAstc10x8SfloatBlockEXT);
        CASE(Format::Astc12x10SfloatBlockEXT,
             vk::Format::eAstc12x10SfloatBlockEXT);
        CASE(Format::Astc12x12SfloatBlockEXT,
             vk::Format::eAstc12x12SfloatBlockEXT);
        CASE(Format::Astc4x4SfloatBlockEXT, vk::Format::eAstc4x4SfloatBlockEXT);
        CASE(Format::Astc5x4SfloatBlockEXT, vk::Format::eAstc5x4SfloatBlockEXT);
        CASE(Format::Astc5x5SfloatBlockEXT, vk::Format::eAstc5x5SfloatBlockEXT);
        CASE(Format::Astc6x5SfloatBlockEXT, vk::Format::eAstc6x5SfloatBlockEXT);
        CASE(Format::Astc6x6SfloatBlockEXT, vk::Format::eAstc6x6SfloatBlockEXT);
        CASE(Format::Astc8x5SfloatBlockEXT, vk::Format::eAstc8x5SfloatBlockEXT);
        CASE(Format::Astc8x6SfloatBlockEXT, vk::Format::eAstc8x6SfloatBlockEXT);
        CASE(Format::Astc8x8SfloatBlockEXT, vk::Format::eAstc8x8SfloatBlockEXT);
        CASE(Format::B10X6G10X6R10X6G10X6422Unorm4Pack16KHR,
             vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16KHR);
        CASE(Format::B12X4G12X4R12X4G12X4422Unorm4Pack16KHR,
             vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16KHR);
        CASE(Format::B16G16R16G16422UnormKHR,
             vk::Format::eB16G16R16G16422UnormKHR);
        CASE(Format::B8G8R8G8422UnormKHR, vk::Format::eB8G8R8G8422UnormKHR);
        CASE(Format::G10X6B10X6G10X6R10X6422Unorm4Pack16KHR,
             vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16KHR);
        CASE(Format::G10X6B10X6R10X62Plane420Unorm3Pack16KHR,
             vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16KHR);
        CASE(Format::G10X6B10X6R10X62Plane422Unorm3Pack16KHR,
             vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16KHR);
        CASE(Format::G10X6B10X6R10X62Plane444Unorm3Pack16EXT,
             vk::Format::eG10X6B10X6R10X62Plane444Unorm3Pack16EXT);
        CASE(Format::G10X6B10X6R10X63Plane420Unorm3Pack16KHR,
             vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16KHR);
        CASE(Format::G10X6B10X6R10X63Plane422Unorm3Pack16KHR,
             vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16KHR);
        CASE(Format::G10X6B10X6R10X63Plane444Unorm3Pack16KHR,
             vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16KHR);
        CASE(Format::G12X4B12X4G12X4R12X4422Unorm4Pack16KHR,
             vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16KHR);
        CASE(Format::G12X4B12X4R12X42Plane420Unorm3Pack16KHR,
             vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16KHR);
        CASE(Format::G12X4B12X4R12X42Plane422Unorm3Pack16KHR,
             vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16KHR);
        CASE(Format::G12X4B12X4R12X42Plane444Unorm3Pack16EXT,
             vk::Format::eG12X4B12X4R12X42Plane444Unorm3Pack16EXT);
        CASE(Format::G12X4B12X4R12X43Plane420Unorm3Pack16KHR,
             vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16KHR);
        CASE(Format::G12X4B12X4R12X43Plane422Unorm3Pack16KHR,
             vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16KHR);
        CASE(Format::G12X4B12X4R12X43Plane444Unorm3Pack16KHR,
             vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16KHR);
        CASE(Format::G16B16G16R16422UnormKHR,
             vk::Format::eG16B16G16R16422UnormKHR);
        CASE(Format::G16B16R162Plane420UnormKHR,
             vk::Format::eG16B16R162Plane420UnormKHR);
        CASE(Format::G16B16R162Plane422UnormKHR,
             vk::Format::eG16B16R162Plane422UnormKHR);
        CASE(Format::G16B16R162Plane444UnormEXT,
             vk::Format::eG16B16R162Plane444UnormEXT);
        CASE(Format::G16B16R163Plane420UnormKHR,
             vk::Format::eG16B16R163Plane420UnormKHR);
        CASE(Format::G16B16R163Plane422UnormKHR,
             vk::Format::eG16B16R163Plane422UnormKHR);
        CASE(Format::G16B16R163Plane444UnormKHR,
             vk::Format::eG16B16R163Plane444UnormKHR);
        CASE(Format::G8B8G8R8422UnormKHR, vk::Format::eG8B8G8R8422UnormKHR);
        CASE(Format::G8B8R82Plane420UnormKHR,
             vk::Format::eG8B8R82Plane420UnormKHR);
        CASE(Format::G8B8R82Plane422UnormKHR,
             vk::Format::eG8B8R82Plane422UnormKHR);
        CASE(Format::G8B8R82Plane444UnormEXT,
             vk::Format::eG8B8R82Plane444UnormEXT);
        CASE(Format::G8B8R83Plane420UnormKHR,
             vk::Format::eG8B8R83Plane420UnormKHR);
        CASE(Format::G8B8R83Plane422UnormKHR,
             vk::Format::eG8B8R83Plane422UnormKHR);
        CASE(Format::G8B8R83Plane444UnormKHR,
             vk::Format::eG8B8R83Plane444UnormKHR);
        CASE(Format::R10X6G10X6B10X6A10X6Unorm4Pack16KHR,
             vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16KHR);
        CASE(Format::R10X6G10X6Unorm2Pack16KHR,
             vk::Format::eR10X6G10X6Unorm2Pack16KHR);
        CASE(Format::R10X6UnormPack16KHR, vk::Format::eR10X6UnormPack16KHR);
        CASE(Format::R12X4G12X4B12X4A12X4Unorm4Pack16KHR,
             vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16KHR);
        CASE(Format::R12X4G12X4Unorm2Pack16KHR,
             vk::Format::eR12X4G12X4Unorm2Pack16KHR);
        CASE(Format::R12X4UnormPack16KHR, vk::Format::eR12X4UnormPack16KHR);
    }
}

inline vk::ShaderStageFlags ShaderStageFlags2Vk(ShaderStageFlags f) {
    uint32_t flags = 0;

    AttachMappedEnum(f, ShaderStageFlagBits::All, flags,
                     vk::ShaderStageFlagBits::eAll);
    AttachMappedEnum(f, ShaderStageFlagBits::Vertex, flags,
                     vk::ShaderStageFlagBits::eVertex);
    AttachMappedEnum(f, ShaderStageFlagBits::TessellationControl, flags,
                     vk::ShaderStageFlagBits::eTessellationControl);
    AttachMappedEnum(f, ShaderStageFlagBits::TessellationEvaluation, flags,
                     vk::ShaderStageFlagBits::eTessellationEvaluation);
    AttachMappedEnum(f, ShaderStageFlagBits::Geometry, flags,
                     vk::ShaderStageFlagBits::eGeometry);
    AttachMappedEnum(f, ShaderStageFlagBits::Fragment, flags,
                     vk::ShaderStageFlagBits::eFragment);
    AttachMappedEnum(f, ShaderStageFlagBits::Compute, flags,
                     vk::ShaderStageFlagBits::eCompute);
    AttachMappedEnum(f, ShaderStageFlagBits::AllGraphics, flags,
                     vk::ShaderStageFlagBits::eAllGraphics);

    return static_cast<vk::ShaderStageFlags>(flags);
}

inline vk::DescriptorType DescriptorType2Vk(DescriptorType type) {
    switch (type) {
        CASE(DescriptorType::Sampler, vk::DescriptorType::eSampler);
        CASE(DescriptorType::CombinedImageSampler,
             vk::DescriptorType::eCombinedImageSampler);
        CASE(DescriptorType::SampledImage, vk::DescriptorType::eSampledImage);
        CASE(DescriptorType::StorageImage, vk::DescriptorType::eStorageImage);
        CASE(DescriptorType::UniformTexelBuffer,
             vk::DescriptorType::eUniformTexelBuffer);
        CASE(DescriptorType::StorageTexelBuffer,
             vk::DescriptorType::eStorageTexelBuffer);
        CASE(DescriptorType::UniformBuffer, vk::DescriptorType::eUniformBuffer);
        CASE(DescriptorType::StorageBuffer, vk::DescriptorType::eStorageBuffer);
        CASE(DescriptorType::UniformBufferDynamic,
             vk::DescriptorType::eUniformBufferDynamic);
        CASE(DescriptorType::StorageBufferDynamic,
             vk::DescriptorType::eStorageBufferDynamic);
        CASE(DescriptorType::InputAttachment,
             vk::DescriptorType::eInputAttachment);
        CASE(DescriptorType::InlineUniformBlock,
             vk::DescriptorType::eInlineUniformBlock);
        CASE(DescriptorType::AccelerationStructureKHR,
             vk::DescriptorType::eAccelerationStructureKHR);
        CASE(DescriptorType::SampleWeightImageQCOM,
             vk::DescriptorType::eSampleWeightImageQCOM);
        CASE(DescriptorType::BlockMatchImageQCOM,
             vk::DescriptorType::eBlockMatchImageQCOM);
        CASE(DescriptorType::MutableVALVE, vk::DescriptorType::eMutableVALVE);
    }
};

inline vk::FrontFace FrontFace2Vk(FrontFace ff) {
    switch (ff) {
        CASE(FrontFace::CW, vk::FrontFace::eClockwise);
        CASE(FrontFace::CCW, vk::FrontFace::eCounterClockwise);
    }
}

inline vk::PolygonMode PolygonMode2Vk(PolygonMode mode) {
    switch (mode) {
        CASE(PolygonMode::Point, vk::PolygonMode::ePoint);
        CASE(PolygonMode::Line, vk::PolygonMode::eLine);
        CASE(PolygonMode::Fill, vk::PolygonMode::eFill);
    }
}

inline vk::CullModeFlagBits CullMode2Vk(CullMode mode) {
    switch (mode) {
        CASE(CullMode::None, vk::CullModeFlagBits::eNone);
        CASE(CullMode::Back, vk::CullModeFlagBits::eBack);
        CASE(CullMode::Front, vk::CullModeFlagBits::eFront);
        CASE(CullMode::Both, vk::CullModeFlagBits::eFrontAndBack);
    }
}

inline vk::CompareOp CompareOp2Vk(CompareOp op) {
    switch (op) {
        CASE(CompareOp::Never, vk::CompareOp::eNever);
        CASE(CompareOp::Less, vk::CompareOp::eLess);
        CASE(CompareOp::LessEqual, vk::CompareOp::eLessOrEqual);
        CASE(CompareOp::Equal, vk::CompareOp::eEqual);
        CASE(CompareOp::GreaterEqual, vk::CompareOp::eGreaterOrEqual);
        CASE(CompareOp::Greater, vk::CompareOp::eGreater);
        CASE(CompareOp::Always, vk::CompareOp::eAlways);
    }
}

inline vk::StencilOp StencilOpEnum2Vk(StencilOpEnum op) {
    switch (op) {
        CASE(StencilOpEnum::Keep, vk::StencilOp::eKeep);
        CASE(StencilOpEnum::Zero, vk::StencilOp::eZero);
        CASE(StencilOpEnum::Replace, vk::StencilOp::eReplace);
        CASE(StencilOpEnum::Increament, vk::StencilOp::eIncrementAndClamp);
        CASE(StencilOpEnum::IncreamentWrap, vk::StencilOp::eIncrementAndWrap);
        CASE(StencilOpEnum::Decreament, vk::StencilOp::eDecrementAndClamp);
        CASE(StencilOpEnum::DecreamentWrap, vk::StencilOp::eDecrementAndWrap);
        CASE(StencilOpEnum::Invert, vk::StencilOp::eInvert);
    }
}

inline vk::PrimitiveTopology Primitive2Vk(PrimitiveType type) {
    switch (type) {
        CASE(PrimitiveType::Points, vk::PrimitiveTopology::ePointList);
        CASE(PrimitiveType::LineStrip, vk::PrimitiveTopology::eLineStrip);
        CASE(PrimitiveType::Lines, vk::PrimitiveTopology::eLineList);
        CASE(PrimitiveType::LineStripAdjacency,
             vk::PrimitiveTopology::eLineStripWithAdjacency);
        CASE(PrimitiveType::LinesAdjacency,
             vk::PrimitiveTopology::eLineListWithAdjacency);
        CASE(PrimitiveType::TriangleStrip,
             vk::PrimitiveTopology::eTriangleStrip);
        CASE(PrimitiveType::TriangleFan, vk::PrimitiveTopology::eTriangleFan);
        CASE(PrimitiveType::Triangles, vk::PrimitiveTopology::eTriangleList);
        CASE(PrimitiveType::TriangleStripAdjacency,
             vk::PrimitiveTopology::eTriangleStripWithAdjacency);
        CASE(PrimitiveType::TrianglesAdjacency,
             vk::PrimitiveTopology::eTriangleListWithAdjacency);
        CASE(PrimitiveType::Patches, vk::PrimitiveTopology::ePatchList);
    }
}

inline vk::SampleCountFlagBits SampleCountFlag2Vk(SampleCountFlag f) {
    switch (f) {
        CASE(SampleCountFlag::e1, vk::SampleCountFlagBits::e1);
        CASE(SampleCountFlag::e2, vk::SampleCountFlagBits::e2);
        CASE(SampleCountFlag::e4, vk::SampleCountFlagBits::e4);
        CASE(SampleCountFlag::e8, vk::SampleCountFlagBits::e8);
        CASE(SampleCountFlag::e16, vk::SampleCountFlagBits::e16);
        CASE(SampleCountFlag::e32, vk::SampleCountFlagBits::e32);
        CASE(SampleCountFlag::e64, vk::SampleCountFlagBits::e64);
    }
}

inline vk::LogicOp LogicOp2Vk(LogicOp op) {
    switch (op) {
        CASE(LogicOp::Clear, vk::LogicOp::eClear);
        CASE(LogicOp::And, vk::LogicOp::eAnd);
        CASE(LogicOp::AndReverse, vk::LogicOp::eAndReverse);
        CASE(LogicOp::AndInverted, vk::LogicOp::eAndInverted);
        CASE(LogicOp::Copy, vk::LogicOp::eCopy);
        CASE(LogicOp::NoOp, vk::LogicOp::eNoOp);
        CASE(LogicOp::Xor, vk::LogicOp::eXor);
        CASE(LogicOp::Or, vk::LogicOp::eOr);
        CASE(LogicOp::Nor, vk::LogicOp::eNor);
        CASE(LogicOp::Equivalent, vk::LogicOp::eEquivalent);
        CASE(LogicOp::Invert, vk::LogicOp::eInvert);
        CASE(LogicOp::OrReverse, vk::LogicOp::eOrReverse);
        CASE(LogicOp::CopyInverted, vk::LogicOp::eCopyInverted);
        CASE(LogicOp::OrInverted, vk::LogicOp::eInvert);
        CASE(LogicOp::Nand, vk::LogicOp::eNand);
        CASE(LogicOp::Set, vk::LogicOp::eSet);
    }
}

inline vk::ColorComponentFlags ColorComponentFlag2Vk(ColorComponentFlags f) {
    uint32_t flags = 0;
    AttachMappedEnum(f, ColorComponentFlagBits::A, flags,
                     vk::ColorComponentFlagBits::eA);
    AttachMappedEnum(f, ColorComponentFlagBits::R, flags,
                     vk::ColorComponentFlagBits::eR);
    AttachMappedEnum(f, ColorComponentFlagBits::G, flags,
                     vk::ColorComponentFlagBits::eG);
    AttachMappedEnum(f, ColorComponentFlagBits::B, flags,
                     vk::ColorComponentFlagBits::eB);
    return static_cast<vk::ColorComponentFlags>(flags);
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

inline vk::BlendFactor BlendFactor2Vk(BlendFactor f) {
    switch (f) {
        CASE(BlendFactor::Zero, vk::BlendFactor::eZero);
        CASE(BlendFactor::One, vk::BlendFactor::eOne);
        CASE(BlendFactor::SrcColor, vk::BlendFactor::eSrcColor);
        CASE(BlendFactor::OneMinusSrcColor, vk::BlendFactor::eOneMinusSrcColor);
        CASE(BlendFactor::DstColor, vk::BlendFactor::eDstColor);
        CASE(BlendFactor::OneMinusDstColor, vk::BlendFactor::eOneMinusDstColor);
        CASE(BlendFactor::SrcAlpha, vk::BlendFactor::eSrcAlpha);
        CASE(BlendFactor::OneMinusSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha);
        CASE(BlendFactor::DstAlpha, vk::BlendFactor::eDstAlpha);
        CASE(BlendFactor::OneMinusDstAlpha, vk::BlendFactor::eOneMinusDstAlpha);
        CASE(BlendFactor::ConstantColor, vk::BlendFactor::eConstantColor);
        CASE(BlendFactor::OneMinusConstantColor,
             vk::BlendFactor::eOneMinusConstantColor);
        CASE(BlendFactor::ConstantAlpha, vk::BlendFactor::eConstantAlpha);
        CASE(BlendFactor::OneMinusConstantAlpha,
             vk::BlendFactor::eOneMinusConstantAlpha);
        CASE(BlendFactor::SrcAlphaSaturate, vk::BlendFactor::eSrcAlphaSaturate);
        CASE(BlendFactor::Src1Color, vk::BlendFactor::eSrc1Color);
        CASE(BlendFactor::OneMinusSrc1Color,
             vk::BlendFactor::eOneMinusSrc1Color);
        CASE(BlendFactor::Src1Alpha, vk::BlendFactor::eSrc1Alpha);
        CASE(BlendFactor::OneMinusSrc1Alpha,
             vk::BlendFactor::eOneMinusSrc1Alpha);
    }
}

inline vk::PipelineBindPoint PipelineBindPoint2Vk(PipelineBindPoint p) {
    switch (p) {
        CASE(PipelineBindPoint::Graphics, vk::PipelineBindPoint::eGraphics);
        CASE(PipelineBindPoint::Compute, vk::PipelineBindPoint::eCompute);
    }
};

inline vk::PipelineStageFlags PipelineStageFlags2Vk(PipelineStageFlags f) {
    uint32_t flags = 0;
    AttachMappedEnum(f, PipelineStageFlagBits::TopOfPipe, flags,
                     vk::PipelineStageFlagBits::eTopOfPipe);
    AttachMappedEnum(f, PipelineStageFlagBits::DrawIndirect, flags,
                     vk::PipelineStageFlagBits::eDrawIndirect);
    AttachMappedEnum(f, PipelineStageFlagBits::VertexInput, flags,
                     vk::PipelineStageFlagBits::eVertexInput);
    AttachMappedEnum(f, PipelineStageFlagBits::VertexShader, flags,
                     vk::PipelineStageFlagBits::eVertexShader);
    AttachMappedEnum(f, PipelineStageFlagBits::TessellationControlShader, flags,
                     vk::PipelineStageFlagBits::eTessellationControlShader);
    AttachMappedEnum(f, PipelineStageFlagBits::TessellationEvaluationShader,
                     flags,
                     vk::PipelineStageFlagBits::eTessellationEvaluationShader);
    AttachMappedEnum(f, PipelineStageFlagBits::GeometryShader, flags,
                     vk::PipelineStageFlagBits::eGeometryShader);
    AttachMappedEnum(f, PipelineStageFlagBits::FragmentShader, flags,
                     vk::PipelineStageFlagBits::eFragmentShader);
    AttachMappedEnum(f, PipelineStageFlagBits::EarlyFragmentTests, flags,
                     vk::PipelineStageFlagBits::eEarlyFragmentTests);
    AttachMappedEnum(f, PipelineStageFlagBits::LateFragmentTests, flags,
                     vk::PipelineStageFlagBits::eLateFragmentTests);
    AttachMappedEnum(f, PipelineStageFlagBits::ColorAttachmentOutput, flags,
                     vk::PipelineStageFlagBits::eColorAttachmentOutput);
    AttachMappedEnum(f, PipelineStageFlagBits::ComputeShader, flags,
                     vk::PipelineStageFlagBits::eComputeShader);
    AttachMappedEnum(f, PipelineStageFlagBits::Transfer, flags,
                     vk::PipelineStageFlagBits::eTransfer);
    AttachMappedEnum(f, PipelineStageFlagBits::BottomOfPipe, flags,
                     vk::PipelineStageFlagBits::eBottomOfPipe);
    AttachMappedEnum(f, PipelineStageFlagBits::Host, flags,
                     vk::PipelineStageFlagBits::eHost);
    AttachMappedEnum(f, PipelineStageFlagBits::AllGraphics, flags,
                     vk::PipelineStageFlagBits::eAllGraphics);
    AttachMappedEnum(f, PipelineStageFlagBits::AllCommands, flags,
                     vk::PipelineStageFlagBits::eAllCommands);
    AttachMappedEnum(f, PipelineStageFlagBits::None, flags,
                     vk::PipelineStageFlagBits::eNone);
    return static_cast<vk::PipelineStageFlags>(flags);
}

inline vk::AccessFlags AccessFlags2Vk(AccessFlags f) {
    uint32_t flags = 0;
    AttachMappedEnum(f, AccessFlagBits::IndirectCommandRead, flags,
                     vk::AccessFlagBits::eIndirectCommandRead);
    AttachMappedEnum(f, AccessFlagBits::IndexRead, flags,
                     vk::AccessFlagBits::eIndexRead);
    AttachMappedEnum(f, AccessFlagBits::VertexAttributeRead, flags,
                     vk::AccessFlagBits::eVertexAttributeRead);
    AttachMappedEnum(f, AccessFlagBits::UniformRead, flags,
                     vk::AccessFlagBits::eUniformRead);
    AttachMappedEnum(f, AccessFlagBits::InputAttachmentRead, flags,
                     vk::AccessFlagBits::eInputAttachmentRead);
    AttachMappedEnum(f, AccessFlagBits::ShaderRead, flags,
                     vk::AccessFlagBits::eShaderRead);
    AttachMappedEnum(f, AccessFlagBits::ShaderWrite, flags,
                     vk::AccessFlagBits::eShaderWrite);
    AttachMappedEnum(f, AccessFlagBits::ColorAttachmentRead, flags,
                     vk::AccessFlagBits::eColorAttachmentRead);
    AttachMappedEnum(f, AccessFlagBits::ColorAttachmentWrite, flags,
                     vk::AccessFlagBits::eColorAttachmentWrite);
    AttachMappedEnum(f, AccessFlagBits::DepthStencilAttachmentRead, flags,
                     vk::AccessFlagBits::eDepthStencilAttachmentRead);
    AttachMappedEnum(f, AccessFlagBits::DepthStencilAttachmentWrite, flags,
                     vk::AccessFlagBits::eDepthStencilAttachmentWrite);
    AttachMappedEnum(f, AccessFlagBits::TransferRead, flags,
                     vk::AccessFlagBits::eTransferRead);
    AttachMappedEnum(f, AccessFlagBits::TransferWrite, flags,
                     vk::AccessFlagBits::eTransferWrite);
    AttachMappedEnum(f, AccessFlagBits::HostRead, flags,
                     vk::AccessFlagBits::eHostRead);
    AttachMappedEnum(f, AccessFlagBits::HostWrite, flags,
                     vk::AccessFlagBits::eHostWrite);
    AttachMappedEnum(f, AccessFlagBits::MemoryRead, flags,
                     vk::AccessFlagBits::eMemoryRead);
    AttachMappedEnum(f, AccessFlagBits::MemoryWrite, flags,
                     vk::AccessFlagBits::eMemoryWrite);
    AttachMappedEnum(f, AccessFlagBits::None, flags, vk::AccessFlagBits::eNone);
    /*
    AttachMappedEnum(f, AccessFlagBits::TransformFeedbackWriteEXT, flags,
                     vk::AccessFlagBits::eTransformFeedbackWriteEXT);
    AttachMappedEnum(f, AccessFlagBits::TransformFeedbackCounterReadEXT, flags,
                     vk::AccessFlagBits::eTransformFeedbackCounterReadEXT);
    AttachMappedEnum(f, AccessFlagBits::TransformFeedbackCounterWriteEXT, flags,
                     vk::AccessFlagBits::eTransformFeedbackCounterWriteEXT);
    AttachMappedEnum(f, AccessFlagBits::ConditionalRenderingReadEXT, flags,
                     vk::AccessFlagBits::eConditionalRenderingReadEXT);
    AttachMappedEnum(f, AccessFlagBits::ColorAttachmentReadNoncoherentEXT,
    flags, vk::AccessFlagBits::eColorAttachmentReadNoncoherentEXT);
    AttachMappedEnum(f, AccessFlagBits::AccelerationStructureReadKHR, flags,
                     vk::AccessFlagBits::eAccelerationStructureReadKHR);
    AttachMappedEnum(f, AccessFlagBits::AccelerationStructureWriteKHR, flags,
                     vk::AccessFlagBits::eAccelerationStructureWriteKHR);
    AttachMappedEnum(f, AccessFlagBits::FragmentDensityMapReadEXT, flags,
                     vk::AccessFlagBits::eFragmentDensityMapReadEXT);
    AttachMappedEnum(f, AccessFlagBits::FragmentShadingRateAttachmentReadKHR,
                     flags,
                     vk::AccessFlagBits::eFragmentShadingRateAttachmentReadKHR);
    AttachMappedEnum(f, AccessFlagBits::CommandPreprocessReadNV, flags,
                     vk::AccessFlagBits::eCommandPreprocessReadNV);
    AttachMappedEnum(f, AccessFlagBits::CommandPreprocessWriteNV, flags,
                     vk::AccessFlagBits::eCommandPreprocessWriteNV);
    AttachMappedEnum(f, AccessFlagBits::AccelerationStructureReadNV, flags,
                     vk::AccessFlagBits::eAccelerationStructureReadNV);
    AttachMappedEnum(f, AccessFlagBits::AccelerationStructureWriteNV, flags,
                     vk::AccessFlagBits::eAccelerationStructureWriteNV);
    AttachMappedEnum(f, AccessFlagBits::NoneKHR, flags,
                     vk::AccessFlagBits::eNoneKHR);
    AttachMappedEnum(f, AccessFlagBits::ShadingRateImageReadNV, flags,
                     vk::AccessFlagBits::eShadingRateImageReadNV);
    */
    return static_cast<vk::AccessFlags>(flags);
}

inline vk::ImageLayout ImageLayout2Vk(ImageLayout layout) {
    switch (layout) {
        CASE(ImageLayout::Undefined, vk::ImageLayout::eUndefined);
        CASE(ImageLayout::General, vk::ImageLayout::eGeneral);
        CASE(ImageLayout::ColorAttachmentOptimal,
             vk::ImageLayout::eColorAttachmentOptimal);
        CASE(ImageLayout::DepthStencilAttachmentOptimal,
             vk::ImageLayout::eDepthStencilAttachmentOptimal);
        CASE(ImageLayout::DepthStencilReadOnlyOptimal,
             vk::ImageLayout::eDepthStencilReadOnlyOptimal);
        CASE(ImageLayout::ShaderReadOnlyOptimal,
             vk::ImageLayout::eShaderReadOnlyOptimal);
        CASE(ImageLayout::TransferSrcOptimal,
             vk::ImageLayout::eTransferSrcOptimal);
        CASE(ImageLayout::TransferDstOptimal,
             vk::ImageLayout::eTransferDstOptimal);
        CASE(ImageLayout::Preinitialized, vk::ImageLayout::ePreinitialized);
        CASE(ImageLayout::DepthReadOnlyStencilAttachmentOptimal,
             vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal);
        CASE(ImageLayout::DepthAttachmentStencilReadOnlyOptimal,
             vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal);
        CASE(ImageLayout::DepthAttachmentOptimal,
             vk::ImageLayout::eDepthAttachmentOptimal);
        CASE(ImageLayout::DepthReadOnlyOptimal,
             vk::ImageLayout::eDepthReadOnlyOptimal);
        CASE(ImageLayout::StencilAttachmentOptimal,
             vk::ImageLayout::eStencilAttachmentOptimal);
        CASE(ImageLayout::StencilReadOnlyOptimal,
             vk::ImageLayout::eStencilReadOnlyOptimal);
        CASE(ImageLayout::ReadOnlyOptimal, vk::ImageLayout::eReadOnlyOptimal);
        CASE(ImageLayout::AttachmentOptimal,
             vk::ImageLayout::eAttachmentOptimal);
        CASE(ImageLayout::PresentSrcKHR, vk::ImageLayout::ePresentSrcKHR);
    }
}

inline vk::AttachmentLoadOp AttachmentLoadOp2Vk(AttachmentLoadOp op) {
    switch (op) {
        CASE(AttachmentLoadOp::Load, vk::AttachmentLoadOp::eLoad);
        CASE(AttachmentLoadOp::Clear, vk::AttachmentLoadOp::eClear);
        CASE(AttachmentLoadOp::DontCare, vk::AttachmentLoadOp::eDontCare);
    }
}

inline vk::AttachmentStoreOp AttachmentStoreOp2Vk(AttachmentStoreOp op) {
    switch (op) {
        CASE(AttachmentStoreOp::Store, vk::AttachmentStoreOp::eStore);
        CASE(AttachmentStoreOp::DontCare, vk::AttachmentStoreOp::eDontCare);
        CASE(AttachmentStoreOp::None, vk::AttachmentStoreOp::eNone);
    }
}

inline vk::BufferUsageFlags BufferUsageFlags2Vk(BufferUsageFlags f) {
    uint32_t flags = 0;
    AttachMappedEnum(f, BufferUsageFlagBits::TransferSrc, flags,
                     vk::BufferUsageFlagBits::eTransferSrc);
    AttachMappedEnum(f, BufferUsageFlagBits::TransferDst, flags,
                     vk::BufferUsageFlagBits::eTransferDst);
    AttachMappedEnum(f, BufferUsageFlagBits::UniformTexelBuffer, flags,
                     vk::BufferUsageFlagBits::eUniformTexelBuffer);
    AttachMappedEnum(f, BufferUsageFlagBits::StorageTexelBuffer, flags,
                     vk::BufferUsageFlagBits::eStorageTexelBuffer);
    AttachMappedEnum(f, BufferUsageFlagBits::UniformBuffer, flags,
                     vk::BufferUsageFlagBits::eUniformBuffer);
    AttachMappedEnum(f, BufferUsageFlagBits::StorageBuffer, flags,
                     vk::BufferUsageFlagBits::eStorageBuffer);
    AttachMappedEnum(f, BufferUsageFlagBits::IndexBuffer, flags,
                     vk::BufferUsageFlagBits::eIndexBuffer);
    AttachMappedEnum(f, BufferUsageFlagBits::VertexBuffer, flags,
                     vk::BufferUsageFlagBits::eVertexBuffer);
    AttachMappedEnum(f, BufferUsageFlagBits::IndirectBuffer, flags,
                     vk::BufferUsageFlagBits::eIndirectBuffer);
    AttachMappedEnum(f, BufferUsageFlagBits::ShaderDeviceAddress, flags,
                     vk::BufferUsageFlagBits::eShaderDeviceAddress);
    return static_cast<vk::BufferUsageFlags>(flags);
}

inline vk::SharingMode SharingMode2Vk(SharingMode mode) {
    switch (mode) {
        CASE(SharingMode::Concurrent, vk::SharingMode::eConcurrent);
        CASE(SharingMode::Exclusive, vk::SharingMode::eExclusive);
    }
}

inline vk::MemoryPropertyFlags MemoryPropertyFlags2Vk(MemoryPropertyFlags f) {
    uint32_t flags = 0;
    AttachMappedEnum(f, MemoryPropertyFlagBits::DeviceLocal, flags,
                     vk::MemoryPropertyFlagBits::eDeviceLocal);
    AttachMappedEnum(f, MemoryPropertyFlagBits::HostVisible, flags,
                     vk::MemoryPropertyFlagBits::eHostVisible);
    AttachMappedEnum(f, MemoryPropertyFlagBits::HostCoherent, flags,
                     vk::MemoryPropertyFlagBits::eHostCoherent);
    AttachMappedEnum(f, MemoryPropertyFlagBits::HostCached, flags,
                     vk::MemoryPropertyFlagBits::eHostCached);
    AttachMappedEnum(f, MemoryPropertyFlagBits::LazilyAllocated, flags,
                     vk::MemoryPropertyFlagBits::eLazilyAllocated);
    AttachMappedEnum(f, MemoryPropertyFlagBits::Protected, flags,
                     vk::MemoryPropertyFlagBits::eProtected);
    return static_cast<vk::MemoryPropertyFlags>(flags);
}

inline vk::ImageViewType ImageViewType2Vk(ImageViewType type) {
    switch (type) {
        CASE(ImageViewType::e1D, vk::ImageViewType::e1D);
        CASE(ImageViewType::e2D, vk::ImageViewType::e2D);
        CASE(ImageViewType::e3D, vk::ImageViewType::e3D);
        CASE(ImageViewType::Cube, vk::ImageViewType::eCube);
        CASE(ImageViewType::e1DArray, vk::ImageViewType::e1DArray);
        CASE(ImageViewType::e2DArray, vk::ImageViewType::e2DArray);
        CASE(ImageViewType::CubeArray, vk::ImageViewType::eCubeArray);
    }
}

inline vk::ComponentSwizzle ComponentSwizzle2Vk(ComponentSwizzle swizzle) {
    switch (swizzle) {
        CASE(ComponentSwizzle::Identity, vk::ComponentSwizzle::eIdentity);
        CASE(ComponentSwizzle::Zero, vk::ComponentSwizzle::eZero);
        CASE(ComponentSwizzle::One, vk::ComponentSwizzle::eOne);
        CASE(ComponentSwizzle::R, vk::ComponentSwizzle::eR);
        CASE(ComponentSwizzle::G, vk::ComponentSwizzle::eG);
        CASE(ComponentSwizzle::B, vk::ComponentSwizzle::eB);
        CASE(ComponentSwizzle::A, vk::ComponentSwizzle::eA);
    }
}

inline vk::ImageAspectFlags ImageAspectFlags2Vk(ImageAspectFlags f) {
    uint32_t flags = 0;
    AttachMappedEnum(f, ImageAspectFlagBits::Color, flags,
                     vk::ImageAspectFlagBits::eColor);
    AttachMappedEnum(f, ImageAspectFlagBits::Depth, flags,
                     vk::ImageAspectFlagBits::eDepth);
    AttachMappedEnum(f, ImageAspectFlagBits::Stencil, flags,
                     vk::ImageAspectFlagBits::eStencil);
    AttachMappedEnum(f, ImageAspectFlagBits::Metadata, flags,
                     vk::ImageAspectFlagBits::eMetadata);
    AttachMappedEnum(f, ImageAspectFlagBits::Plane0, flags,
                     vk::ImageAspectFlagBits::ePlane0);
    AttachMappedEnum(f, ImageAspectFlagBits::Plane1, flags,
                     vk::ImageAspectFlagBits::ePlane1);
    AttachMappedEnum(f, ImageAspectFlagBits::Plane2, flags,
                     vk::ImageAspectFlagBits::ePlane2);
    AttachMappedEnum(f, ImageAspectFlagBits::None, flags,
                     vk::ImageAspectFlagBits::eNone);
    return static_cast<vk::ImageAspectFlags>(flags);
}

inline vk::ImageType ImageType2Vk(ImageType type) {
    switch (type) {
        CASE(ImageType::e1D, vk::ImageType::e1D);
        CASE(ImageType::e2D, vk::ImageType::e2D);
        CASE(ImageType::e3D, vk::ImageType::e3D);
    }
};

inline vk::ImageUsageFlags ImageUsageFlags2Vk(ImageUsageFlags usage) {
    uint32_t flags = 0;
    AttachMappedEnum(usage, ImageUsageFlagBits::TransferSrc, flags,
                     vk::ImageUsageFlagBits::eTransferSrc);
    AttachMappedEnum(usage, ImageUsageFlagBits::TransferDst, flags,
                     vk::ImageUsageFlagBits::eTransferDst);
    AttachMappedEnum(usage, ImageUsageFlagBits::Sampled, flags,
                     vk::ImageUsageFlagBits::eSampled);
    AttachMappedEnum(usage, ImageUsageFlagBits::Storage, flags,
                     vk::ImageUsageFlagBits::eStorage);
    AttachMappedEnum(usage, ImageUsageFlagBits::ColorAttachment, flags,
                     vk::ImageUsageFlagBits::eColorAttachment);
    AttachMappedEnum(usage, ImageUsageFlagBits::DepthStencilAttachment, flags,
                     vk::ImageUsageFlagBits::eDepthStencilAttachment);
    AttachMappedEnum(usage, ImageUsageFlagBits::TransientAttachment, flags,
                     vk::ImageUsageFlagBits::eTransientAttachment);
    AttachMappedEnum(usage, ImageUsageFlagBits::InputAttachment, flags,
                     vk::ImageUsageFlagBits::eInputAttachment);
    return static_cast<vk::ImageUsageFlags>(flags);
}

inline vk::ImageTiling ImageTiling2Vk(ImageTiling tiling) {
    switch (tiling) {
        CASE(ImageTiling::Linear, vk::ImageTiling::eLinear);
        CASE(ImageTiling::Optimal, vk::ImageTiling::eOptimal);
    }
}

inline vk::CommandPoolCreateFlags CommandPoolCreateFlag2Vk(
    CommandPoolCreateFlag f) {
    switch (f) {
        CASE(CommandPoolCreateFlag::Transient,
             vk::CommandPoolCreateFlagBits::eTransient);
        CASE(CommandPoolCreateFlag::ResetCommandBuffer,
             vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        CASE(CommandPoolCreateFlag::Protected,
             vk::CommandPoolCreateFlagBits::eProtected);
    }
}

inline vk::CommandBufferLevel CommandBufferLevel2Vk(CommandBufferLevel level) {
    switch (level) {
        CASE(CommandBufferLevel::Primary, vk::CommandBufferLevel::ePrimary);
        CASE(CommandBufferLevel::Secondary, vk::CommandBufferLevel::eSecondary);
    }
}

inline vk::CommandBufferUsageFlags CommandBufferUsage2Vk(
    CommandBufferUsage usage) {
    switch (usage) {
        CASE(CommandBufferUsage::OneTimeSubmit,
             vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        CASE(CommandBufferUsage::RenderPassContinue,
             vk::CommandBufferUsageFlagBits::eRenderPassContinue);
        CASE(CommandBufferUsage::SimultaneousUse,
             vk::CommandBufferUsageFlagBits::eSimultaneousUse);
    }
}

#undef CASE
}  // namespace nickel::rhi::vulkan