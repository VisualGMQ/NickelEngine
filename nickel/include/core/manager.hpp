#pragma once

#include "core/handle.hpp"

namespace nickel {

template <typename T>
class Manager {
public:
    Manager() = default;
    Manager(const Manager&) = delete;
    Manager operator=(const Manager&) = delete;

    using AssetType = T;
    using AssetHandle = Handle<AssetType>;
    using AssetStoreType = std::unique_ptr<AssetType>;

    void Destroy(AssetHandle handle) { datas_.erase(handle); }

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
        for (auto& [handle, texture] : AllDatas()) {
            if (GetRootPath() / texture->RelativePath() ==
                GetRootPath() / path) {
                return handle;
            }
        }
        return {};
    }

    const AssetType& Get(const std::filesystem::path& path) const {
        for (auto& [handle, texture] : AllDatas()) {
            if (GetRootPath() / texture->RelativePath() ==
                GetRootPath() / path) {
                return Get(handle);
            }
        }
        return AssetType::Null;
    }

    bool Has(AssetHandle handle) const {
        return datas_.find(handle) != datas_.end();
    }

    auto& GetRootPath() const { return rootPath_; }

    void SetRootPath(const std::filesystem::path& path) { rootPath_ = path; }

    void ReleaseAll() { datas_.clear(); }

    auto& AllDatas() const { return datas_; }

protected:
    void storeNewItem(AssetHandle handle, AssetStoreType&& item) {
        if (handle) {
            datas_.emplace(handle, std::move(item));
        }
    }

    std::filesystem::path addRootPath(const std::filesystem::path& path) const {
        return rootPath_ / path;
    }

    std::unordered_map<AssetHandle, AssetStoreType, typename Handle<T>::Hash,
                       typename Handle<T>::HashEq>
        datas_;
    std::filesystem::path rootPath_ = "./";
};

template <typename T>
using ResourceManager = Manager<T>;

}  // namespace nickel