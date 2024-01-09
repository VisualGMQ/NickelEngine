#pragma once

#include "pch.hpp"
#include "rhi/framebuffer.hpp"
#include "rhi/gl/image.hpp"

namespace nickel::rhi::gl {

class Framebuffer : public rhi::Framebuffer {
public:
    Framebuffer(const std::vector<ImageView>& imageViews);
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

private:
    GLuint id_;

    friend void swap(Framebuffer& o1, Framebuffer& o2) noexcept {
        using std::swap;
        swap(o1.id_, o2.id_);
    }
};

}  // namespace nickel::rhi::gl