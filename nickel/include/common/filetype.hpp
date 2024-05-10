#pragma once

#include "stdpch.hpp"

namespace nickel {

// NOTE: don't change order(related to misc/asset.hpp)
enum class FileType {
    Unknown = 0,
    Meta,
    Image,
    Font,
    Audio,
    Tilesheet,
    Animation,
    Timer,
    Script,
    GLTF,
    Material2D,

    FileTypeCount,
};

FileType DetectFileType(const std::filesystem::path& path);

// fwd
class Texture;
class Sound;
class Font;
class Tilesheet;
class Animation;
class Timer;
class LuaScript;
class Material2D;

template <typename T>
FileType DetectFileType() {
    if constexpr (std::is_same_v<T, Texture>) {
        return FileType::Image;
    } else if constexpr (std::is_same_v<T, Font>) {
        return FileType::Font;
    } else if constexpr (std::is_same_v<T, Sound>) {
        return FileType::Audio;
    } else if constexpr (std::is_same_v<T, Tilesheet>) {
        return FileType::Tilesheet;
    } else if constexpr (std::is_same_v<T, Animation>) {
        return FileType::Animation;
    } else if constexpr (std::is_same_v<T, Timer>) {
        return FileType::Timer;
    } else if constexpr (std::is_same_v<T, LuaScript>) {
        return FileType::Script;
    } else if constexpr (std::is_same_v<T, Material2D>) {
        return FileType::Material2D;
    }
    return FileType::Unknown;
}

std::string_view GetMetaFileExtension(FileType);
bool HasMetaFile(FileType);
std::filesystem::path StripMetaExtension(const std::filesystem::path&);

}  // namespace nickel