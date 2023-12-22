#pragma once

#include "pch.hpp"
#include "core/asset.hpp"
#include "core/manager.hpp"

namespace nickel {

#define MA_CALL(expr)                                     \
    do {                                                  \
        if (auto err = (expr); err != MA_SUCCESS) {       \
            LOGW(log_tag::Audio, #expr, " faild: ", err); \
        }                                                 \
    } while (0)

class Sound final : public Asset {
public:
    Sound(const std::filesystem::path& rootPath,
          const std::filesystem::path& filename);
    ~Sound();

    void Play() { MA_CALL(ma_sound_start(&data_)); }

    void Stop() { MA_CALL(ma_sound_stop(&data_)); }

    void SetLoop(bool loop) { ma_sound_set_looping(&data_, true); }

    bool IsPlaying() const {
        return ma_sound_is_playing(&data_);
    }

    bool IsLooping() const {
        return ma_sound_is_looping(&data_);
    }

    static Sound Null;

    explicit operator bool() const { return data_.pDataSource != nullptr; }

    toml::table Save2Toml() const override {
        toml::table tbl;
        tbl.emplace("path", RelativePath().string());
        return tbl;
    }

    bool operator==(const Sound& oth) const {
        return RelativePath() == oth.RelativePath();
    }

    bool operator!=(const Sound& oth) const {
        return !(*this == oth);
    }

private:
    ma_sound data_{};

    Sound() = default;
};

using AudioHandle = Handle<Sound>;

class AudioManager : public Manager<Sound> {
public:
    AudioHandle Load(const std::filesystem::path& filename) {
        auto sound = std::make_unique<Sound>(GetRootPath(), filename);
        if (!sound || *sound == Sound::Null) {
            return AudioHandle::Null();
        }

        auto handle = AudioHandle::Create();
        storeNewItem(handle, std::move(sound));
        return handle;
    }
};

void InitAudioSystem();
void ShutdownAudioSystem();

template <>
std::unique_ptr<Sound> LoadAssetFromToml(const toml::table&,
                                           const std::filesystem::path& root);

}  // namespace nickel