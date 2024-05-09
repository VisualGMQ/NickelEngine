#include "audio/audio.hpp"
#include "common/log_tag.hpp"
#include "common/profile.hpp"
#include "miniaudio.h"

namespace nickel {


#define MA_CALL(expr)                                     \
    do {                                                  \
        if (auto err = (expr); err != MA_SUCCESS) {       \
            LOGW(log_tag::Audio, #expr, " faild: ", err); \
        }                                                 \
    } while (0)


ma_engine gEngine;

Sound Sound::Null;

SoundPlayer::SoundPlayer()
    : mgr_{&ECS::Instance().World().res_mut<AudioManager>().get()} {
    data_ = new ma_sound;
    data_->pDataSource = nullptr;
}

SoundPlayer::SoundPlayer(SoundHandle handle) : SoundPlayer() {
    handle_ = handle;
    recreateInnerSound(handle, *mgr_);
}

SoundPlayer::~SoundPlayer() {
    ma_sound_uninit(data_);
    delete data_;
}

Sound::Sound(const std::filesystem::path& filename) : Asset(filename) {
    data_ = new ma_decoder;
    if (auto result =
            ma_decoder_init_file(filename.string().c_str(), NULL, data_);
        result != MA_SUCCESS) {
        LOGW(nickel::log_tag::Asset, "load audio from ", filename,
             " failed: ", ma_result_description(result));
    }
}

void* Sound::GetAudioData() {
    return data_;
}

Sound::~Sound() {
    ma_decoder_uninit(data_);
    delete data_;
}

void SoundPlayer::recreateInnerSound(SoundHandle handle, AudioManager& mgr) {
    if (data_->pDataSource) {
        ma_sound_uninit(data_);
    }

    handle_ = handle;
    if (mgr.Has(handle)) {
        auto& sound = mgr.Get(handle);
        if (auto result = ma_sound_init_from_data_source(
                &gEngine, (ma_data_source*)sound.GetAudioData(),
                MA_SOUND_FLAG_DECODE, nullptr, data_);
            result != MA_SUCCESS) {
            LOGW(log_tag::Audio, "create audio player failed: ", result);
        }
    }
}

void SoundPlayer::Play() {
    MA_CALL(ma_sound_start(data_));
}

void SoundPlayer::Stop() {
    MA_CALL(ma_sound_stop(data_));
}

void SoundPlayer::Pause() {
    MA_CALL(ma_sound_stop(data_));
}

void SoundPlayer::Rewind() {
    MA_CALL(ma_sound_seek_to_pcm_frame(data_, 0));
}

void SoundPlayer::SetLoop(bool loop) {
    ma_sound_set_looping(data_, true);
}

bool SoundPlayer::IsPlaying() const {
    return ma_sound_is_playing(data_);
}

bool SoundPlayer::IsLooping() const {
    return ma_sound_is_looping(data_);
}

bool SoundPlayer::IsAtTheEnd() const {
    return ma_sound_at_end(data_);
}

void SoundPlayer::SetStartTime(uint64_t millisecond) {
    ma_sound_set_start_time_in_milliseconds(data_, millisecond);
}

void SoundPlayer::SetStopTime(uint64_t millisecond) {
    ma_sound_set_stop_time_in_milliseconds(data_, millisecond);
}

void SoundPlayer::SetVelocity(const cgmath::Vec3& v) {
    ma_sound_set_velocity(data_, v.x, v.y, v.z);
}

cgmath::Vec3 SoundPlayer::GetVelocity() const {
    auto vel = ma_sound_get_velocity(data_);
    return {vel.x, vel.y, vel.z};
}

float SoundPlayer::Length() {
    float len;
    MA_CALL(ma_sound_get_length_in_seconds(data_, &len));
    return len;
}

float SoundPlayer::GetCursor() {
    float cursor;
    MA_CALL(ma_sound_get_cursor_in_seconds(data_, &cursor));
    return cursor;
}

void SoundPlayer::SetPosition(const cgmath::Vec3& pos) {
    ma_sound_set_position(data_, pos.x, pos.y, pos.z);
}

void SoundPlayer::SetPosition(const cgmath::Vec2& pos) {
    ma_sound_set_position(data_, pos.x, 0, pos.y);
}

void SoundPlayer::SetDirection(const cgmath::Vec3& dir) {
    ma_sound_set_direction(data_, dir.x, dir.y, dir.z);
}

void SoundPlayer::SetDirection(const cgmath::Vec2& dir) {
    ma_sound_set_direction(data_, dir.x, 0, dir.y);
}

cgmath::Vec3 SoundPlayer::GetDirToListener() {
    auto dir = ma_sound_get_direction_to_listener(data_);
    return {dir.x, dir.y, dir.z};
}

float SoundPlayer::GetVolumn() const {
    return ma_sound_get_volume(data_);
}

void SoundPlayer::SetVolumn(float vol) {
    ma_sound_set_volume(data_, vol);
}

cgmath::Vec3 SoundPlayer::GetDirection() const {
    auto dir = ma_sound_get_direction(data_);
    return {dir.x, dir.y, dir.z};
}

cgmath::Vec3 SoundPlayer::GetPosition() const {
    auto pos = ma_sound_get_position(data_);
    return {pos.x, pos.y, pos.z};
}

SoundPlayer::operator bool() const {
    return data_ && data_->pDataSource != nullptr;
}

void SoundPlayer::ChangeSound(SoundHandle handle) {
    recreateInnerSound(handle, *mgr_);
}

void InitAudioSystem() {
    PROFILE_BEGIN();

    auto result = ma_engine_init(NULL, &gEngine);
    if (result != MA_SUCCESS) {
        LOGW(log_tag::Audio, "miniaudio engine init failed: ", result);
    }
}

void ShutdownAudioSystem() {
    ma_engine_uninit(&gEngine);
}

template <>
std::unique_ptr<Sound> LoadAssetFromMetaTable(const toml::table& tbl) {
    if (auto path = tbl.get("path"); path && path->is_string()) {
        return std::make_unique<Sound>(path->as_string()->get());
    }
    return nullptr;
}

}  // namespace nickel