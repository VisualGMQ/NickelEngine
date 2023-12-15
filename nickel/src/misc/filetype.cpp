#include "misc/filetype.hpp"

namespace nickel {

std::unordered_map<std::string, FileType> gFileTypeMap = {
  // image
    {  ".png",     FileType::Image},
    {  ".bmp",     FileType::Image},
    {  ".jpg",     FileType::Image},
 // font
    {  ".ttf",      FileType::Font},
 // audio
    {  ".wav",     FileType::Audio},
    {  ".ogg",     FileType::Audio},
    {  ".mp3",     FileType::Audio},

    {".timer",     FileType::Timer},
    { ".anim", FileType::Animation},
    {   ".ts", FileType::Tilesheet},
};

FileType DetectFileType(const std::filesystem::path& path) {
    auto extension = path.extension().string();
    if (auto it = gFileTypeMap.find(extension); it != gFileTypeMap.end()) {
        return it->second;
    }
    return FileType::Unknown;
}

}  // namespace nickel