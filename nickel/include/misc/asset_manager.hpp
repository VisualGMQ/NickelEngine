#pragma once

#include "anim/anim.hpp"
#include "audio/audio.hpp"
#include "common/filetype.hpp"
#include "common/timer.hpp"
#include "common/util.hpp"
#include "graphics/font.hpp"
#include "graphics/gltf.hpp"
#include "graphics/texture.hpp"
#include "graphics/tilesheet.hpp"
#include "audio/audio.hpp"
#include "script/script.hpp"
#include "common/util.hpp"
#include "script/script.hpp"

namespace nickel {

class AssetManager final {
public:
    AssetManager(TextureManager& texture, Material2DManager& mtl, FontManager& font,
                 TimerManager& timer, TilesheetManager& tilesheet,
                 AnimationManager& anim, AudioManager& audio, GLTFManager& gltf, ScriptManager& script)
        : mgrs_{texture, mtl, font, timer, tilesheet, anim, audio, gltf, script} {}

    auto& TextureMgr() { return std::get<TextureManager&>(mgrs_); }

    auto& FontMgr() { return std::get<FontManager&>(mgrs_); }

    auto& TilesheetMgr() { return std::get<TilesheetManager&>(mgrs_); }

    auto& AnimationMgr() { return std::get<AnimationManager&>(mgrs_); }

    auto& TimerMgr() { return std::get<TimerManager&>(mgrs_); }

    auto& AudioMgr() { return std::get<AudioManager&>(mgrs_); }

    auto& ScriptMgr() { return std::get<ScriptManager&>(mgrs_); }

    auto& TextureMgr() const { return std::get<TextureManager&>(mgrs_); }

    auto& FontMgr() const { return std::get<FontManager&>(mgrs_); }

    auto& TilesheetMgr() const { return std::get<TilesheetManager&>(mgrs_); }

    auto& AnimationMgr() const { return std::get<AnimationManager&>(mgrs_); }

    auto& TimerMgr() const { return std::get<TimerManager&>(mgrs_); }

    auto& AudioMgr() const { return std::get<AudioManager&>(mgrs_); }

    auto& GLTFMgr() const { return std::get<GLTFManager&>(mgrs_); }

    auto& Material2DMgr() const { return std::get<Material2DManager&>(mgrs_); }

    auto& ScriptMgr() const { return std::get<ScriptManager&>(mgrs_); }

    bool Load(const std::filesystem::path& path) {
        auto filetype = DetectFileType(path);
        bool success = false;

        VisitTuple(mgrs_, [=, &path, &success](auto&& mgr) {
            if (mgr.GetFileType() == filetype) {
                success = success || mgr.Load(path);
            }
        });

        return success;
    }

    void LoadFromMeta(const std::filesystem::path& path) {
        auto filetype = DetectFileType(StripMetaExtension(path));
        VisitTuple(mgrs_, [=, &path](auto&& mgr) {
            if (mgr.GetFileType() == filetype) {
                mgr.LoadAssetFromMeta(path);
            }
        });
    }

