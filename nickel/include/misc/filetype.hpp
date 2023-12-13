#pragma once

#include "pch.hpp"

namespace nickel {

// NOTE: don't change order(related to misc/asset.hpp)
enum class FileType {
    Unknown = 0,
    Image,
    Font,
    Audio,

    FileTypeCount,
};

FileType DetectFileType(const std::filesystem::path& path);

}