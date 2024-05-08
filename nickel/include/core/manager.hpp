#pragma once

#include "core/asset.hpp"
#include "core/handle.hpp"
#include "misc/filetype.hpp"

namespace nickel {

template <typename T>
class Manager {
public:
    Manager() = default;

    using AssetType = T;
    using AssetHandle = Handle<AssetType>;
    using AssetStoreType = std::unique_ptr<AssetType>;

    void Destroy(AssetHandle handle) {
        if (Has(handle)) {
            pathHandleMap_.erase(Get(handle).RelativePath());
            datas_.erase(handle);
        }
    }

    void Destroy(const std::filesystem::path& path) {
        if (auto it = pathHandleMap_.find(path);
            it != pathHandleMap_.end()) {
            datas_.erase(it->second);
            pathHandleMap_.erase(it);
        }
    }

    const AssetType& Get(AssetHandle handle) const {
        if (auto it = datas_.find(handle); it != datas_.end()) {
            return *it->second;
        } else {
            return AssetType::Null;
        }
    }

    void Reload(AssetHandle handle, const std::filesystem::path& filename) {
        if (Has(handle)) {
            auto& elem = Get(handle);
            auto parse = toml::parse_file(filename.string());
            if (!parse) {
                LOGW(log_tag::Asset, "load asset from ", filename, " failed");
                return;
            }
            AssetType newElem(parse.table());
            newElem.AssociateFile(filename);
            elem = std::move(newElem);
        }
    }

    AssetType& Get(AssetHandle handle) {
        return const_cast<AssetType&>(std::as_const(*this).Get(handle));
    }

    AssetHandle GetHandle(const std::filesystem::path& path) const {
        if (auto it = pathHandleMap_.find(path);
            it != pathHandleMap_.end()) {
            return it->second;
        }
        return {};
    }

    const AssetType& Get(const std::filesystem::path& path) const {
        if (auto it = pathHandleMap_.find(path);
            it != pathHandleMap_.end()) {
            return Get(it->second);
        }
        return AssetType::Null;
    }

    bool Has(const std::filesystem::path& path) const {
        return GetHandle(path) != AssetHandle::Null();
    }

    bool Has(AssetHandle handle) const {
        return datas_.find(handle) != datas_.end();
    }

    AssetHandle Create(AssetStoreType&& asset,
                       const std::filesystem::path& filename) {
        auto handle = AssetHandle::Create();
        asset->AssociateFile(filename);
        storeNewItem(handle, std::move(asset));
        return handle;
    }

    void ReleaseAll() {
        datas_.clear();
        pathHandleMap_.clear();
    }

    auto& AllDatas() const { return datas_; }

    /**
     * @brief save all assets metadata to file
     */
    void SaveAssets2File() const {
        for (auto& [_, asset] : AllDatas()) {
            asset->Save2File(attachMetafileExt(*asset));
        }
    }

    /**
     * @brief save manager info to toml
     */
    toml::table Save2Toml(const std::filesystem::path& projRootDir) const {
        toml::table tbl;

        toml::array arr;
        for (auto& [_, asset] : AllDatas()) {
            if (!asset->RelativePath().empty()) {
                arr.push_back(attachMetafileExt(*asset).string());
            }
        }
        tbl.emplace("datas", arr);

        return tbl;
    }

    /**
     * @brief save manager info into file
     */
    void Save2File(const std::filesystem::path& projRootDir,
                   const std::filesystem::path& filename) const {
        std::ofstream file(filename);
        file << Save2Toml(projRootDir);
    }

    void LoadFromToml(const std::filesystem::path& filename) {
        auto parse = toml::parse_file(filename.string());
        if (!parse) {
            LOGW(nickel::log_tag::Asset, "load manager from config file ", filename,
                 " failed:", parse.error());
        } else {
            LoadFromToml(parse.table());
        }
    }

    /**
     * @brief load assets from config toml table
     */
    void LoadFromToml(const toml::table& tbl) {
        if (auto dataPaths = tbl["datas"]; dataPaths.is_array()) {
            for (auto& node : *dataPaths.as_array()) {
                if (!node.is_string()) {
                    continue;
                }
                auto& dataPath = node.as_string()->get();
                LoadAssetFromMeta(dataPath);
            }
        }
    }

    void LoadAssetFromMeta(const std::filesystem::path& filename) {
        auto parse = toml::parse_file(filename.string());
        if (!parse) {
            LOGW(nickel::log_tag::Asset, "load asset from meta file ", filename,
                 " failed:", parse.error());
        } else {
            if (auto asset = ::nickel::LoadAssetFromMeta<T>(parse.table());
                asset && *asset) {
                asset->AssociateFile(StripMetaExtension(filename));
                storeNewItem(AssetHandle::Create(), std::move(asset));
            }
        }
    }

    void AssociateFile(AssetHandle handle,
                       const std::filesystem::path& filename) {
        if (Has(handle)) {
            auto& elem = Get(handle);
            pathHandleMap_.erase(elem.RelativePath());
            elem.AssociateFile(filename);
            pathHandleMap_[elem.RelativePath()] = handle;
        }
    }

protected:
    void storeNewItem(AssetHandle handle, AssetStoreType&& item) {
        if (handle) {
            pathHandleMap_.emplace(item->RelativePath(), handle);
            datas_.emplace(handle, std::move(item));
        }
    }

    std::filesystem::path attachMetafileExt(const T& asset) const {
        auto filetype = DetectFileType<T>();
        std::filesystem::path relativePath = asset.RelativePath();
        if (HasMetaFile(filetype) &&
            relativePath.extension() != GetMetaFileExtension(filetype)) {
            relativePath += GetMetaFileExtension(filetype);
        }
        return relativePath;
    }

    std::unordered_map<AssetHandle, AssetStoreType, typename AssetHandle::Hash,
                       typename AssetHandle::Eq>
        datas_;

    class PathHash {
    public:
        size_t operator()(const std::filesystem::path& p) const {
            return std::hash<std::string>()(p.string());
        }
    };

    std::unordered_map<std::filesystem::path, AssetHandle, PathHash>
        pathHandleMap_;
};

}  // namespace nickel
