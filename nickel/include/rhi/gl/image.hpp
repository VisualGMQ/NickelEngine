#pragma once

#include "core/cgmath.hpp"
#include "pch.hpp"
#include "rhi/enums.hpp"
#include "rhi/image.hpp"

namespace nickel::rhi::gl {

class ImageView;

class Image : public rhi::Image {
public:
    Image(ImageType type, const cgmath::Vec3& extent, enum Format format,
          ImageUsageFlags usage);
    ~Image();

    enum ImageType ImageType() const noexcept { return type_; }

    enum Format Format() const noexcept { return format_; }

    auto Raw() const noexcept { return id_; }

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    Image(Image&& o) { swap(o, *this); }

    Image& operator=(Image&& o) {
        if (&o != this) {
            swap(*this, o);
        }
        return *this;
    }

private:
    GLuint id_ = 0;
    enum ImageType type_;
    enum Format format_;
    cgmath::Vec3 extent_;

    friend void swap(Image& o1, Image& o2) noexcept {
        using std::swap;
        swap(static_cast<rhi::Image&>(o1), static_cast<rhi::Image&>(o2));
        swap(o1.id_, o2.id_);
        swap(o1.type_, o2.type_);
        swap(o1.format_, o2.format_);
        swap(o1.extent_, o2.extent_);
    }
};

class ImageView : public rhi::ImageView {
public:
    ImageView(const Image& image, ImageViewType type, enum Format format);

    ImageView(const ImageView&) = delete;
    ImageView& operator=(const ImageView&) = delete;

    ImageView(ImageView&& o) { swap(o, *this); }

    ImageView& operator=(ImageView&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    const Image& GetImage() const { return *image_; }

private:
    const Image* image_{};
    GLenum glType_;

    friend void swap(ImageView& o1, ImageView& o2) noexcept {
        using std::swap;
        swap(static_cast<rhi::ImageView&>(o1),
             static_cast<rhi::ImageView&>(o2));
        swap(o1.image_, o2.image_);
        swap(o1.glType_, o2.glType_);
    }
};

class Sampler : public rhi::Sampler {
public:
    Sampler(Filter min, Filter mag, SamplerAddressMode u, SamplerAddressMode v,
            SamplerAddressMode w, float mipLodBias, bool anisotropyEnable,
            float maxAnisotropy, bool compareEnable, CompareOp compareOp,
            float minLod, float maxLod, BorderColor borderColor,
            bool unormalizedCoordinates);
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
    void Bind(GLuint textureIdx);

private:
    GLuint id_ = 0;

    friend void swap(Sampler& o1, Sampler& o2) noexcept {
        using std::swap;
        swap(o1.id_, o2.id_);
    }
};

}  // namespace nickel::rhi::gl