#pragma once

namespace nickel::graphics {

class ImageViewImpl;

class ImageView {
public:
    struct Descriptor {
        VkImage image;
        VkImageViewType viewType;
        VkFormat format;
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

    const ImageViewImpl& Impl() const noexcept;
    ImageViewImpl& Impl() noexcept;

    operator bool() const noexcept;

private:
    ImageViewImpl* m_impl;
};

}  // namespace nickel::graphics