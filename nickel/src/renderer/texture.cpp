#include "renderer/texture.hpp"

namespace nickel {

Texture Texture::Null = Texture{};

Texture::Texture(const std::filesystem::path& filename,
                 const gogl::Sampler& sampler, gogl::Format fmt,
                 gogl::Format gpuFmt): Asset(filename) {
    stbi_uc* pixels = stbi_load((filename).string().c_str(), &w_, &h_, nullptr,
                                STBI_rgb_alpha);
    if (pixels) {
        texture_ = std::make_unique<gogl::Texture>(
            gogl::Texture::Type::Dimension2, pixels, w_, h_, sampler, fmt,
            gpuFmt, gogl::DataType::UByte);
        sampler_ = sampler;
        w_ = texture_->Width();
        h_ = texture_->Height();
    } else {
        LOGE(log_tag::Asset, "create texture from ", filename, " failed");
    }
}

Texture::Texture(const toml::table& tbl) {
    std::filesystem::path filename;

    do {
        if (auto path = tbl.get("path"); !path || !path->is_string()) {
            LOGW(log_tag::Asset,
                 "deserialize texture failed! `path` field not string");
            break;
        } else {
            filename = path->as_string()->get();
        }

        gogl::Sampler sampler;
        auto samplerTypeinfo = mirrow::drefl::typeinfo<gogl::Sampler>();
        auto samplerName = samplerTypeinfo->name();
        if (auto node = tbl.get(samplerName); node && node->is_table()) {
            auto ref = mirrow::drefl::any_make_ref(sampler);
            mirrow::serd::drefl::deserialize(ref, *node->as_table());
        } else {
            LOGW(log_tag::Asset, "deserialize texture failed! `", samplerName,
                 "` field not table");
            break;
        }

        auto newTexture = Texture(filename, sampler);
        newTexture.AssociateFile(filename);
        *this = std::move(newTexture);
    } while (0);
}

Texture::Texture(void* pixels, int w, int h, const gogl::Sampler& sampler,
                 gogl::Format fmt, gogl::Format gpuFmt)
    : sampler_(sampler), w_(w), h_(h) {
    texture_ = std::make_unique<gogl::Texture>(gogl::Texture::Type::Dimension2,
                                               pixels, w, h, sampler, fmt,
                                               gpuFmt, gogl::DataType::UByte);
}

template <>
std::unique_ptr<Texture> LoadAssetFromToml(const toml::table& tbl) {
    return std::make_unique<Texture>(tbl);
}

toml::table Texture::Save2Toml() const {
    toml::table tbl;
    tbl.emplace("path", RelativePath().string());
    mirrow::serd::drefl::serialize(
        tbl, mirrow::drefl::any_make_constref(Sampler()),
        mirrow::drefl::typeinfo<gogl::Sampler>()->name());
    return tbl;
}

TextureHandle TextureManager::Load(const std::filesystem::path& filename,
                                   const gogl::Sampler& sampler) {
    if (Has(filename)) {
        return GetHandle(filename);
    }

    TextureHandle handle = TextureHandle::Create();
    auto texture = std::make_unique<Texture>(filename, sampler);
    if (texture && *texture) {
        storeNewItem(handle, std::move(texture));
        return handle;
    } else {
        return TextureHandle::Null();
    }
}

bool TextureManager::Replace(TextureHandle handle,
                             const std::filesystem::path& filename,
                             const gogl::Sampler& sampler) {
    if (!Has(handle)) {
        return false;
    }

    Texture texture{filename, sampler};
    if (texture) {
        Get(handle) = std::move(texture);
        return true;
    } else {
        return false;
    }
}

std::unique_ptr<Texture> TextureManager::CreateSolitary(
    void* data, int w, int h, const gogl::Sampler& sampler, gogl::Format fmt,
    gogl::Format gpuFmt) {
    auto texture =
        std::unique_ptr<Texture>(new Texture{data, w, h, sampler, fmt, gpuFmt});
    if (texture && *texture) {
        return std::move(texture);
    } else {
        return nullptr;
    }
}

TextureHandle TextureManager::LoadSVG(const std::filesystem::path& filename,
                                      const gogl::Sampler&,
                                      std::optional<cgmath::Vec2> size) {
    if (Has(filename)) {
        return GetHandle(filename);
    }

    auto doc = lunasvg::Document::loadFromFile(filename.string());
    if (!doc) {
        LOGW(log_tag::Asset, "load svg file from ", filename, " failed");
        return TextureHandle::Null();
    }
    auto bitmap = doc->renderToBitmap(size ? size->w : 0, size ? size->h : 0);
    bitmap.convertToRGBA();

    if (!bitmap.valid()) {
        return TextureHandle::Null();
    }

    auto handle = TextureHandle::Create();

    auto texture = std::make_unique<Texture>(
        (void*)bitmap.data(), bitmap.width(), bitmap.height(),
        gogl::Sampler::CreateLinearRepeat());
    texture->AssociateFile(filename);
    if (texture && *texture) {
        storeNewItem(handle, std::move(texture));
        return handle;
    }

    return TextureHandle::Null();
}

}  // namespace nickel