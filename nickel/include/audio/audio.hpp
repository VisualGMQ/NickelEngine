#pragma once

#include "common/asset.hpp"
#include "common/cgmath.hpp"


class ma_decoder;
class ma_sound;

namespace nickel {

class Sound : public Asset {
public:
    static Sound Null;

    Sound() = default;
    Sound(const Sound&) = delete;

    Sound(Sound&& o) { swap(*this, o); }

    Sound& operator=(const Sound&) = delete;

    Sound& operator=(Sound&& o) {
        if (this != &o) {
            swap(*this, o);
        }
        return *this;
    }

    ~Sound();

    void* GetAudioData();

    bool operator==(const Sound& oth) const {
        return GetRelativePath() == oth.GetRelativePath();
    }

    bool operator!=(const Sound& oth) const { return !(*this == oth); }

    bool Load(const std::filesystem::path&) override;
    bool Load(const toml::table& tbl) override;
    bool Save(toml::table& tbl) const override;

    explicit operator bool() const { return data_ != nullptr; }

private:
    ma_decoder* data_{};

    bool load(const std::filesystem::path&);

    friend void swap(Sound& o1, Sound& o2) {
        using std::swap;
        swap(o1.data_, o2.data_);
    }
};

using SoundHandle = Handle<Sound>;

class SoundPlayer {
public:
    SoundPlayer();
    explicit SoundPlayer(SoundHandle);
    SoundPlayer(const SoundPlayer&) = delete;
    SoundPlayer& operator=(const SoundPlayer&) = delete;

    SoundPlayer(SoundPlayer&& o) { swap(o, *this); }
    ~SoundPlayer();

    SoundPlayer& operator=(SoundPlayer&& o) {
        if (&o != this) {
            swap(*this, o);
        }
        return *this;
    }

    auto Handle() const { return handle_; }

    void ChangeSound(SoundHandle);

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

    void recreateInnerSound(SoundHandle);

    friend void swap(SoundPlayer& o1, SoundPlayer& o2) noexcept {
        using std::swap;
        swap(o1.data_, o2.data_);
        swap(o1.handle_, o2.handle_);
    }
};

void InitAudioSystem();
void ShutdownAudioSystem();

}  // namespace nickel