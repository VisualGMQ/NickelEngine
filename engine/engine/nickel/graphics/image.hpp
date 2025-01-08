#pragma once
#include "nickel/common/dllexport.hpp"
#include "nickel/common/flags.hpp"
#include "nickel/common/math/smatrix.hpp"
#include "nickel/graphics/enums.hpp"

namespace nickel::graphics {

class ImageViewImpl;
class Image;

class NICKEL_API ImageView {
public:
    struct Descriptor {
        struct ImageSubresourceRange {
            Flags<ImageAspect> aspectMask;
            uint32_t baseMipLevel;
            uint32_t levelCount;
            uint32_t baseArrayLayer;
            uint32_t layerCount;
        };

        ImageViewType viewType;
        Format format;
        ComponentMapping components;
        ImageSubresourceRange subresourceRange;
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
        ImageType imageType;
        Format format;
        SVector<uint32_t, 3> extent;
        uint32_t mipLevels;
        uint32_t arrayLayers;
        SampleCount samples;
        ImageTiling tiling;
        Flags<ImageUsage> usage;
        SharingMode sharingMode;
        ImageLayout initialLayout;
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