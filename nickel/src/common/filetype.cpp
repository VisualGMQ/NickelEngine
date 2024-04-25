#include "common/filetype.hpp"

namespace nickel {

std::unordered_map<std::string, FileType> gFileTypeMap = {
 // image
    {      ".png",     FileType::Image},
    {      ".bmp",     FileType::Image},
    {      ".jpg",     FileType::Image},
 // font
    {      ".ttf",      FileType::Font},
 // audio
    {      ".wav",     FileType::Audio},
    {      ".ogg",     FileType::Audio},
    {      ".mp3",     FileType::Audio},
    {     ".flac",     FileType::Audio},
 // misc
    {    ".timer",     FileType::Timer},
    {     ".anim", FileType::Animation},
    {".tilesheet", FileType::Tilesheet},
// 3d model
    {".gltf", FileType::GLTF},
// meta
    {".meta", FileType::Meta},
// material
    {".mtl2d", FileType::Material2D},
};

FileType DetectFileType(const std::filesystem::path& path) {
    auto extension = path.extension().string();
    if (auto it = gFileTypeMap.find(extension); it != gFileTypeMap.end()) {
        return it->second;
    }
    return FileType::Unknown;
}

std::string_view GetMetaFileExtension(FileType filetype) {
    switch (filetype) {
        case FileType::Tilesheet:
            return ".tilesheet";
        case FileType::Animation:
            return ".anim";
        case FileType::Timer:
            return ".timer";
        case FileType::Image:
        case FileType::Font:
        case FileType::Audio:
        case FileType::Unknown:
        case FileType::GLTF:
            return ".meta";
        case FileType::Material2D:
            return ".mtl2d";
        case FileType::Meta:
        case FileType::FileTypeCount:
            return "";
    }
}

bool HasMetaFile(FileType filetype) {
    switch (filetype) {
        case FileType::Tilesheet:
        case FileType::Animation:
        case FileType::Timer:
        case FileType::Material2D:
            return false;
        case FileType::Image:
        case FileType::Font:
        case FileType::Audio:
        case FileType::GLTF:
            return true;
        case FileType::Unknown:
        case FileType::FileTypeCount:
        case FileType::Meta:
            return false;
    }
}

std::filesystem::path StripMetaExtension(
    const std::filesystem::path& filename) {
    if (filename.extension() == ".meta") {
        auto newFilename = filename;
        newFilename.replace_extension("");
        return newFilename;
    }
    return filename;
}

}  // namespace nickel