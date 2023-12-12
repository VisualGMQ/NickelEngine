#include "renderer/texture.hpp"
#include "refl/sampler.hpp"

namespace nickel {

Texture Texture::Null = Texture{};

Texture::Texture(TextureHandle handle, const std::filesystem::path& root,
                 const std::filesystem::path& filename,
                 const gogl::Sampler& sampler, gogl::Format fmt,
                 gogl::Format gpuFmt)
    : Res(filename), handle_(handle), sampler_(sampler) {
    stbi_uc* pixels = stbi_load((root / filename).string().c_str(), &w_, &h_,
                                nullptr, STBI_rgb_alpha);
    if (pixels) {
        texture_ = std::make_unique<gogl::Texture>(
            gogl::Texture::Type::Dimension2, pixels, w_, h_, sampler, fmt,
            gpuFmt, gogl::DataType::UByte);
    } else {
        LOGE(log_tag::Res, "create texture from ", filename, " failed");
    }
}

Texture::Texture(TextureHandle handle, const std::filesystem::path& filename,
                 void* pixels, int w, int h, const gogl::Sampler& sampler,
                 gogl::Format fmt, gogl::Format gpuFmt)
    : Res(filename), handle_(handle), sampler_(sampler), w_(w), h_(h) {
    texture_ = std::make_unique<gogl::Texture>(gogl::Texture::Type::Dimension2,
                                               pixels, w, h, sampler, fmt,
                                               gpuFmt, gogl::DataType::UByte);
}

TextureHandle TextureManager::Load(const std::filesystem::path& filename,
                                   const gogl::Sampler& sampler) {
    TextureHandle handle = TextureHandle::Create();
    auto relativePath = filename.is_relative() ? filename
                                               : std::filesystem::relative(
                                                     filename, GetRootPath());
    auto texture = std::unique_ptr<Texture>(
        new Texture(handle, GetRootPath(), relativePath, sampler));
    if (texture) {
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
    return std::unique_ptr<Texture>(new Texture{
        TextureHandle::Null(), {}, data, w, h, sampler, fmt, gpuFmt});
}

toml::table TextureManager::Save2Toml() const {
    toml::table tbl;

    tbl.emplace("type", std::string_view("texture"));
    tbl.emplace("root_path", GetRootPath().string());
    toml::array arr;
    for (auto& [handle, texture] : AllDatas()) {
        toml::table textureTbl;
        textureTbl.emplace("id", (HandleInnerIDType)handle);
        if (!texture->RelativePath().empty()) {
            textureTbl.emplace("filename", texture->RelativePath().string());
        }
        toml::table samplerTbl;
        ::mirrow::serd::srefl::serialize<gogl::Sampler>(texture->Sampler(),
                                                        samplerTbl);
        textureTbl.emplace("sampler", samplerTbl);
        arr.push_back(textureTbl);
    }
    tbl.emplace("datas", arr);

    return tbl;
}

void TextureManager::LoadFromToml(toml::table& tbl) {
    if (!(tbl["type"].is_string() && tbl["type"] == "texture")) {
        LOGW(::nickel::log_tag::Res, "assets manager type don't suitable");
        return;
    }

    if (auto root = tbl["root_path"]; root.is_string()) {
        SetRootPath(root.as_string()->get());
    }

    if (auto datas = tbl["datas"]; datas.is_array()) {
        for (auto& node : *datas.as_array()) {
            Assert(node.is_table(), "texture info record node is not a table");
            auto& textureTbl = *node.as_table();

            auto handleId = textureTbl["id"].as_integer()->get();
            TextureHandle handle = TextureHandle::ForceCastFromIntegral(
                static_cast<HandleInnerIDType>(handleId));
            std::string filename = textureTbl["filename"].as_string()->get();
            gogl::Sampler sampler = gogl::Sampler::CreateLinearRepeat();
            if (auto node = textureTbl["sampler"]; node.is_table()) {
                mirrow::serd::srefl::deserialize(*node.as_table(), sampler);
            }
            storeNewItem(handle,
                         std::unique_ptr<Texture>(new Texture(
                             handle, GetRootPath(), filename,
                             ::nickel::gogl::Sampler::CreateLinearRepeat())));
        }
    }
}

TextureHandle TextureManager::LoadSVG(const std::filesystem::path& filename,
                                      const gogl::Sampler&,
                                      std::optional<cgmath::Vec2> size) {
    auto doc =
        lunasvg::Document::loadFromFile((GetRootPath() / filename).string());
    auto bitmap = doc->renderToBitmap(size ? size->w : 0, size ? size->h : 0);
    bitmap.convertToRGBA();

    auto handle = TextureHandle::Create();

    auto texture = std::unique_ptr<Texture>(
        new Texture(handle, filename, (void*)bitmap.data(), bitmap.width(),
                    bitmap.height(), gogl::Sampler::CreateLinearRepeat()));
    storeNewItem(handle, std::move(texture));

    return handle;
    return {};
}

}  // namespace nickel