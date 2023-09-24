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

    int Width() const { return w_; }

    int Height() const { return h_; }

    cgmath::Vec2 Size() const {
        return cgmath::Vec2{static_cast<float>(w_), static_cast<float>(h_)};
    }

    auto& Filename() const { return filename_; }
    auto& Sampler() const { return sampler_; }

private:
    TextureHandle handle_;
    std::unique_ptr<gogl::Texture> texture_ = nullptr;
    std::string filename_;
    gogl::Sampler sampler_;
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
        swap(lhs.filename_, rhs.filename_);
    }
};

class TextureManager final : public ResourceManager<Texture> {
public:
    template <typename T>
    friend std::enable_if_t<std::is_same_v<T, TextureManager>> serialize_impl(
        const T&,
        ::mirrow::serd::srefl::impl::serialize_destination_type_t<T>&);

    template <typename T>
    friend std::enable_if_t<std::is_same_v<T, TextureManager>> deserialize_impl(
        const toml::node&, T&);

    TextureHandle Load(const std::string& filename, const gogl::Sampler&);
    std::unique_ptr<Texture> CreateSolitary(void* data, int w, int h,
                                            const gogl::Sampler&);

    toml::table Save2Toml() const;
    void LoadFromToml(toml::table&);
};

}  // namespace nickel

/**
 * @}
 */