    TextureHandle LoadTexture(
        const std::filesystem::path& path,
        rhi::TextureFormat gpuFmt = rhi::TextureFormat::RGBA8_UNORM_SRGB) {
        if (auto filetype = DetectFileType(path); filetype == FileType::Image) {
            return TextureMgr().Load(path, gpuFmt);
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

    SoundHandle LoadAudio(const std::filesystem::path& path) {
        if (auto filetype = DetectFileType(path); filetype == FileType::Audio) {
            return AudioMgr().Load(path);
        }
        return {};
    }

    GLTFHandle LoadGLTF(const std::filesystem::path& path) {
        if (auto filetype = DetectFileType(path); filetype == FileType::GLTF) {
            return GLTFMgr().Load(path);
        }
        return {};
    }

    Material2DHandle LoadMaterial2D(const std::filesystem::path& path) {
        if (auto filetype = DetectFileType(path); filetype == FileType::Material2D) {
            return Material2DMgr().Load(path);
        }
        return {};
    }

    template <typename T>
    void Destroy(Handle<T> handle) {
        return SwitchManager<T>().Destroy(handle);
    }

    void Destroy(const std::filesystem::path& path) {
        auto filetype = DetectFileType(path);

        VisitTuple(mgrs_, [=, &path](auto&& mgr) {
            if (mgr.GetFileType() == filetype) {
                mgr.Destroy(path);
            }
        });
    }

    template <typename T>
    void AssociateFile(Handle<T> handle, const std::filesystem::path& path) {
        SwitchManager<T>().AssociateFile(handle, path);
    }

    template <typename T>
    bool Has(Handle<T> handle) const {
        return SwitchManager<T>().Has(handle);
    }

    bool Has(const std::filesystem::path& filename) const {
        auto filetype = DetectFileType(filename);

        bool has = false;

        VisitTuple(mgrs_, [=, &filename, &has](auto&& mgr) {
            if (mgr.GetFileType() == filetype) {
                has = has || mgr.Has(filename);
            }
        });

        return has;
    }

    template <typename T>
    const T& Get(Handle<T> handle) const {
        return SwitchManager<T>().Get(handle);
    }

    template <typename T>
    T& Get(Handle<T> handle) {
        return const_cast<T&>(std::as_const(*this).Get(handle));
    }

    void ReleaseAll() {
        VisitTuple(mgrs_, [](auto&& mgr) { mgr.ReleaseAll(); });
    }

    void Save2TomlFile(const std::filesystem::path& rootDir,
                       const std::filesystem::path& filename) const {
        std::ofstream file(filename);
        file << Save2Toml(rootDir);
    }

    void SaveAssets2File() const {
        TextureMgr().SaveAssets2File();
        Material2DMgr().SaveAssets2File();
        FontMgr().SaveAssets2File();
        AnimationMgr().SaveAssets2File();
        TilesheetMgr().SaveAssets2File();
        TimerMgr().SaveAssets2File();
        AudioMgr().SaveAssets2File();
        GLTFMgr().SaveAssets2File();
        ScriptMgr().SaveAssets2File();
    }

    toml::table Save2Toml(const std::filesystem::path& rootDir) const {
        toml::table tbl;

        tbl.emplace("texture", TextureMgr().Save2Toml(rootDir));
        tbl.emplace("material2d", Material2DMgr().Save2Toml(rootDir));
        tbl.emplace("font", FontMgr().Save2Toml(rootDir));
        tbl.emplace("anim", AnimationMgr().Save2Toml(rootDir));
        tbl.emplace("tilesheet", TilesheetMgr().Save2Toml(rootDir));
        tbl.emplace("timer", TimerMgr().Save2Toml(rootDir));
        tbl.emplace("audio", AudioMgr().Save2Toml(rootDir));
        tbl.emplace("gltf", GLTFManager().Save2Toml(rootDir));
        tbl.emplace("script", ScriptMgr().Save2Toml(rootDir));

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
        if (auto node = tbl.get("audio"); node && node->is_table()) {
            AudioMgr().LoadFromToml(*node->as_table());
        }
        if (auto node = tbl.get("gltf"); node && node->is_table()) {
            GLTFMgr().LoadFromToml(*node->as_table());
        }
        if (auto node = tbl.get("material2d"); node && node->is_table()) {
            Material2DMgr().LoadFromToml(*node->as_table());
        }
        if (auto node = tbl.get("script"); node && node->is_table()) {
            ScriptMgr().LoadFromToml(*node->as_table());
        }
    }

    auto& Managers() const { return mgrs_; }

    auto& Managers() { return mgrs_; }

    template <typename T>
    auto& SwitchManager() const {
        if constexpr (std::is_same_v<T, Texture>) {
            return std::get<TextureManager&>(mgrs_);
        } else if constexpr (std::is_same_v<T, Font>) {
            return std::get<FontManager&>(mgrs_);
        } else if constexpr (std::is_same_v<T, Timer>) {
            return std::get<TimerManager&>(mgrs_);
        } else if constexpr (std::is_same_v<T, Tilesheet>) {
            return std::get<TilesheetManager&>(mgrs_);
        } else if constexpr (std::is_same_v<T, Animation>) {
            return std::get<AnimationManager&>(mgrs_);
        } else if constexpr (std::is_same_v<T, Sound>) {
            return std::get<AudioManager&>(mgrs_);
        } else if constexpr (std::is_same_v<T, GLTFModel>) {
            return std::get<GLTFManager&>(mgrs_);
        } else if constexpr (std::is_same_v<T, Material2D>) {
            return std::get<Material2DManager&>(mgrs_);
        } else if constexpr (std::is_same_v<T, typename ScriptManager::AssetType>) {
            return std::get<ScriptManager&>(mgrs_);
        }
    }

    template <typename T>
    auto& SwitchManager() {
        using type = decltype(std::declval<const AssetManager>()
                                  .template SwitchManager<T>());

        using refType = std::remove_const_t<std::remove_reference_t<type>>&;
        return const_cast<refType>(
            std::as_const(*this).template SwitchManager<T>());
    }

private:
    std::tuple<TextureManager&, Material2DManager&, FontManager&, TimerManager&, TilesheetManager&,
               AnimationManager&, AudioManager&, GLTFManager&, ScriptManager&>
        mgrs_;
};

}  // namespace nickel