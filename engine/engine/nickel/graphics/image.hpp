#pragma once

namespace nickel::graphics {

class ImageImpl;

class Image {
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

    explicit Image(ImageImpl*);
    Image(const Image&);
    Image(Image&&) noexcept;
    Image& operator=(const Image&) noexcept;
    Image& operator=(Image&&) noexcept;
    ~Image();

    const ImageImpl& Impl() const noexcept;
    ImageImpl& Impl() noexcept;

private:
    ImageImpl* m_impl;
};

}  // namespace nickel::graphics