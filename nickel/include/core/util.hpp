#pragma once

#include "log.hpp"
#include <fstream>
#include <optional>
#include <string>

#include "core/singleton.hpp"

namespace nickel {

/**
 * @addtogroup utilities
 * @{
 */

/**
 * @brief read whole file content
 * @param filename
 * @return file content. Read file failed will return `std::nullopt`
 */
inline std::optional<std::string> ReadWholeFile(const std::string& filename) {
    std::ifstream file(filename);
    if (file.fail()) {
        LOGE("file %s open failed", filename);
        return std::nullopt;
    }
    std::string content(std::istreambuf_iterator<char>(file),
                        (std::istreambuf_iterator<char>()));
    return content;
}

/**
 * @brief eazy use random utility
 */
class Random final : public Singlton<Random, true> {
public:
    Random() : seed_(time(nullptr)) { num_ = seed_; }

    explicit Random(int seed) : seed_(seed) { num_ = seed_; }

    std::uint32_t Rand() {
        num_ = (a_ * num_ + b_) % m_;
        return num_;
    }

    std::uint32_t RandRange(int low, int high) {
        return Rand() % (high - low) + low;
    }

    std::uint32_t Seed() const { return seed_; }

private:
    std::uint32_t a_ = 48271, b_ = 2312,
                  m_ = std::numeric_limits<uint32_t>::max();
    std::uint32_t seed_;
    std::uint32_t num_;
};

/**
 * @}
 */

}  // namespace nickel