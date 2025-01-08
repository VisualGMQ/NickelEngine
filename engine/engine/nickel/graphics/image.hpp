#pragma once
#include "nickel/graphics/enums.hpp"
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class ImageViewImpl;
class Image;

class NICKEL_API ImageView {
public:
    struct Descriptor {
        ImageViewType viewType;
        ImageFormat format;
        VkComponentMapping components;
        VkImageSubresourceRange subresourceRange;
    };

    ImageView() = default;
    explicit ImageView(ImageViewImpl*);
    ImageView(const ImageView&);
    ImageView(ImageView&&) noexcept;
    ImageView& operator=(const ImageView&) noexcept;
    ImageView& operator=(ImageView&&) noexcept;
    ~ImageView();
    Image GetImage() const;

    const ImageViewImpl& Impl() const noexcept;
    ImageViewImpl& Impl() noexcept;

    operator bool() const noexcept;
    void Release();

private:
    ImageViewImpl* m_impl;
};

class ImageImpl;

class NICKEL_API Image {
public:
    struct Descriptor {
        VkImageType imageType;
        VkFormat format;
        VkExtent3D extent;
        uint32_t mipLevels;
        uint32_t arrayLayers;
        VkSampleCountFlagBits samples;
        VkImageTiling tiling;
        VkImageUsageFlags usage;
        VkSharingMode sharingMode;
        VkImageLayout initialLayout;
    };

    Image() = default;
    explicit Image(ImageImpl*);
    Image(const Image&);
    Image(Image&&) noexcept;
    Image& operator=(const Image&) noexcept;
    Image& operator=(Image&&) noexcept;
    ImageView CreateView(const ImageView::Descriptor&);
    ~Image();

    const ImageImpl& Impl() const noexcept;
    ImageImpl& Impl() noexcept;

    operator bool() const noexcept;
    void Release();

private:
    ImageImpl* m_impl{};
};

}  // namespace nickel::graphics