#pragma once

#include "core/cgmath.hpp"
#include "pch.hpp"
#include "rhi/image.hpp"


namespace nickel::rhi::vulkan {

class Device;

class Image : public rhi::Image {
public:
    Image(Device* device, ImageType type, const cgmath::Vec3& extent,
          const Format& format, ImageLayout initLayout, uint32_t arrayLayer,
          uint32_t mipLevel, SampleCountFlag sampleCount, ImageUsageFlags usage,
          ImageTiling tiling,
          std::optional<std::reference_wrapper<std::vector<uint32_t>>>
              queueIndices);

    Image(const Image&) = delete;

    Image(Image&& o) { swap(*this, o); }

    Image& operator=(const Image&) = delete;

    Image& operator=(Image&& o) {
        if (&o != this) {
            swap(*this, o);
        }
        return *this;
    }

    auto& Raw() const { return image_; }

    ~Image();

private:
    vk::Image image_;
    Device* device_ = nullptr;

    friend void swap(Image& o1, Image& o2) noexcept {
        using std::swap;
        swap(o1.image_, o2.image_);
        swap(o1.device_, o2.device_);
    }
};

struct ComponentMapping final {
    ComponentSwizzle r;
    ComponentSwizzle g;
    ComponentSwizzle b;
    ComponentSwizzle a;
};

struct ImageSubresourceRange final {
    uint32_t baseArrayLayer;
    uint32_t baseMipmapLevel;
    uint32_t layerCount;
    uint32_t levelCount;
    ImageAspectFlags aspectMask;
};

class ImageView : public rhi::ImageView {
public:
    ImageView(Device*, const Image&, ImageViewType, Format,
              const ComponentMapping&,
              const ImageSubresourceRange& subresourceRange);
    ImageView(const ImageView&) = delete;
    ImageView& operator=(const ImageView&) = delete;

    ImageView(ImageView&& o) { swap(*this, o); }

    ImageView& operator=(ImageView&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    auto& Raw() const { return view_; }

    ~ImageView();

private:
    Device* device_;
    vk::ImageView view_;

    friend void swap(ImageView& o1, ImageView& o2) {
        using std::swap;
        swap(o1.device_, o2.device_);
        swap(o1.view_, o2.view_);
    }
};

}  // namespace nickel::rhi::vulkan