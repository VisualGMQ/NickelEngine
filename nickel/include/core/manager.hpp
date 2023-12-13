#pragma once

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
        if (auto it = pathHandleMap_.find(relativePath); it != pathHandleMap_.end()) {
            pathHandleMap_.erase(it);
            datas_.erase(it->second);
        }
    }

    const AssetType& Get(AssetHandle handle) const {
        if (auto it = datas_.find(handle); it != datas_.end()) {
            return *it->second;
        } else {
            return AssetType::Null;
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

    auto& GetRootPath() const { return rootPath_; }

    void SetRootPath(const std::filesystem::path& path) { rootPath_ = path; }

    void ReleaseAll() {
        datas_.clear();
        pathHandleMap_.clear();
    }

    auto& AllDatas() const { return datas_; }

    virtual toml::table Save2Toml() const = 0;
    virtual void LoadFromToml(toml::table&) = 0;

    void Save2TomlFile(const std::filesystem::path& path) const {
        std::ofstream file(path);
        file << toml::toml_formatter{Save2Toml()};
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

    std::unordered_map<AssetHandle, AssetStoreType, typename Handle<T>::Hash,
                       typename Handle<T>::HashEq>
        datas_;
    std::unordered_map<std::filesystem::path, AssetHandle> pathHandleMap_;
    std::filesystem::path rootPath_ = "./";
};

template <typename T>
using ResourceManager = Manager<T>;

}  // namespace nickel