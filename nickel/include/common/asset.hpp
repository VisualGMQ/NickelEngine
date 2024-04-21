#pragma once

#include "common/handle.hpp"
#include <filesystem>

namespace nickel {

/**
 * @brief parent class of all resource classes
 */
class Asset {
public:
    virtual ~Asset() = default;

    template <typename, typename>
    friend class Manager;

    Asset(const std::filesystem::path& relativePath)
        : relativePath_(relativePath) {}
    Asset() = default;

    /**
     * @brief save asset content to toml
     */
    virtual toml::table Save2Toml() const = 0;

    /**
     * @brief save asset content to file
     */
    void Save2File(const std::filesystem::path& filename) const {
        std::ofstream file(filename);
        if (file) {
            file << toml::toml_formatter{Save2Toml()};
        }
    }

    auto& RelativePath() const { return relativePath_; }

    bool HasAssociatedFile() const { return !relativePath_.empty(); }

    void AssociateFile(const std::filesystem::path& path) {
        relativePath_ = path;
    }

private:
    std::filesystem::path relativePath_;
};

template <typename T>
std::unique_ptr<T> LoadAssetFromMetaTable(const toml::table&);

template <typename T>
std::unique_ptr<T> LoadAssetFromMeta(const std::filesystem::path& path) {
    if (auto result = toml::parse_file(path.string()); result) {
        return LoadAssetFromMetaTable<T>(result.table());
    } else {
        LOGW(log_tag::Asset, "load asset from ", path,
             " failed: ", result.error());
        return false;
    }
}

}  // namespace nickel