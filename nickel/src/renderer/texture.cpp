#include "renderer/texture.hpp"
#include "refl/sampler.hpp"

namespace nickel {

Texture Texture::Null = Texture{};

Texture::Texture(TextureHandle handle, const std::string& filename,
                 const gogl::Sampler& sampler, gogl::Format fmt,
                 gogl::Format gpuFmt)
    : handle_(handle), filename_(filename), sampler_(sampler) {
    stbi_uc* pixels =
        stbi_load(filename.c_str(), &w_, &h_, nullptr, STBI_rgb_alpha);
    if (pixels) {
        texture_ = std::make_unique<gogl::Texture>(
            gogl::Texture::Type::Dimension2, pixels, w_, h_, sampler, fmt,
            gpuFmt, gogl::DataType::UByte);
    } else {
        LOGE(log_tag::Res, "create texture from ", filename, " failed");
    }
}

Texture::Texture(TextureHandle handle, void* pixels, int w, int h,
                 const gogl::Sampler& sampler, gogl::Format fmt,
                 gogl::Format gpuFmt)
    : handle_(handle), sampler_(sampler), w_(w), h_(h) {
    texture_ = std::make_unique<gogl::Texture>(
        gogl::Texture::Type::Dimension2, pixels, w, h, sampler, fmt,
        gpuFmt, gogl::DataType::UByte);
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

TextureHandle TextureManager::Load(const std::string& filename,
                                   const gogl::Sampler& sampler) {
    TextureHandle handle = TextureHandle::Create();
    auto texture = std::unique_ptr<Texture>(
        new Texture{handle, addRootPath(filename), sampler});
    if (texture) {
        storeNewItem(handle, std::move(texture));
        return handle;
    } else {
        return TextureHandle::Null();
    }
}

std::unique_ptr<Texture> TextureManager::CreateSolitary(
    void* data, int w, int h, const gogl::Sampler& sampler, gogl::Format fmt,
    gogl::Format gpuFmt) {
    return std::unique_ptr<Texture>(
        new Texture{TextureHandle::Null(), data, w, h, sampler, fmt, gpuFmt});
}

toml::table TextureManager::Save2Toml() const {
    toml::table tbl;

    tbl.emplace("type", std::string_view("texture"));
    tbl.emplace("root_path", GetRootPath());
    toml::array arr;
    for (auto& [handle, filename] : associateFiles_) {
        const Texture& texture = Get(handle);
        toml::table textureTbl;
        textureTbl.emplace("id", (HandleInnerIDType)handle);
        if (!texture.Filename().empty()) {
            textureTbl.emplace("filename", texture.Filename());
        }
        toml::table samplerTbl;
        ::mirrow::serd::srefl::serialize<gogl::Sampler>(texture.Sampler(),
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
                             handle, filename,
                             ::nickel::gogl::Sampler::CreateLinearRepeat())));
        }
    }
}

}  // namespace nickel