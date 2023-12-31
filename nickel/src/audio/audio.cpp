#include "audio/audio.hpp"
#include "core/log_tag.hpp"
#include "misc/asset_manager.hpp"

namespace nickel {

ma_engine gEngine;

Sound Sound::Null;

Sound::Sound(const std::filesystem::path& filename) : Asset(filename) {
    if (auto result =
            ma_decoder_init_file(filename.string().c_str(), NULL, data_.get());
        result != MA_SUCCESS) {
        LOGW(nickel::log_tag::Asset, "load audio from ", filename,
             " failed: ", result);
    }
}

Sound::Sound(const toml::table& tbl) {
    if (auto node = tbl.get("path"); node && node->is_string()) {
        *this = Sound(node->as_string()->get());
    }
}

void* Sound::GetAudioData() {
    return data_.get();
}

Sound::~Sound() {
    ma_decoder_uninit(data_.get());
}

SoundPlayer SoundPlayer::Null;

SoundPlayer::SoundPlayer(SoundHandle handle) {
    recreateInnerSound(handle);
}

void SoundPlayer::recreateInnerSound(SoundHandle handle) {
    if (data_->pDataSource) {
        ma_sound_uninit(data_.get());
    }

    handle_ = handle;
    auto assetMgr = gWorld->res_mut<AssetManager>();
    if (assetMgr->Has(handle)) {
        auto& sound = assetMgr->Get(handle);
        if (auto result = ma_sound_init_from_data_source(
                &gEngine, (ma_data_source*)sound.GetAudioData(),
                MA_SOUND_FLAG_DECODE, nullptr, data_.get());
            result != MA_SUCCESS) {
            LOGW(log_tag::Audio, "create audio player failed: ", result);
        }
    }
}

void SoundPlayer::ChangeSound(SoundHandle handle) {
    recreateInnerSound(handle);
}

void InitAudioSystem() {
    auto result = ma_engine_init(NULL, &gEngine);
    if (result != MA_SUCCESS) {
        LOGW(log_tag::Audio, "miniaudio engine init failed: ", result);
        return;
    }
}

void ShutdownAudioSystem() {
    ma_engine_uninit(&gEngine);
}

template <>
std::unique_ptr<Sound> LoadAssetFromMeta(const toml::table& tbl) {
    if (auto path = tbl.get("path"); path && path->is_string()) {
        return std::make_unique<Sound>(tbl);
    }
    return nullptr;
}

}  // namespace nickel