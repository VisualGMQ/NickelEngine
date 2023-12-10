#pragma once

#include "nickel.hpp"

enum class FileType {
    Unknown,
    Image,
    Font,
    Audio,
};

FileType DetectFileType(const std::filesystem::path& path);