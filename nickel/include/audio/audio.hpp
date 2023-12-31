#pragma once

#include "core/asset.hpp"
#include "core/cgmath.hpp"
#include "core/manager.hpp"
#include "misc/filetype.hpp"
#include "pch.hpp"

namespace nickel {

class Sound : public Asset {
public:
    static Sound Null;

    Sound(const std::filesystem::path& filename);
    Sound(const toml::table&);
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
    std::unique_ptr<ma_decoder> data_ = std::make_unique<ma_decoder>();
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

#define MA_CALL(expr)                                     \
    do {                                                  \
        if (auto err = (expr); err != MA_SUCCESS) {       \
            LOGW(log_tag::Audio, #expr, " faild: ", err); \
        }                                                 \
    } while (0)

class SoundPlayer {
public:
    static SoundPlayer Null;

    explicit SoundPlayer(SoundHandle);
    SoundPlayer() = default;

    SoundPlayer(const SoundPlayer&) = delete;
    SoundPlayer& operator=(const SoundPlayer&) = delete;
    SoundPlayer(SoundPlayer&& o) = default;
    SoundPlayer& operator=(SoundPlayer&& o) = default;

    auto Handle() const { return handle_; }
    void ChangeSound(SoundHandle);

    void Play() { MA_CALL(ma_sound_start(data_.get())); }

    void Stop() { MA_CALL(ma_sound_stop(data_.get())); }

    void Pause() { MA_CALL(ma_sound_stop(data_.get())); }

    void Rewind() { MA_CALL(ma_sound_seek_to_pcm_frame(data_.get(), 0)); }

    void SetLoop(bool loop) { ma_sound_set_looping(data_.get(), true); }

    bool IsPlaying() const { return ma_sound_is_playing(data_.get()); }

    bool IsLooping() const { return ma_sound_is_looping(data_.get()); }

    bool IsAtTheEnd() const { return ma_sound_at_end(data_.get()); }

    void SetStartTime(uint64_t millisecond) {
        ma_sound_set_start_time_in_milliseconds(data_.get(), millisecond);
    }

    void SetStopTime(uint64_t millisecond) {
        ma_sound_set_stop_time_in_milliseconds(data_.get(), millisecond);
    }

    void SetVelocity(const cgmath::Vec3& v) {
        ma_sound_set_velocity(data_.get(), v.x, v.y, v.z);
    }

    cgmath::Vec3 GetVelocity() const {
        auto vel = ma_sound_get_velocity(data_.get());
        return {vel.x, vel.y, vel.z};
    }

    float Length() {
        float len;
        MA_CALL(ma_sound_get_length_in_seconds(data_.get(), &len));
        return len;
    }

    float GetCursor() {
        float cursor;
        MA_CALL(ma_sound_get_cursor_in_seconds(data_.get(), &cursor));
        return cursor;
    }

    void SetPosition(const cgmath::Vec3& pos) {
        ma_sound_set_position(data_.get(), pos.x, pos.y, pos.z);
    }

    void SetPosition(const cgmath::Vec2& pos) {
        ma_sound_set_position(data_.get(), pos.x, 0, pos.y);
    }

    void SetDirection(const cgmath::Vec3& dir) {
        ma_sound_set_direction(data_.get(), dir.x, dir.y, dir.z);
    }

    void SetDirection(const cgmath::Vec2& dir) {
        ma_sound_set_direction(data_.get(), dir.x, 0, dir.y);
    }

    cgmath::Vec3 GetDirToListener() {
        auto dir = ma_sound_get_direction_to_listener(data_.get());
        return {dir.x, dir.y, dir.z};
    }

    float GetVolumn() const { return ma_sound_get_volume(data_.get()); }

    void SetVolumn(float vol) { ma_sound_set_volume(data_.get(), vol); }

    cgmath::Vec3 GetDirection() const {
        auto dir = ma_sound_get_direction(data_.get());
        return {dir.x, dir.y, dir.z};
    }

    cgmath::Vec3 GetPosition() const {
        auto pos = ma_sound_get_position(data_.get());
        return {pos.x, pos.y, pos.z};
    }

    explicit operator bool() const {
        return data_ && data_->pDataSource != nullptr;
    }

private:
    static void destroySound(ma_sound* data) { ma_sound_uninit(data); }

    std::unique_ptr<ma_sound, void (*)(ma_sound*)> data_{new ma_sound{0},
                                                         destroySound};
    SoundHandle handle_;

    void recreateInnerSound(SoundHandle);
};

#undef MA_CALL

void InitAudioSystem();
void ShutdownAudioSystem();

template <>
std::unique_ptr<Sound> LoadAssetFromMeta(const toml::table&);

}  // namespace nickel