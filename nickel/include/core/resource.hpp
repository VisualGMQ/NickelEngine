#pragma once
#include "pch.hpp"

namespace nickel {

/**
 * @brief parent class of all resource classes
 */
class Res {
public:
    virtual ~Res() = default;

    Res(const std::filesystem::path& relativePath)
        : relativePath_(relativePath) {}
    Res() = default;

    auto& RelativePath() const { return relativePath_; }

private:
    std::filesystem::path relativePath_;
};

}  // namespace nickel