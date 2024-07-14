#pragma once
#include "common/data_pool.hpp"
#include "common/sparse_set.hpp"
#include "stdpch.hpp"
#include "common/handle.hpp"
#include "toml++/toml.hpp"
#include "common/asset.hpp"

namespace nickel {

class AssetManagerBase {
public:
    AssetManagerBase(uint32_t typeID) : typeID_{typeID} {}

    virtual ~AssetManagerBase() {}

    AssetManagerBase(const AssetManagerBase&) = delete;
    AssetManagerBase& operator=(const AssetManagerBase&) = delete;

    /**
     * @brief load asset into memory without return a Ref<T>
     *  this function try to get rid of the type T
     *
     * @param relativePath
     */
    virtual bool LoadToMem(const std::filesystem::path& relativePath) = 0;

    auto& GetAssets() const { return datas_; }

    uint32_t GetTypeID() const { return typeID_; }

protected:
    uint32_t typeID_;
    std::vector<DataID> datas_;
};

template <typename T>
class AssetManagerT final : public AssetManagerBase {
public:
    AssetManagerT() : AssetManagerBase{TypeIDGenerator::GetID<T>()} {}

    Handle<T> Find(const std::filesystem::path& relativePath) {
        auto it = std::find_if(datas_.begin(), datas_.end(), [&](DataID id) {
            if (auto payload = DataPool::Instance().Get<T>(id); payload) {
                return payload->GetRelativePath() == relativePath;
            }
            return false;
        });

        if (it == datas_.end()) {
            return {};
        } else {
            return Handle<T>{*it};
        }
    }

    template <typename... Args>
    Handle<T> Create(Args&&... args) {
        auto& dataPool = DataPool::Instance();
        auto [ref, mem] = dataPool.Allocate<T>();
        new (mem) T{std::forward<Args>(args)...};
        return Handle<T>{ref};
    }

    Handle<T> LoadFromMeta(const std::filesystem::path& relativePath) {
        auto& dataPool = DataPool::Instance();
        auto [ref, mem] = dataPool.Allocate<T>();
        new (mem) T{};
        if (!((Asset*)mem)->LoadFromMeta(relativePath)) {
            return {};
        }
        return Handle<T>{ref};
    }

    Handle<T> Load(const std::filesystem::path& relativePath) {
        auto& dataPool = DataPool::Instance();
        auto [ref, mem] = dataPool.Allocate<T>();
        new (mem) T{};
        if (!((Asset*)mem)->Load(relativePath)) {
            return {};
        }
        return Handle<T>{ref};
    }

    bool LoadToMem(const std::filesystem::path& relativePath) override {
        return LoadFromMeta(relativePath);
    }

    bool SaveAs(const Handle<T>& ref,
                const std::filesystem::path& relativePath) {
        if (!ref) return false;

        return ref.GetDataConst()->SaveAs(relativePath);
    }

    bool Save(const Handle<T>& ref) {
        if (!ref) return false;

        return ref.GetDataConst()->Save();
    }
};

class AssetManager final : public Singlton<AssetManager, true> {
public:
    AssetManager() {
        std::error_code err;
        rootPath_ = std::filesystem::current_path(err);
        if (err) {
            LOGE(log_tag::Filesystem,
                 "get current path failed: ", err.message());
        }
    }

    /**
     * @brief register a type as asset, associate it with it's meta file
     *
     * @tparam T
     */
    template <typename T>
    void RegistAssetType(
        std::string_view metaExtension,
        const std::vector<std::string_view>& importableAssetExtensions = {}) {
        auto id = TypeIDGenerator::GetID<T>();
        assetMgrs_.emplace(id, std::make_unique<AssetManagerT<T>>());
        metaExtTypeMap_.emplace(metaExtension, id);
        typeMetaExtMap_.emplace(id, metaExtension);
        for (auto extension : importableAssetExtensions) {
            importAssetExtTypeMap_.emplace(extension, id);
        }
    }

    void ChangeRootPath(const std::filesystem::path& path) { rootPath_ = path; }

    template <typename T>
    Handle<T> Find(const std::filesystem::path& filename) {
        auto relPath = cvtPath2Relative(filename);
        if (auto mgr = get<T>(); mgr && !relPath.empty()) {
            return mgr->Find(filename);
        }
        return {};
    }

    /**
     * @brief create asset
     * 
     * @param relativePath 
     * @return Handle<T> 
     */
    template <typename T, typename... Args>
    Handle<T> Create(Args&&... args) {
        auto& mgr = assure<T>();
        return mgr.template Create<T>(std::forward<Args>(args)...);
    }

    /**
     * @brief load asset from raw file
     * 
     * @param relativePath 
     * @return Handle<T> 
     */
    template <typename T>
    Handle<T> Load(const std::filesystem::path& filename) {
        auto relPath = cvtPath2Relative(filename);
        if (auto& mgr = assure<T>(); !relPath.empty()) {
            return mgr.Load(filename);
        }
        return {};
    }

    /**
     * @brief load asset from meta file
     * 
     * @param relativePath 
     * @return Handle<T> 
     */
    template <typename T>
    Handle<T> LoadFromMeta(const std::filesystem::path& filename) {
        auto relPath = cvtPath2Relative(filename);
        if (auto& mgr = assure<T>(); !relPath.empty()) {
            return mgr.LoadFromMeta(filename);
        }
        return {};
    }



