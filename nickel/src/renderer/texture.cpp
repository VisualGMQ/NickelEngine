#include "renderer/texture.hpp"

namespace nickel {

Texture Texture::Null = Texture{};

Texture::Texture(TextureHandle handle, const std::string& filename,
                 const gogl::Sampler& sampler)
    : handle_(handle) {
    stbi_uc* pixels =
        stbi_load(filename.c_str(), &w_, &h_, nullptr, STBI_rgb_alpha);
    if (pixels) {
        texture_ = std::make_unique<gogl::Texture>(
            gogl::Texture::Type::Dimension2, pixels, w_, h_, sampler,
            gogl::Format::RGBA, gogl::Format::RGBA);
    } else {
        LOGE(log_tag::Res, "create texture from ", filename, " failed");
    }
}

Texture::Texture(TextureHandle handle, void* pixels, int w, int h,
                 const gogl::Sampler& sampler)
    : handle_(handle) {
    texture_ = std::make_unique<gogl::Texture>(
        gogl::Texture::Type::Dimension2, pixels, w_, h_, sampler,
        gogl::Format::RGBA, gogl::Format::RGBA);
}

Texture& Texture::operator=(Texture&& img) {
    if (&img != this) {
        swap(*this, img);
    }
    return *this;
}

Texture::Texture(Texture&& o) : handle_(TextureHandle::Null()) {
    swap(*this, o);
}

TextureHandle TextureManager::Load(const std::string& filename, const gogl::Sampler& sampler) {
    TextureHandle handle = TextureHandle::Create();
    auto texture = std::unique_ptr<Texture>(new Texture{handle, filename, sampler});
    if (texture) {
        storeNewItem(handle, std::move(texture));
        return handle;
    } else {
        return TextureHandle::Null();
    }
}

std::unique_ptr<Texture> TextureManager::CreateSolitary(void* data, int w, int h, const gogl::Sampler& sampler) {
    return std::unique_ptr<Texture>(new Texture{TextureHandle::Null(), data, w, h, sampler});
}

}  // namespace nickel