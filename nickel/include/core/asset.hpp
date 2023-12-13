#pragma once
#include "pch.hpp"

namespace nickel {

/**
 * @brief parent class of all resource classes
 */
class Asset {
public:
    virtual ~Asset() = default;

    Asset(const std::filesystem::path& relativePath)
        : relativePath_(relativePath) {}
    Asset() = default;

    auto& RelativePath() const { return relativePath_; }

private:
    std::filesystem::path relativePath_;
};

}  // namespace nickel