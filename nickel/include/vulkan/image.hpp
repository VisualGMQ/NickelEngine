#pragma once

#include "common/cgmath.hpp"
#include "vulkan/pch.hpp"


namespace nickel::vulkan {

class Device;

inline uint32_t GetFormatSize(vk::Format fmt) {
    switch (fmt) {
        case vk::Format::eUndefined:
            return 0;
        case vk::Format::eR8Unorm:
        case vk::Format::eR8Snorm:
        case vk::Format::eR8Uscaled:
        case vk::Format::eR8Sscaled:
        case vk::Format::eR8Uint:
        case vk::Format::eR8Sint:
        case vk::Format::eR8Srgb:
        case vk::Format::eR4G4UnormPack8:
            return 1;
        case vk::Format::eR4G4B4A4UnormPack16:
        case vk::Format::eB4G4R4A4UnormPack16:
        case vk::Format::eR5G6B5UnormPack16:
        case vk::Format::eB5G6R5UnormPack16:
        case vk::Format::eR5G5B5A1UnormPack16:
        case vk::Format::eB5G5R5A1UnormPack16:
        case vk::Format::eA1R5G5B5UnormPack16:
        case vk::Format::eR8G8Unorm:
        case vk::Format::eR8G8Snorm:
        case vk::Format::eR8G8Uscaled:
        case vk::Format::eR8G8Sscaled:
        case vk::Format::eR8G8Uint:
        case vk::Format::eR8G8Sint:
        case vk::Format::eR8G8Srgb:
            return 2;
        case vk::Format::eR8G8B8Unorm:
        case vk::Format::eR8G8B8Snorm:
        case vk::Format::eR8G8B8Uscaled:
        case vk::Format::eR8G8B8Sscaled:
        case vk::Format::eR8G8B8Uint:
        case vk::Format::eR8G8B8Sint:
        case vk::Format::eR8G8B8Srgb:
        case vk::Format::eB8G8R8Unorm:
        case vk::Format::eB8G8R8Snorm:
        case vk::Format::eB8G8R8Uscaled:
        case vk::Format::eB8G8R8Sscaled:
        case vk::Format::eB8G8R8Uint:
        case vk::Format::eB8G8R8Sint:
        case vk::Format::eB8G8R8Srgb:
            return 3;
        case vk::Format::eR8G8B8A8Unorm:
        case vk::Format::eR8G8B8A8Snorm:
        case vk::Format::eR8G8B8A8Uscaled:
        case vk::Format::eR8G8B8A8Sscaled:
        case vk::Format::eR8G8B8A8Uint:
        case vk::Format::eR8G8B8A8Sint:
        case vk::Format::eR8G8B8A8Srgb:
        case vk::Format::eB8G8R8A8Unorm:
        case vk::Format::eB8G8R8A8Snorm:
        case vk::Format::eB8G8R8A8Uscaled:
        case vk::Format::eB8G8R8A8Sscaled:
        case vk::Format::eB8G8R8A8Uint:
        case vk::Format::eB8G8R8A8Sint:
        case vk::Format::eB8G8R8A8Srgb:
        case vk::Format::eA8B8G8R8UnormPack32:
        case vk::Format::eA8B8G8R8SnormPack32:
        case vk::Format::eA8B8G8R8UscaledPack32:
        case vk::Format::eA8B8G8R8SscaledPack32:
        case vk::Format::eA8B8G8R8UintPack32:
        case vk::Format::eA8B8G8R8SintPack32:
        case vk::Format::eA8B8G8R8SrgbPack32:
        case vk::Format::eA2R10G10B10UnormPack32:
        case vk::Format::eA2R10G10B10SnormPack32:
        case vk::Format::eA2R10G10B10UscaledPack32:
        case vk::Format::eA2R10G10B10SscaledPack32:
        case vk::Format::eA2R10G10B10UintPack32:
        case vk::Format::eA2R10G10B10SintPack32:
        case vk::Format::eA2B10G10R10UnormPack32:
        case vk::Format::eA2B10G10R10SnormPack32:
        case vk::Format::eA2B10G10R10UscaledPack32:
        case vk::Format::eA2B10G10R10SscaledPack32:
        case vk::Format::eA2B10G10R10UintPack32:
        case vk::Format::eA2B10G10R10SintPack32:
            return 4;
        case vk::Format::eR16Unorm:
        case vk::Format::eR16Snorm:
        case vk::Format::eR16Uscaled:
        case vk::Format::eR16Sscaled:
        case vk::Format::eR16Uint:
        case vk::Format::eR16Sint:
        case vk::Format::eR16Sfloat:
            return 2;
        case vk::Format::eR16G16Unorm:
        case vk::Format::eR16G16Snorm:
        case vk::Format::eR16G16Uscaled:
        case vk::Format::eR16G16Sscaled:
        case vk::Format::eR16G16Uint:
        case vk::Format::eR16G16Sint:
        case vk::Format::eR16G16Sfloat:
            return 4;
        case vk::Format::eR16G16B16Unorm:
        case vk::Format::eR16G16B16Snorm:
        case vk::Format::eR16G16B16Uscaled:
        case vk::Format::eR16G16B16Sscaled:
        case vk::Format::eR16G16B16Uint:
        case vk::Format::eR16G16B16Sint:
        case vk::Format::eR16G16B16Sfloat:
            return 6;
        case vk::Format::eR16G16B16A16Unorm:
        case vk::Format::eR16G16B16A16Snorm:
        case vk::Format::eR16G16B16A16Uscaled:
        case vk::Format::eR16G16B16A16Sscaled:
        case vk::Format::eR16G16B16A16Uint:
        case vk::Format::eR16G16B16A16Sint:
        case vk::Format::eR16G16B16A16Sfloat:
            return 8;
        case vk::Format::eR32Uint:
        case vk::Format::eR32Sint:
        case vk::Format::eR32Sfloat:
            return 4;
        case vk::Format::eR32G32Uint:
        case vk::Format::eR32G32Sint:
        case vk::Format::eR32G32Sfloat:
            return 8;
        case vk::Format::eR32G32B32Uint:
        case vk::Format::eR32G32B32Sint:
        case vk::Format::eR32G32B32Sfloat:
            return 12;
        case vk::Format::eR32G32B32A32Uint:
        case vk::Format::eR32G32B32A32Sint:
        case vk::Format::eR32G32B32A32Sfloat:
            return 16;
        case vk::Format::eR64Uint:
        case vk::Format::eR64Sint:
        case vk::Format::eR64Sfloat:
            return 8;
        case vk::Format::eR64G64Uint:
        case vk::Format::eR64G64Sint:
        case vk::Format::eR64G64Sfloat:
            return 16;
        case vk::Format::eR64G64B64Uint:
        case vk::Format::eR64G64B64Sint:
        case vk::Format::eR64G64B64Sfloat:
            return 24;
        case vk::Format::eR64G64B64A64Uint:
        case vk::Format::eR64G64B64A64Sint:
        case vk::Format::eR64G64B64A64Sfloat:
            return 32;
        case vk::Format::eB10G11R11UfloatPack32:
        case vk::Format::eE5B9G9R9UfloatPack32:
            return 4;
        case vk::Format::eD16Unorm:
            return 2;
        case vk::Format::eX8D24UnormPack32:
        case vk::Format::eD32Sfloat:
            return 4;
        case vk::Format::eS8Uint:
            return 1;
        case vk::Format::eD16UnormS8Uint:
            return 3;
        case vk::Format::eD24UnormS8Uint:
            return 4;
        case vk::Format::eD32SfloatS8Uint:
            return 5;
        // case vk::Format::eBc1RgbUnormBlock:
        // case vk::Format::eBc1RgbSrgbBlock:
        // case vk::Format::eBc1RgbaUnormBlock:
        // case vk::Format::eBc1RgbaSrgbBlock:
        // case vk::Format::eBc2UnormBlock:
        // case vk::Format::eBc2SrgbBlock:
        // case vk::Format::eBc3UnormBlock:
        // case vk::Format::eBc3SrgbBlock:
        // case vk::Format::eBc4UnormBlock:
        // case vk::Format::eBc4SnormBlock:
        // case vk::Format::eBc5UnormBlock:
        // case vk::Format::eBc5SnormBlock:
        // case vk::Format::eBc6HUfloatBlock:
        // case vk::Format::eBc6HSfloatBlock:
        // case vk::Format::eBc7UnormBlock:
        // case vk::Format::eBc7SrgbBlock:
        // case vk::Format::eEtc2R8G8B8UnormBlock:
        // case vk::Format::eEtc2R8G8B8SrgbBlock:
        // case vk::Format::eEtc2R8G8B8A1UnormBlock:
        // case vk::Format::eEtc2R8G8B8A1SrgbBlock:
        // case vk::Format::eEtc2R8G8B8A8UnormBlock:
        // case vk::Format::eEtc2R8G8B8A8SrgbBlock:
        // case vk::Format::eEacR11UnormBlock:
        // case vk::Format::eEacR11SnormBlock:
        // case vk::Format::eEacR11G11UnormBlock:
        // case vk::Format::eEacR11G11SnormBlock:
        // case vk::Format::eAstc4x4UnormBlock:
        // case vk::Format::eAstc4x4SrgbBlock:
        // case vk::Format::eAstc5x4UnormBlock:
        // case vk::Format::eAstc5x4SrgbBlock:
        // case vk::Format::eAstc5x5UnormBlock:
        // case vk::Format::eAstc5x5SrgbBlock:
        // case vk::Format::eAstc6x5UnormBlock:
        // case vk::Format::eAstc6x5SrgbBlock:
        // case vk::Format::eAstc6x6UnormBlock:
        // case vk::Format::eAstc6x6SrgbBlock:
        // case vk::Format::eAstc8x5UnormBlock:
        // case vk::Format::eAstc8x5SrgbBlock:
        // case vk::Format::eAstc8x6UnormBlock:
        // case vk::Format::eAstc8x6SrgbBlock:
        // case vk::Format::eAstc8x8UnormBlock:
        // case vk::Format::eAstc8x8SrgbBlock:
        // case vk::Format::eAstc10x5UnormBlock:
        // case vk::Format::eAstc10x5SrgbBlock:
        // case vk::Format::eAstc10x6UnormBlock:
        // case vk::Format::eAstc10x6SrgbBlock:
        // case vk::Format::eAstc10x8UnormBlock:
        // case vk::Format::eAstc10x8SrgbBlock:
        // case vk::Format::eAstc10x10UnormBlock:
        // case vk::Format::eAstc10x10SrgbBlock:
        // case vk::Format::eAstc12x10UnormBlock:
        // case vk::Format::eAstc12x10SrgbBlock:
        // case vk::Format::eAstc12x12UnormBlock:
        // case vk::Format::eAstc12x12SrgbBlock:
        // case vk::Format::eG8B8G8R8422Unorm:
        // case vk::Format::eB8G8R8G8422Unorm:
        // case vk::Format::eG8B8R83Plane420Unorm:
        // case vk::Format::eG8B8R82Plane420Unorm:
        // case vk::Format::eG8B8R83Plane422Unorm:
        // case vk::Format::eG8B8R82Plane422Unorm:
        // case vk::Format::eG8B8R83Plane444Unorm:
        // case vk::Format::eR10X6UnormPack16:
        // case vk::Format::eR10X6G10X6Unorm2Pack16:
        // case vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16:
        // case vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16:
        // case vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16:
        // case vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16:
        // case vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16:
        // case vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16:
        // case vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16:
        // case vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16:
        // case vk::Format::eR12X4UnormPack16:
        // case vk::Format::eR12X4G12X4Unorm2Pack16:
        // case vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16:
        // case vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16:
        // case vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16:
        // case vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16:
        // case vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16:
        // case vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16:
        // case vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16:
        // case vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16:
        // case vk::Format::eG16B16G16R16422Unorm:
        // case vk::Format::eB16G16R16G16422Unorm:
        // case vk::Format::eG16B16R163Plane420Unorm:
        // case vk::Format::eG16B16R162Plane420Unorm:
        // case vk::Format::eG16B16R163Plane422Unorm:
        // case vk::Format::eG16B16R162Plane422Unorm:
        // case vk::Format::eG16B16R163Plane444Unorm:
        // case vk::Format::eG8B8R82Plane444Unorm:
        // case vk::Format::eG10X6B10X6R10X62Plane444Unorm3Pack16:
        // case vk::Format::eG12X4B12X4R12X42Plane444Unorm3Pack16:
        // case vk::Format::eG16B16R162Plane444Unorm:
        // case vk::Format::eA4R4G4B4UnormPack16:
        // case vk::Format::eA4B4G4R4UnormPack16:
        // case vk::Format::eAstc4x4SfloatBlock:
        // case vk::Format::eAstc5x4SfloatBlock:
        // case vk::Format::eAstc5x5SfloatBlock:
        // case vk::Format::eAstc6x5SfloatBlock:
        // case vk::Format::eAstc6x6SfloatBlock:
        // case vk::Format::eAstc8x5SfloatBlock:
        // case vk::Format::eAstc8x6SfloatBlock:
        // case vk::Format::eAstc8x8SfloatBlock:
        // case vk::Format::eAstc10x5SfloatBlock:
        // case vk::Format::eAstc10x6SfloatBlock:
        // case vk::Format::eAstc10x8SfloatBlock:
        // case vk::Format::eAstc10x10SfloatBlock:
        // case vk::Format::eAstc12x10SfloatBlock:
        // case vk::Format::eAstc12x12SfloatBlock:
        // case vk::Format::ePvrtc12BppUnormBlockIMG:
        // case vk::Format::ePvrtc14BppUnormBlockIMG:
        // case vk::Format::ePvrtc22BppUnormBlockIMG:
        // case vk::Format::ePvrtc24BppUnormBlockIMG:
        // case vk::Format::ePvrtc12BppSrgbBlockIMG:
        // case vk::Format::ePvrtc14BppSrgbBlockIMG:
        // case vk::Format::ePvrtc22BppSrgbBlockIMG:
        // case vk::Format::ePvrtc24BppSrgbBlockIMG:
        // case vk::Format::eR16G16S105NV:
        // case vk::Format::eA1B5G5R5UnormPack16KHR:
        // case vk::Format::eA8UnormKHR:
        default:
            Assert(false, "unimplement enum");
            return 0;
    }
}

class Image final {
public:
    Image(Device*, vk::ImageType type, vk::ImageViewType viewType,
          const vk::Extent3D& extent, vk::Format format, vk::Format viewFormat,
          vk::ImageLayout initLayout, uint32_t arrayLayer, uint32_t mipLevel,
          vk::SampleCountFlagBits sampleCount, vk::Flags<vk::ImageUsageFlagBits> usage,
          vk::ImageTiling tiling, const vk::ComponentMapping& components,
          const vk::ImageSubresourceRange& subresourceRange,
          const std::set<uint32_t>& queueIndices);
    ~Image();

