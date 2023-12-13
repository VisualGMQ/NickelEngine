#pragma once

#include "misc/filetype.hpp"
#include "renderer/texture.hpp"
#include "renderer/font.hpp"

namespace nickel {

class AssetManager final {
public:
    AssetManager() = default;

    AssetManager(const std::filesystem::path& rootPath) {
        SetRootPath(rootPath);
    }

    void SetRootPath(const std::filesystem::path& path) {
        textureMgr_.SetRootPath(path);
        fontMgr_.SetRootPath(path);
    }

    auto& TextureMgr() { return textureMgr_; }
    auto& FontMgr() { return fontMgr_; }

    auto& TextureMgr() const { return textureMgr_; }
    auto& FontMgr() const { return fontMgr_; }

    void Load(const std::filesystem::path& path) {
        auto filetype = DetectFileType(path);
        switch (filetype) {
            case FileType::Image:
                TextureMgr().Load(path, gogl::Sampler::CreateLinearRepeat());
                break;
            case FileType::Font:
                FontMgr().Load(path);
                break;
            case FileType::Audio:
                // TODO: not finish
                break;
            default:
                LOGW(log_tag::Editor, "Unknown asset type: ", path);
                break;
        }
    }

    TextureHandle LoadTexture(const std::filesystem::path& path) {
        if (auto filetype = DetectFileType(path); filetype == FileType::Image) {
            return TextureMgr().Load(path, gogl::Sampler::CreateLinearRepeat());
        }
        return TextureHandle::Null();
    }

    TextureHandle LoadTexture(const std::filesystem::path& path,
                              const gogl::Sampler& sampler) {
        if (auto filetype = DetectFileType(path); filetype == FileType::Image) {
            return TextureMgr().Load(path, sampler);
        }
        return TextureHandle::Null();
    }

    FontHandle LoadFont(const std::filesystem::path& path) {
        if (auto filetype = DetectFileType(path); filetype == FileType::Font) {
            return FontMgr().Load(path);
        }
        return FontHandle::Null();
    }

    template <typename T>
    void Destroy(Handle<T> handle) {
        return switchManager<T>().Destroy(handle);
    }

    template <typename T>
    bool Has(Handle<T> handle) const {
        return switchManager<T>().Has(handle);
    }

    bool Has(const std::filesystem::path& filename) const {
        auto filetype = DetectFileType(filename);
        switch (filetype) {
            case FileType::Image:
                return TextureMgr().Has(filename);
            case FileType::Font:
                return FontMgr().Has(filename);
                break;
            case FileType::Audio:
                // TODO: not finish
                return false;
            default:
                return false;
        }
    }

    template <typename T>
    const T& Get(Handle<T> handle) const {
        return switchManager<T>().Get(handle);
    }

    template <typename T>
    T& Get(Handle<T> handle) {
        return const_cast<T&>(std::as_const(*this).Get(handle));
    }

    void ReleaseAll() {
        textureMgr_.ReleaseAll();
        fontMgr_.ReleaseAll();
    }

private:
    TextureManager textureMgr_;
    FontManager fontMgr_;

    template <typename T>
    const auto& switchManager() const {
        if constexpr (std::is_same_v<T, Texture>) {
            return textureMgr_;
        } else {
            return fontMgr_;
        }
    }

    template <typename T>
    auto& switchManager() {
        if constexpr (std::is_same_v<T, Texture>) {
            return textureMgr_;
        } else {
            return fontMgr_;
        }
    }
};

}  // namespace nickel