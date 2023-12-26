#pragma once

#include "core/asset.hpp"
#include "core/handle.hpp"

namespace nickel {

template <typename T>
class Manager {
public:
    Manager() = default;

    using AssetType = T;
    using AssetHandle = Handle<AssetType>;
    using AssetStoreType = std::unique_ptr<AssetType>;

    virtual ~Manager() = default;

    void Destroy(AssetHandle handle) {
        if (Has(handle)) {
            pathHandleMap_.erase(Get(handle).RelativePath());
            datas_.erase(handle);
        }
    }

    void Destroy(const std::filesystem::path& path) {
        auto relativePath = convert2RelativePath(path);
        if (auto it = pathHandleMap_.find(relativePath);
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
            AssetType newElem(GetRootPath(), convert2RelativePath(filename));
            elem = std::move(newElem);
        }
    }

    AssetType& Get(AssetHandle handle) {
        return const_cast<AssetType&>(std::as_const(*this).Get(handle));
    }

    AssetHandle GetHandle(const std::filesystem::path& path) const {
        auto relativePath = convert2RelativePath(path);
        if (auto it = pathHandleMap_.find(relativePath);
            it != pathHandleMap_.end()) {
            return it->second;
        }
        return {};
    }

    const AssetType& Get(const std::filesystem::path& path) const {
        auto relativePath = convert2RelativePath(path);
        if (auto it = pathHandleMap_.find(relativePath);
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

    auto& GetRootPath() const { return rootPath_; }

    void SetRootPath(const std::filesystem::path& path) { rootPath_ = path; }

    void ReleaseAll() {
        datas_.clear();
        pathHandleMap_.clear();
    }

    auto& AllDatas() const { return datas_; }

    toml::table Save2Toml(const std::filesystem::path& rootDir) const {
        toml::table tbl;

        auto relativePath = std::filesystem::relative(GetRootPath(), rootDir);
        tbl.emplace("root_path", relativePath.string());
        toml::array arr;
        for (auto& [_, asset] : AllDatas()) {
            arr.push_back(asset->Save2Toml());
        }
        tbl.emplace("datas", arr);

        return tbl;
    }

    /**
     * @brief ignore `root_path` in tbl and use resourcePath to load assets
     */
    void LoadFromTomlWithPath(const toml::table& tbl,
                              const std::filesystem::path& configDir) {
        if (auto root = tbl["root_path"]; root.is_string()) {
            SetRootPath(configDir / root.as_string()->get());
        }

        if (auto datas = tbl["datas"]; datas.is_array()) {
            for (auto& node : *datas.as_array()) {
                if (!node.is_table()) {
                    continue;
                }

                auto& elemTbl = *node.as_table();

                if (auto asset = LoadAssetFromToml<T>(elemTbl, GetRootPath());
                    asset && *asset) {
                    storeNewItem(AssetHandle::Create(), std::move(asset));
                }
            }
        }
    }

    /**
     * @brief load assets from config toml table
     */
    void LoadFromToml(const toml::table& tbl) { LoadFromTomlWithPath(tbl, ""); }

    void Save2TomlFile(const std::filesystem::path& path) const {
        std::ofstream file(path);
        file << Save2Toml();
    }

    void AssociateFile(AssetHandle handle, const std::filesystem::path& filename) {
        if (Has(handle)) {
            auto& elem = Get(handle);
            pathHandleMap_.erase(elem.RelativePath());
            elem.AssociateFile(convert2RelativePath(filename));
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

    auto convert2RelativePath(const std::filesystem::path& path) const {
        return path.is_relative()
                   ? path
                   : std::filesystem::relative(path, GetRootPath());
    }

    std::filesystem::path addRootPath(const std::filesystem::path& path) const {
        return rootPath_ / path;
    }

    std::unordered_map<AssetHandle, AssetStoreType, typename AssetHandle::Hash,
                       typename AssetHandle::HashEq>
        datas_;
    std::filesystem::path rootPath_ = "./";
    std::unordered_map<std::filesystem::path, AssetHandle> pathHandleMap_;
};

}  // namespace nickel