    void ChangeView(vk::ImageViewType type, vk::Format format,
                    const vk::ComponentMapping& componentMapping,
                    const vk::ImageSubresourceRange& range);

    auto GetImage() const { return image_; }

    auto GetImage() { return image_; }

    auto GetView() const { return view_; }

    auto GetView() { return view_; }

private:
    Device* device_{};
    vk::Image image_;
    vk::ImageView view_;
    vk::DeviceMemory mem_;

    friend void swap(Image& o1, Image& o2) noexcept {
        using std::swap;
        swap(o1.device_, o2.device_);
        swap(o1.image_, o2.image_);
        swap(o1.view_, o2.view_);
        swap(o1.mem_, o2.mem_);
    }

    void allocateMem(vk::Format format, const vk::Extent3D& extent);
};

class Framebuffer final {
public:
    Framebuffer(Device* device, const std::vector<vk::ImageView>& views,
                uint32_t width, uint32_t height, uint32_t layers,
                vk::RenderPass renderPass);
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    Framebuffer(Framebuffer&& o) { swap(o, *this); }

    Framebuffer& operator=(Framebuffer&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    ~Framebuffer();

    operator vk::Framebuffer() const { return fbo_; }

    operator vk::Framebuffer() { return fbo_; }

private:
    Device* device_{};
    vk::Framebuffer fbo_;

    friend void swap(Framebuffer& o1, Framebuffer& o2) noexcept {
        using std::swap;
        swap(o1.device_, o2.device_);
        swap(o1.fbo_, o2.fbo_);
    }
};

class Sampler final {
public:
    Sampler(Device* device, vk::Filter min, vk::Filter mag,
            vk::SamplerMipmapMode mipmap, vk::SamplerAddressMode u,
            vk::SamplerAddressMode v, vk::SamplerAddressMode w,
            float mipLodBias, bool anisotropyEnable, float maxAnisotropy,
            bool compareEnable, vk::CompareOp compare, float minLod,
            float maxLod, vk::BorderColor borderColor,
            bool unnormalizedCoordinates);
    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;

    Sampler(Sampler&& o) { swap(o, *this); }

    Sampler& operator=(Sampler&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    ~Sampler();

    operator vk::Sampler() const { return sampler_; }
    operator vk::Sampler() { return sampler_; }

private:
    Device* device_{};
    vk::Sampler sampler_;

    friend void swap(Sampler& o1, Sampler& o2) noexcept {
        using std::swap;

        swap(o1.device_, o2.device_);
        swap(o1.sampler_, o2.sampler_);
    }
};

}  // namespace nickel::vulkan