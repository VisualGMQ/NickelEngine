#pragma once

#include "core/asset.hpp"
#include "core/cgmath.hpp"
#include "core/manager.hpp"
#include "misc/filetype.hpp"
#include "pch.hpp"

namespace nickel {

#define MA_CALL(expr)                                     \
    do {                                                  \
        if (auto err = (expr); err != MA_SUCCESS) {       \
            LOGW(log_tag::Audio, #expr, " faild: ", err); \
        }                                                 \
    } while (0)

class Sound final : public Asset {
public:
    static Sound Null;

    explicit Sound(const toml::table&);
    explicit Sound(const std::filesystem::path& filename);
    ~Sound();

    Sound(const Sound&) = delete;
    Sound(Sound&&) = default;
    Sound& operator=(const Sound&) = delete;
    Sound& operator=(Sound&&) = default;

    void Play() { MA_CALL(ma_sound_start(&data_)); }

    void Stop() { MA_CALL(ma_sound_stop(&data_)); }

    void Pause() { MA_CALL(ma_sound_stop(&data_)); }

    void Rewind() { MA_CALL(ma_sound_seek_to_pcm_frame(&data_, 0)); }

    void SetLoop(bool loop) { ma_sound_set_looping(&data_, true); }

    bool IsPlaying() const { return ma_sound_is_playing(&data_); }

    bool IsLooping() const { return ma_sound_is_looping(&data_); }

    bool IsAtTheEnd() const { return ma_sound_at_end(&data_); }

    void SetStartTime(uint64_t millisecond) {
        ma_sound_set_start_time_in_milliseconds(&data_, millisecond);
    }

    void SetStopTime(uint64_t millisecond) {
        ma_sound_set_stop_time_in_milliseconds(&data_, millisecond);
    }

    void SetVelocity(const cgmath::Vec3& v) {
        ma_sound_set_velocity(&data_, v.x, v.y, v.z);
    }

    cgmath::Vec3 GetVelocity() const {
        auto vel = ma_sound_get_velocity(&data_);
        return {vel.x, vel.y, vel.z};
    }

    float Length() {
        float len;
        MA_CALL(ma_sound_get_length_in_seconds(&data_, &len));
        return len;
    }

    float GetCursor() {
        float cursor;
        MA_CALL(ma_sound_get_cursor_in_seconds(&data_, &cursor));
        return cursor;
    }

    void SetPosition(const cgmath::Vec3& pos) {
        ma_sound_set_position(&data_, pos.x, pos.y, pos.z);
    }

    void SetPosition(const cgmath::Vec2& pos) {
        ma_sound_set_position(&data_, pos.x, 0, pos.y);
    }

    void SetDirection(const cgmath::Vec3& dir) {
        ma_sound_set_direction(&data_, dir.x, dir.y, dir.z);
    }

    void SetDirection(const cgmath::Vec2& dir) {
        ma_sound_set_direction(&data_, dir.x, 0, dir.y);
    }

    cgmath::Vec3 GetDirToListener() {
        auto dir = ma_sound_get_direction_to_listener(&data_);
        return {dir.x, dir.y, dir.z};
    }

    float GetVolumn() const { return ma_sound_get_volume(&data_); }

    void SetVolumn(float vol) { ma_sound_set_volume(&data_, vol); }

    cgmath::Vec3 GetDirection() const {
        auto dir = ma_sound_get_direction(&data_);
        return {dir.x, dir.y, dir.z};
    }

    cgmath::Vec3 GetPosition() const {
        auto pos = ma_sound_get_position(&data_);
        return {pos.x, pos.y, pos.z};
    }

    explicit operator bool() const { return data_.pDataSource != nullptr; }

    toml::table Save2Toml() const override {
        toml::table tbl;
        tbl.emplace("path", RelativePath().string());
        return tbl;
    }

    bool operator==(const Sound& oth) const {
        return RelativePath() == oth.RelativePath();
    }

    bool operator!=(const Sound& oth) const { return !(*this == oth); }

private:
    ma_sound data_{};

    Sound() = default;
};

#undef MA_CALL

using AudioHandle = Handle<Sound>;

class AudioManager : public Manager<Sound> {
public:
    static FileType GetFileType() { return FileType::Audio; }

    AudioHandle Load(const std::filesystem::path& filename) {
        auto sound = std::make_unique<Sound>(filename);
        if (!sound || !(*sound)) {
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
std::unique_ptr<Sound> LoadAssetFromToml(const toml::table&);

}  // namespace nickel