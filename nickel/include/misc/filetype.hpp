#pragma once

#include "pch.hpp"

namespace nickel {

enum class FileType {
    Unknown = 0,
    Image = 1,
    Font = 2,
    Audio = 3,
};

FileType DetectFileType(const std::filesystem::path& path);

}