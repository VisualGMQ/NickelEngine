#pragma once

#include "anim/anim.hpp"
#include "misc/filetype.hpp"
#include "misc/timer.hpp"
#include "renderer/font.hpp"
#include "renderer/texture.hpp"
#include "renderer/tilesheet.hpp"

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
        timerMgr_.SetRootPath(path);
        tilesheetMgr_.SetRootPath(path);
        animMgr_.SetRootPath(path);
    }

    auto& TextureMgr() { return textureMgr_; }

    auto& FontMgr() { return fontMgr_; }

    auto& TilesheetMgr() { return tilesheetMgr_; }

    auto& AnimationMgr() { return animMgr_; }

    auto& TimerMgr() { return timerMgr_; }

    auto& TextureMgr() const { return textureMgr_; }

    auto& FontMgr() const { return fontMgr_; }

    auto& TilesheetMgr() const { return tilesheetMgr_; }

    auto& AnimationMgr() const { return animMgr_; }

    auto& TimerMgr() const { return timerMgr_; }

    bool Load(const std::filesystem::path& path) {
        auto filetype = DetectFileType(path);
        switch (filetype) {
            case FileType::Image:
                return TextureMgr().Load(path,
                                         gogl::Sampler::CreateLinearRepeat()) !=
                       TextureHandle::Null();
            case FileType::Font:
                return FontMgr().Load(path) != FontHandle::Null();
            case FileType::Audio:
                // TODO: not finish
                return false;
            case FileType::Tilesheet:
                return TilesheetMgr().Load(path) != TilesheetHandle::Null();
            case FileType::Animation:
                // return AnimationMgr().Load(path) != TilesheetHandle::Null();
            case FileType::Timer:
                return TimerMgr().Load(path) != TimerHandle::Null();
            default:
                LOGW(log_tag::Editor, "Unknown asset type: ", path);
                return false;
        }
    }

    TextureHandle LoadTexture(const std::filesystem::path& path) {
        if (auto filetype = DetectFileType(path); filetype == FileType::Image) {
            return TextureMgr().Load(path, gogl::Sampler::CreateLinearRepeat());
        }
        return {};
    }

    TextureHandle LoadTexture(const std::filesystem::path& path,
                              const gogl::Sampler& sampler) {
        if (auto filetype = DetectFileType(path); filetype == FileType::Image) {
            return TextureMgr().Load(path, sampler);
        }
        return {};
    }

    FontHandle LoadFont(const std::filesystem::path& path) {
        if (auto filetype = DetectFileType(path); filetype == FileType::Font) {
            return FontMgr().Load(path);
        }
        return {};
    }

    TimerHandle LoadTimer(const std::filesystem::path& path) {
        if (auto filetype = DetectFileType(path); filetype == FileType::Timer) {
            return TimerMgr().Load(path);
        }
        return {};
    }

    TilesheetHandle LoadTilesheet(const std::filesystem::path& path) {
        if (auto filetype = DetectFileType(path);
            filetype == FileType::Tilesheet) {
            return TilesheetMgr().Load(path);
        }
        return {};
    }

    AnimationHandle LoadAnimation(const std::filesystem::path& path) {
        if (auto filetype = DetectFileType(path);
            filetype == FileType::Animation) {
            return AnimationMgr().Load(path);
        }
        return {};
    }

    template <typename T>
    void Destroy(Handle<T> handle) {
        return switchManager<T>().Destroy(handle);
    }

    void Destroy(const std::filesystem::path& path) {
        auto filetype = DetectFileType(path);

        switch (filetype) {
            case FileType::Image:
                TextureMgr().Destroy(path);
                break;
            case FileType::Font:
                FontMgr().Destroy(path);
                break;
            case FileType::Audio:
                // TODO: not finish
                break;
            case FileType::Tilesheet:
                TilesheetMgr().Destroy(path);
                break;
            case FileType::Animation:
                AnimationMgr().Destroy(path);
                break;
            case FileType::Timer:
                TimerMgr().Destroy(path);
                break;
            case FileType::Unknown:
            case FileType::FileTypeCount:
                break;
        }
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
            case FileType::Animation:
                return AnimationMgr().Has(filename);
            case FileType::Timer:
                return TimerMgr().Has(filename);
            case FileType::Tilesheet:
                return TilesheetMgr().Has(filename);
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
        animMgr_.ReleaseAll();
        timerMgr_.ReleaseAll();
        tilesheetMgr_.ReleaseAll();
    }

    void Save2TomlFile(const std::filesystem::path& rootDir,
                       const std::filesystem::path& filename) const {
        std::ofstream file(filename);
        file << Save2Toml(rootDir);
    }

    toml::table Save2Toml(const std::filesystem::path& rootDir) const {
        toml::table tbl;

        tbl.emplace("texture", TextureMgr().Save2Toml(rootDir));
        tbl.emplace("font", FontMgr().Save2Toml(rootDir));
        tbl.emplace("anim", AnimationMgr().Save2Toml(rootDir));
        tbl.emplace("tilesheet", TilesheetMgr().Save2Toml(rootDir));
        tbl.emplace("timer", TimerMgr().Save2Toml(rootDir));

        return tbl;
    }

    void LoadFromToml(const toml::table& tbl) {
        if (auto node = tbl.get("texture"); node && node->is_table()) {
            TextureMgr().LoadFromToml(*node->as_table());
        }
        if (auto node = tbl.get("font"); node && node->is_table()) {
            FontMgr().LoadFromToml(*node->as_table());
        }
        if (auto node = tbl.get("anim"); node && node->is_table()) {
            AnimationMgr().LoadFromToml(*node->as_table());
        }
        if (auto node = tbl.get("tilesheet"); node && node->is_table()) {
            TilesheetMgr().LoadFromToml(*node->as_table());
        }
        if (auto node = tbl.get("timer"); node && node->is_table()) {
            TimerMgr().LoadFromToml(*node->as_table());
        }
    }

    void LoadFromTomlWithPath(const toml::table& tbl, const std::filesystem::path& configDir) {
        if (auto node = tbl.get("texture"); node && node->is_table()) {
            TextureMgr().LoadFromTomlWithPath(*node->as_table(), configDir);
        }
        if (auto node = tbl.get("font"); node && node->is_table()) {
            FontMgr().LoadFromTomlWithPath(*node->as_table(), configDir);
        }
        if (auto node = tbl.get("anim"); node && node->is_table()) {
            AnimationMgr().LoadFromTomlWithPath(*node->as_table(), configDir);
        }
        if (auto node = tbl.get("tilesheet"); node && node->is_table()) {
            TilesheetMgr().LoadFromTomlWithPath(*node->as_table(), configDir);
        }
        if (auto node = tbl.get("timer"); node && node->is_table()) {
            TimerMgr().LoadFromTomlWithPath(*node->as_table(), configDir);
        }
    }

private:
    TextureManager textureMgr_;
    FontManager fontMgr_;
    TimerManager timerMgr_;
    TilesheetManager tilesheetMgr_;
    AnimationManager animMgr_;

    template <typename T>
    auto& switchManager() const {
        if constexpr (std::is_same_v<T, Texture>) {
            return textureMgr_;
        } else if constexpr (std::is_same_v<T, Font>) {
            return fontMgr_;
        } else if constexpr (std::is_same_v<T, Timer>) {
            return timerMgr_;
        } else if constexpr (std::is_same_v<T, Tilesheet>) {
            return tilesheetMgr_;
        } else if constexpr (std::is_same_v<T, Animation>) {
            return animMgr_;
        }
    }

    template <typename T>
    auto& switchManager() {
        if constexpr (std::is_same_v<T, Texture>) {
            return textureMgr_;
        } else if constexpr (std::is_same_v<T, Font>) {
            return fontMgr_;
        } else if constexpr (std::is_same_v<T, Timer>) {
            return timerMgr_;
        } else if constexpr (std::is_same_v<T, Tilesheet>) {
            return tilesheetMgr_;
        } else if constexpr (std::is_same_v<T, Animation>) {
            return animMgr_;
        }
    }
};

}  // namespace nickel