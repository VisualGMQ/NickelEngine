#pragma once

#include "core/cgmath.hpp"
#include "core/gogl.hpp"
#include "core/handle.hpp"
#include "core/manager.hpp"
#include "core/resource.hpp"

/**
 * @addtogroup resource-manager
 * @{
 */

namespace nickel {

class Renderer2D;
class Texture;

using TextureHandle = Handle<Texture>;

class Texture final : public Res {
public:
    friend class Renderer2D;
    friend class TextureManager;
    friend class Material2D;

    static Texture Null;

    Texture(const Texture&) = delete;
    Texture(Texture&&) = default;
    Texture& operator=(Texture&&) = default;

    Texture& operator=(const Texture&) = delete;

    explicit operator bool() const {
        return handle_ == TextureHandle::Null();
    }

    int Width() const { return w_; }

    int Height() const { return h_; }

    cgmath::Vec2 Size() const {
        return cgmath::Vec2{static_cast<float>(w_), static_cast<float>(h_)};
    }

    void* Raw() const { return reinterpret_cast<void*>(texture_->Id()); }

    auto& Filename() const { return RelativePath(); }
    auto& Sampler() const { return sampler_; }

private:
    TextureHandle handle_;
    std::unique_ptr<gogl::Texture> texture_ = nullptr;
    gogl::Sampler sampler_;
    int w_ = 0;
    int h_ = 0;

    Texture(TextureHandle handle, const std::filesystem::path& root,
            const std::filesystem::path& filename, const gogl::Sampler&,
            gogl::Format fmt = gogl::Format::RGBA,
            gogl::Format gpuFmt = gogl::Format::RGBA);
    Texture(TextureHandle handle, void*, int w, int h,
            const gogl::Sampler& sampler, gogl::Format fmt = gogl::Format::RGBA,
            gogl::Format gpuFmt = gogl::Format::RGBA);
    Texture() = default;
};

class TextureManager final : public ResourceManager<Texture> {
public:
    template <typename T>
    friend std::enable_if_t<std::is_same_v<T, TextureManager>> serialize(
        const T&, ::mirrow::serd::srefl::serialize_destination_type_t<T>&);

    template <typename T>
    friend std::enable_if_t<std::is_same_v<T, TextureManager>> deserialize(
        const toml::node&, T&);

    TextureHandle Load(const std::string& filename, const gogl::Sampler&);
    std::unique_ptr<Texture> CreateSolitary(
        void* data, int w, int h, const gogl::Sampler&,
        gogl::Format fmt = gogl::Format::RGBA,
        gogl::Format gpuFmt = gogl::Format::RGBA);

    toml::table Save2Toml() const;
    void LoadFromToml(toml::table&);
};

}  // namespace nickel

/**
 * @}
 */