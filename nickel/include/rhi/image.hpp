#pragma once

#include "core/cgmath.hpp"
#include "rhi/enums.hpp"
#include <utility>


namespace nickel::rhi {

class ImageView {
public:
    ImageView(ImageViewType type, enum Format format)
        : type_{type}, format_{format} {}

    ImageView() = default;
    virtual ~ImageView() = default;

    ImageViewType ViewType() const noexcept { return type_; }

    enum Format Format() const noexcept { return format_; }

private:
    ImageViewType type_;
    enum Format format_;

protected:
    friend void swap(ImageView& o1, ImageView& o2) noexcept {
        using std::swap;
        swap(o1.type_, o2.type_);
        swap(o1.format_, o2.format_);
    }
};

class Image {
public:
    Image(ImageType type, const cgmath::Vec3& extent, Format f,
          ImageUsageFlags usage)
        : type_{type}, extent_{extent}, format_{f}, usage_{usage} {}

    virtual ~Image() = default;

    auto Type() const noexcept { return type_; }

    auto& Extent() const noexcept { return extent_; }

    auto Format() const noexcept { return format_; }

    auto Usage() const noexcept { return usage_; }

private:
    ImageType type_ = ImageType::e2D;
    cgmath::Vec3 extent_;
    enum Format format_ = Format::R8G8B8A8Srgb;
    ImageUsageFlags usage_ =
        static_cast<ImageUsageFlags>(ImageUsageFlagBits::ColorAttachment);

protected:
    Image() = default;

    friend void swap(Image& o1, Image& o2) noexcept {
        using std::swap;
        swap(o1.type_, o2.type_);
        swap(o1.extent_, o2.extent_);
        swap(o1.format_, o2.format_);
        swap(o1.usage_, o2.usage_);
    }
};

class Sampler {
public:
    virtual ~Sampler() = default;
};

}  // namespace nickel::rhi