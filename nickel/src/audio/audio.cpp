#include "audio/audio.hpp"
#include "core/log_tag.hpp"

namespace nickel {

ma_engine gEngine;

Sound Sound::Null;

Sound::Sound(const std::filesystem::path &rootPath,
             const std::filesystem::path &filename)
    : Asset(filename) {
    if (auto result = ma_sound_init_from_file(
            &gEngine, (rootPath / filename).string().c_str(),
            MA_SOUND_FLAG_DECODE, nullptr, nullptr, &data_);
        result != MA_SUCCESS) {
        LOGW(log_tag::Audio, "load audio ", filename, " failed: ", result);
    }
}

Sound::~Sound() {
    ma_sound_uninit(&data_);
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
std::unique_ptr<Sound> LoadAssetFromToml(const toml::table& tbl,
                                         const std::filesystem::path &root) {
    if (auto path = tbl.get("path"); path && path->is_string()) {
        return std::make_unique<Sound>(root, path->as_string()->get());
    }
    return nullptr;
}

}  // namespace nickel