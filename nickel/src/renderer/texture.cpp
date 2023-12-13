#include "renderer/texture.hpp"
#include "refl/sampler.hpp"

namespace nickel {

Texture Texture::Null = Texture{};

Texture::Texture(TextureHandle handle, const std::filesystem::path& root,
                 const std::filesystem::path& filename,
                 const gogl::Sampler& sampler, gogl::Format fmt,
                 gogl::Format gpuFmt)
    : Asset(filename), handle_(handle), sampler_(sampler) {
    stbi_uc* pixels = stbi_load((root / filename).string().c_str(), &w_, &h_,
                                nullptr, STBI_rgb_alpha);
    if (pixels) {
        texture_ = std::make_unique<gogl::Texture>(
            gogl::Texture::Type::Dimension2, pixels, w_, h_, sampler, fmt,
            gpuFmt, gogl::DataType::UByte);
    } else {
        LOGE(log_tag::Asset, "create texture from ", filename, " failed");
    }
}

Texture::Texture(TextureHandle handle, const std::filesystem::path& filename,
                 void* pixels, int w, int h, const gogl::Sampler& sampler,
                 gogl::Format fmt, gogl::Format gpuFmt)
    : Asset(filename), handle_(handle), sampler_(sampler), w_(w), h_(h) {
    texture_ = std::make_unique<gogl::Texture>(gogl::Texture::Type::Dimension2,
                                               pixels, w, h, sampler, fmt,
                                               gpuFmt, gogl::DataType::UByte);
}

TextureHandle TextureManager::Load(const std::filesystem::path& filename,
                                   const gogl::Sampler& sampler) {
    auto relativePath = filename.is_relative() ? filename
                                               : std::filesystem::relative(
                                                     filename, GetRootPath());
    if (Has(relativePath)) {
        return GetHandle(relativePath);
    }

    TextureHandle handle = TextureHandle::Create();
    auto texture = std::unique_ptr<Texture>(
        new Texture(handle, GetRootPath(), relativePath, sampler));
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

    Texture texture{handle, GetRootPath(), filename, sampler};
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
    auto texture = std::unique_ptr<Texture>(new Texture{
        TextureHandle::Null(), {}, data, w, h, sampler, fmt, gpuFmt});
    if (texture && *texture) {
        return std::move(texture);
    } else {
        return nullptr;
    }
}

toml::table TextureManager::Save2Toml() const {
    toml::table tbl;

    tbl.emplace("root_path", GetRootPath().string());
    toml::array arr;
    for (auto& [handle, texture] : AllDatas()) {
        arr.push_back(serializeTexture(*texture));
    }
    tbl.emplace("textures", arr);

    return tbl;
}

void TextureManager::LoadFromToml(toml::table& tbl) {
    if (auto root = tbl["root_path"]; root.is_string()) {
        SetRootPath(root.as_string()->get());
    }

    if (auto datas = tbl["textures"]; datas.is_array()) {
        for (auto& node : *datas.as_array()) {
            if (!node.is_table()) {
                continue;
            }

            auto& textureTbl = *node.as_table();
            deserializeTexture(textureTbl);
        }
    }
}

TextureHandle TextureManager::LoadSVG(const std::filesystem::path& filename,
                                      const gogl::Sampler&,
                                      std::optional<cgmath::Vec2> size) {
    auto relativePath = convert2RelativePath(filename);
    if (Has(relativePath)) {
        return GetHandle(relativePath);
    }

    auto doc =
        lunasvg::Document::loadFromFile((GetRootPath() / relativePath).string());
    auto bitmap = doc->renderToBitmap(size ? size->w : 0, size ? size->h : 0);
    bitmap.convertToRGBA();

    if (!bitmap.valid()) {
        return TextureHandle::Null();
    }

    auto handle = TextureHandle::Create();

    auto texture = std::unique_ptr<Texture>(
        new Texture(handle, filename, (void*)bitmap.data(), bitmap.width(),
                    bitmap.height(), gogl::Sampler::CreateLinearRepeat()));
    if (texture && *texture) {
        storeNewItem(handle, std::move(texture));
        return handle;
    }

    return TextureHandle::Null();
}

toml::table TextureManager::serializeTexture(const Texture& texture) const {
    toml::table tbl;
    tbl.emplace("path", texture.RelativePath().string());
    mirrow::serd::drefl::serialize(
        tbl, mirrow::drefl::any_make_constref(texture.Sampler()),
        mirrow::drefl::typeinfo<gogl::Sampler>()->name());
    return tbl;
}

bool TextureManager::deserializeTexture(const toml::table& tbl) {
    std::filesystem::path filename;

    if (auto path = tbl.get("path"); !path || !path->is_string()) {
        LOGW(log_tag::Asset,
             "deserialize texture failed! `path` field not string");
        return false;
    } else {
        filename = path->as_string()->get();
    }

    gogl::Sampler sampler;
    auto samplerTypeinfo = mirrow::drefl::typeinfo<gogl::Sampler>();
    if (!tbl.contains(samplerTypeinfo->name())) {
        LOGW(log_tag::Asset,
             "deserialize texture failed! `sampler` field not table");
        return false;
    } else {
        auto ref = mirrow::drefl::any_make_ref(sampler);
        mirrow::serd::drefl::deserialize(ref, tbl);
    }

    return Load(filename, sampler) != TextureHandle::Null();
}

}  // namespace nickel