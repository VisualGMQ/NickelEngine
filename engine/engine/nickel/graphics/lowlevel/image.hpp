#pragma once
#include "nickel/common/dllexport.hpp"
#include "nickel/common/flags.hpp"
#include "nickel/common/math/smatrix.hpp"
#include "nickel/graphics/lowlevel/enums.hpp"

namespace nickel::graphics {

class ImageViewImpl;
class Image;

class NICKEL_API ImageView {
public:
    struct Descriptor {
        struct ImageSubresourceRange {
            Flags<ImageAspect> aspectMask = ImageAspect::None;
            uint32_t baseMipLevel = 0;
            uint32_t levelCount = 1;
            uint32_t baseArrayLayer = 0;
            uint32_t layerCount = 1;
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
    ImageViewImpl* m_impl{};
};

class ImageImpl;

class NICKEL_API Image {
public:
    struct Descriptor {
        bool is_cube_map = false;
        ImageType imageType = ImageType::Dim2;
        Format format = Format::R8G8B8A8_SRGB;
        SVector<uint32_t, 3> extent;
        uint32_t mipLevels = 1;
        uint32_t arrayLayers = 1;
        SampleCount samples = SampleCount::Count1;
        ImageTiling tiling = ImageTiling::Linear;
        Flags<ImageUsage> usage = ImageUsage::Sampled;
        SharingMode sharingMode = SharingMode::Concurrent;
        ImageLayout initialLayout = ImageLayout::Undefined;
    };

    Image() = default;
    explicit Image(ImageImpl*);
    Image(const Image&);
    Image(Image&&) noexcept;
    Image& operator=(const Image&) noexcept;
    Image& operator=(Image&&) noexcept;
    ImageView CreateView(const ImageView::Descriptor&);
    ~Image();
    
    SVector<uint32_t, 3> Extent() const;
    uint32_t MipLevelCount() const;

    const ImageImpl& Impl() const noexcept;
    ImageImpl& Impl() noexcept;

    operator bool() const noexcept;
    void Release();

private:
    ImageImpl* m_impl{};
};

}  // namespace nickel::graphics