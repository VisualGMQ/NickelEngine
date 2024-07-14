#pragma once

#include "common/data_pool.hpp"
#include "common/sparse_set.hpp"
#include "stdpch.hpp"
#include "common/handle.hpp"
#include "toml++/toml.hpp"

namespace nickel {

class Asset {
public:
    virtual ~Asset() = default;

    const std::filesystem::path& GetRelativePath() const {
        return relativePath_;
    }

    void ChangeRelativePath(const std::filesystem::path& filename) {
        relativePath_ = filename;
    }

    /**
     * @brief load from asset file
     * @note load from meta file default. Override this if you load from raw
     * asset file
     * @param relativePath
     */
    virtual bool Load(const std::filesystem::path& relativePath) {
        auto parse = toml::parse_file(relativePath.string());
        if (!parse) {
            LOGE(log_tag::Asset, "load asset ", relativePath,
                 " failed: ", parse.error());
            return false;
        }

        bool success = Load(parse.table());
        ChangeRelativePath(relativePath);
        return success;
    }

    /**
     * @brief load asset from meta file
     *
     * @param relativePath
     * @return true
     * @return false
     */
    bool LoadFromMeta(const std::filesystem::path& relativePath) {
        auto tbl = toml::parse(relativePath.string());
        if (!tbl) {
            LOGE(log_tag::Asset, "load asset ", relativePath,
                 " failed: ", tbl.error());
            relativePath_.clear();
            return false;
        } else {
            relativePath_ = relativePath;
            if (Load(tbl.table())) {
                return true;
            }
            relativePath_.clear();
            LOGE(log_tag::Asset, "load asset ", relativePath, " failed");
            return false;
        }
    }

    bool SaveToMeta() const {
        if (!relativePath_.empty()) {
            LOGE(log_tag::Asset, "trying save invalid asset");
            return true;
        } else {
            return SaveAs(relativePath_);
        }
    }

    bool SaveAs(const std::filesystem::path& filename) const {
        toml::table tbl;
        if (Save(tbl)) {
            std::ofstream file(filename);
            if (!file) {
                LOGE(log_tag::Asset, "save asset to ", filename,
                     " failed: open file failed");
            } else {
                file << toml::toml_formatter{tbl};
            }
            return true;
        }
        return false;
    }

    virtual bool Load(const toml::table&) = 0;
    virtual bool Save(toml::table&) const = 0;

private:
    std::filesystem::path relativePath_;
};

#define NICKEL_TOML_EMPLACE_NODE(tbl, name, value)                          \
    do {                                                                    \
        if (!tbl.emplace(name, value).second) {                             \
            LOGE(log_tag::Asset, "empalce node ", name, " to toml failed"); \
            return false;                                                   \
        }                                                                   \
    } while (0)

}  // namespace nickel