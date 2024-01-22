#include "misc/filetype.hpp"

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
    {".meta", FileType::Meta},
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
        case FileType::FileTypeCount:
            return ".meta";
        case FileType::Meta:
            return "";
    }
}

bool HasMetaFile(FileType filetype) {
    switch (filetype) {
        case FileType::Tilesheet:
        case FileType::Animation:
        case FileType::Timer:
            return false;
        case FileType::Image:
        case FileType::Font:
        case FileType::Audio:
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