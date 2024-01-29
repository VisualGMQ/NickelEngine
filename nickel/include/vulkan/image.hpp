#pragma once

#include "core/cgmath.hpp"
#include "pch.hpp"


namespace nickel::vulkan {

class Device;

class Image final {
public:
    Image(Device*, vk::ImageType type, vk::ImageViewType viewType,
          const vk::Extent3D& extent, vk::Format format, vk::Format viewFormat,
          vk::ImageLayout initLayout, uint32_t arrayLayer, uint32_t mipLevel,
          vk::SampleCountFlagBits sampleCount, vk::ImageUsageFlagBits usage,
          vk::ImageTiling tiling, const vk::ComponentMapping& components,
          const vk::ImageSubresourceRange& subresourceRange,
          std::vector<uint32_t> queueIndices);
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

    friend void swap(Image& o1, Image& o2) noexcept {
        using std::swap;
        swap(o1.device_, o2.device_);
        swap(o1.image_, o2.image_);
        swap(o1.view_, o2.view_);
    }
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