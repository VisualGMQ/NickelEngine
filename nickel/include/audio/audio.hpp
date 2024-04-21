#pragma once

#include "common/asset.hpp"
#include "common/cgmath.hpp"
#include "common/manager.hpp"
#include "common/filetype.hpp"

class ma_decoder;
class ma_sound;

namespace nickel {

class Sound : public Asset {
public:
    static Sound Null;

    Sound(const std::filesystem::path& filename);
    Sound(const Sound&) = delete;
    Sound(Sound&&) = default;
    Sound& operator=(const Sound&) = delete;
    Sound& operator=(Sound&&) = default;
    ~Sound();

    void* GetAudioData();

    bool operator==(const Sound& oth) const {
        return RelativePath() == oth.RelativePath();
    }

    bool operator!=(const Sound& oth) const { return !(*this == oth); }

    toml::table Save2Toml() const override {
        toml::table tbl;
        tbl.emplace("path", RelativePath().string());
        return tbl;
    }

    explicit operator bool() const {
        return data_ != nullptr;
    }

private:
    Sound() = default;
    ma_decoder* data_{};
};

using SoundHandle = Handle<Sound>;

class AudioManager : public Manager<Sound> {
public:
    static FileType GetFileType() { return FileType::Audio; }

    SoundHandle Load(const std::filesystem::path& filename) {
        auto sound = std::make_unique<Sound>(filename);
        if (!sound || !(*sound)) {
            return SoundHandle::Null();
        }

        auto handle = SoundHandle::Create();
        storeNewItem(handle, std::move(sound));
        return handle;
    }
};

class SoundPlayer {
public:
    static SoundPlayer Null;

    SoundPlayer(SoundHandle, AudioManager&);
    SoundPlayer() = default;

    SoundPlayer(const SoundPlayer&) = delete;
    SoundPlayer& operator=(const SoundPlayer&) = delete;
    SoundPlayer(SoundPlayer&& o) = default;
    SoundPlayer& operator=(SoundPlayer&& o) = default;

    auto Handle() const { return handle_; }
    void ChangeSound(SoundHandle, AudioManager&);

    void Play();

    void Stop();

    void Pause();

    void Rewind();

    void SetLoop(bool loop);

    bool IsPlaying() const;

    bool IsLooping() const;

    bool IsAtTheEnd() const;

    void SetStartTime(uint64_t millisecond);

    void SetStopTime(uint64_t millisecond);
    void SetVelocity(const cgmath::Vec3& v);

    cgmath::Vec3 GetVelocity() const;

    float Length();
    float GetCursor();
    void SetPosition(const cgmath::Vec3& pos);
    void SetPosition(const cgmath::Vec2& pos);
    void SetDirection(const cgmath::Vec3& dir);
    void SetDirection(const cgmath::Vec2& dir);
    cgmath::Vec3 GetDirToListener();
    float GetVolumn() const;
    void SetVolumn(float vol);
    cgmath::Vec3 GetDirection() const;
    cgmath::Vec3 GetPosition() const;
    explicit operator bool() const;

private:
    // static void destroySound(ma_sound* data) { ma_sound_uninit(data); }
    ma_sound* data_{};
    SoundHandle handle_;

    void recreateInnerSound(SoundHandle, AudioManager&);
};

void InitAudioSystem();
void ShutdownAudioSystem();

template <>
std::unique_ptr<Sound> LoadAssetFromMetaTable(const toml::table&);

}  // namespace nickel