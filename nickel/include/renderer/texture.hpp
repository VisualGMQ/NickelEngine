#pragma once

#include "core/cgmath.hpp"
#include "core/gogl.hpp"
#include "core/handle.hpp"
#include "core/manager.hpp"
#include "pch.hpp"

/**
 * @addtogroup resource-manager
 * @{
 */

namespace nickel {

class Renderer2D;
class Texture;

using TextureHandle = Handle<Texture>;

class Texture final {
public:
    friend class Renderer2D;
    friend class TextureManager;

    static Texture Null;

    Texture(const Texture&) = delete;
    Texture(Texture&&);

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&);

    int W() const { return w_; }

    int H() const { return h_; }

    cgmath::Vec2 Size() const {
        return cgmath::Vec2{static_cast<float>(w_), static_cast<float>(h_)};
    }

private:
    TextureHandle handle_;
    std::unique_ptr<gogl::Texture> texture_ = nullptr;
    int w_ = 0;
    int h_ = 0;

    Texture(TextureHandle handle, const std::string& filename,
            const gogl::Sampler&);
    Texture(TextureHandle handle, void*, int w, int h,
            const gogl::Sampler& sampler);
    Texture() = default;

    friend void swap(Texture& lhs, Texture& rhs) {
        using std::swap;
        swap(lhs.handle_, rhs.handle_);
        swap(lhs.texture_, rhs.texture_);
        swap(lhs.w_, rhs.w_);
        swap(lhs.h_, rhs.h_);
    }
};

class TextureManager final : public ResourceManager<Texture> {
public:
    TextureHandle Load(const std::string& filename, const gogl::Sampler&);
    std::unique_ptr<Texture> CreateSolitary(void* data, int w, int h,
                                            const gogl::Sampler&);
};

}  // namespace nickel

/**
 * @}
 */