    template <typename T>
    bool SaveAs(const Handle<T>& ref, const std::filesystem::path& filename) {
        auto relPath = cvtPath2Relative(filename);
        if (auto mgr = assure<T>(); mgr && !relPath.empty()) {
            return mgr->SaveAs(filename);
        }
        return false;
    }

    template <typename T>
    bool Save(const Handle<T>& ref) {
        return SaveAs(ref, ref.GetDataConst()->GetRelativePath());
    }

    /**
     * @brief load meta file to memory
     *
     * @param filename  meta filename
     * @return true
     * @return false
     */
    bool LoadToMem(const std::filesystem::path& filename) {
        if (!filename.has_extension()) {
            LOGE(log_tag::Asset, filename, " don't has extension");
            return false;
        }
        auto extension = filename.extension();
        if (auto it = metaExtTypeMap_.find(extension.string());
            it != metaExtTypeMap_.end()) {
            return assetMgrs_[it->second]->LoadToMem(
                cvtPath2Relative(filename));
        } else {
            LOGE(log_tag::Asset, "load asset ", filename,
                 " failed: unregister asset type");
            return false;
        }
    }

    /**
     * @brief load unimported asset(don't has meta file) to memory
     * @param filename  importable asset filename
     * @return true
     * @return false
     */
    bool LoadUnimportAssetToMem(const std::filesystem::path& filename) {
        if (!filename.has_extension()) {
            LOGE(log_tag::Asset, filename, " don't has extension");
            return false;
        }
        auto extension = filename.extension();
        if (auto it = importAssetExtTypeMap_.find(extension.string());
            it != importAssetExtTypeMap_.end()) {
            return assetMgrs_[it->second]->LoadToMem(
                cvtPath2Relative(filename));
        } else {
            LOGE(log_tag::Asset, "load asset ", filename,
                 " failed: no associated asset type");
            return false;
        }
    }

    template <typename T>
    std::string_view GetMetaExtension() const {
        if (auto it = typeMetaExtMap_.find(TypeIDGenerator::GetID<T>());
            it != typeMetaExtMap_.end()) {
            return it->second;
        }
        return {};
    }

    bool IsRegisteredAsset(uint32_t typeID) const {
        return assetMgrs_.find(typeID) != assetMgrs_.end();
    }

    template <typename T>
    bool IsRegisteredAsset() const {
        return assetMgrs_.find(TypeIDGenerator::GetID<T>()) != assetMgrs_.end();
    }

    void SaveAllAssets() const {
        for (auto&& [typeID, mgr] : assetMgrs_) {
            auto& assets = mgr->GetAssets();
            auto metaExt = typeMetaExtMap_.at(typeID);
            for (auto dataID : assets) {
                const Asset* data =
                    (const Asset*)DataPool::Instance().GetData(typeID, dataID);
                auto finalPath = rootPath_ / data->GetRelativePath();
                finalPath.replace_extension(metaExt);
                data->SaveAs(finalPath);
            }
        }
    }

    void LoadAllAssets() const {
        // TODO:
    }

private:
    std::unordered_map<uint32_t, std::unique_ptr<AssetManagerBase>> assetMgrs_;
    std::unordered_map<uint32_t, std::string_view>
        typeMetaExtMap_;  //! @brief asset type <-> meta file extension
    std::unordered_map<std::string_view, uint32_t>
        metaExtTypeMap_;  //! @brief meta file extension <-> asset type
    std::unordered_map<std::string_view, uint32_t>
        importAssetExtTypeMap_;  //! @brief asset extension <-> asset type
    std::filesystem::path rootPath_;

    template <typename T>
    AssetManagerT<T>& assure() {
        auto typeID = TypeIDGenerator::GetID<T>();
        if (auto it = assetMgrs_.find(typeID); it != assetMgrs_.end()) {
            return static_cast<AssetManagerT<T>&>(*it->second);
        } else {
            auto result =
                assetMgrs_.emplace(typeID, std::make_unique<AssetManagerT<T>>());
            return static_cast<AssetManagerT<T>&>(*result.first->second);
        }
    }

    template <typename T>
    AssetManagerT<T>* get() {
        return const_cast<AssetManagerT<T>*>(std::as_const(*this).get<T>());
    }

    template <typename T>
    AssetManagerT<T>* const get() const {
        auto typeID = TypeIDGenerator::GetID<T>();
        if (auto it = assetMgrs_.find(typeID); it != assetMgrs_.end()) {
            return static_cast<AssetManagerT<T>* const>(it->second.get());
        } else {
            return nullptr;
        }
    }

    std::filesystem::path cvtPath2Relative(const std::filesystem::path& path) {
        if (path.is_relative()) {
            return path;
        } else {
            std::error_code err;
            auto relPath = std::filesystem::relative(path, rootPath_, err);
            if (err) {
                LOGE(log_tag::Filesystem, "can't make ", path, " relative to ",
                     rootPath_, ": ", err.message());
                return "";
            }
            return relPath;
        }
    }
};

}  // namespace nickel
