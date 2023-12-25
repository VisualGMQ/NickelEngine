#pragma once
#include "core/handle.hpp"
#include "pch.hpp"

namespace nickel {

/**
 * @brief parent class of all resource classes
 */
class Asset {
public:
    virtual ~Asset() = default;

    template <typename T>
    friend class Manager;

    Asset(const std::filesystem::path& relativePath)
        : relativePath_(relativePath) {}
    Asset() = default;

    virtual toml::table Save2Toml() const = 0;

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
std::unique_ptr<T> LoadAssetFromToml(const toml::table&,
                                     const std::filesystem::path& = "");

template <typename T>
std::unique_ptr<T> LoadAssetFromFile(const std::filesystem::path& path) {
    if (auto result = toml::parse_file(path.string()); result) {
        return LoadAssetFromToml<T>(result.table());
    } else {
        LOGW(log_tag::Asset, "load asset from ", path,
             " failed: ", result.error());
        return false;
    }
}

}  // namespace nickel