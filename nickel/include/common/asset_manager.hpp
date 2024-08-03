#pragma once
#include "common/asset.hpp"
#include "common/data_storage.hpp"
#include "common/log.hpp"
#include "common/log_tag.hpp"
#include "common/ref.hpp"
#include "common/storage.hpp"
#include "common/typeid_generator.hpp"
#include "stdpch.hpp"
#include "toml++/toml.hpp"

namespace nickel {

class LoadStoreStrategy {
public:
    virtual ~LoadStoreStrategy() = default;

    virtual void* Load(const std::filesystem::path&) = 0;
    virtual bool Save() const = 0;

    auto& GetRelativePath() const noexcept { return relativePath_; }

    void ChangeRelativePath(const std::filesystem::path& relPath) {
        relativePath_ = relPath;
    }

private:
    std::filesystem::path relativePath_;
};

template <typename T>
concept IsLoadStoreStrategy = requires {
    { T::GetMetaExtension() } -> std::same_as<const std::filesystem::path>;
    { T::IsImportable() } -> std::same_as<bool>;
};

template <typename T>
struct AssetData {
    T* asset{};
    LoadStoreStrategy* strategy;
};

using CreateStrategyFn =
    std::function<std::unique_ptr<LoadStoreStrategy>(void)>;

template <typename T>
class AssetManagerT final : public DataStorage<AssetData<T>> {
public:
    using Base = DataStorage<T>;
    using DataType = AssetData<T>;

    explicit AssetManagerT(const CreateStrategyFn& fn) : createFn_{fn} {}

    DataID Find(const std::filesystem::path& relativePath) {
        if (auto it = filenameDataMap_.find(relativePath);
            it != filenameDataMap_.end()) {
            return it->second;
        }
    }

    template <typename... Args>
    DataID Create(Args&&... args) {
        auto&& [data, id] = Allocate();
        new (data.data.asset) T(std::forward<Args>(args)...);
        return id;
    }

    DataID LoadFromMeta(const std::filesystem::path& relativePath) {
        auto strategy = createFn_();
        T* asset = (T*)strategy->Load(relativePath);
        if (!asset) {
            return InvalidDataID;
        }

        auto&& [data, id] = Allocate();
        *data.data = DataType{asset, std::move(strategy)};
        filenameDataMap_[relativePath] = id;
        return id;
    }

    DataID Load(const std::filesystem::path& relativePath) {
        auto strategy = createFn_();
        T* asset = strategy->Load(relativePath);
        if (!asset) {
            return InvalidDataID;
        }

        auto&& [data, id] = Allocate();
        *data.data = DataType{asset, std::move(strategy)};
        filenameDataMap_[relativePath] = id;
        return id;
    }

    bool Exists(DataID id) const { return Get(id); }

    bool SaveAs(DataID id, const std::filesystem::path& relativePath) {
        if (!Exists(id)) return false;

        DataType* data = Get(id);
        return data->strategy.SaveAs(*data->asset, relativePath);
    }

    bool Save(DataID id) {
        if (!Exists(id)) return false;

        DataType* data = Get(id);
        return data->strategy.SaveAs(*data->asset,
                                     data->strategy.GetRelativePath());
    }

private:
    std::unordered_map<std::filesystem::path, DataID> filenameDataMap_;
    const CreateStrategyFn& createFn_;
};

class AssetManager;

template <typename T>
using AssetHandle = Ref<T, AssetManager>;

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
    template <typename T, typename Strategy>
    requires(IsLoadStoreStrategy<Strategy>)
    void RegisterAssetType() {
        auto id = TypeIDGenerator::GetID<T>();
        if (createStrategyFnMap_.contains(id)) {
            LOGW(log_tag::Asset,
                 "register asset type failed: asset already exists");
        }
    }

    void ChangeRootPath(const std::filesystem::path& path) { rootPath_ = path; }

    auto& GetRootPath() const noexcept { return rootPath_; }

    template <typename T>
    AssetHandle<T> Find(const std::filesystem::path& filename) {
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
     * @return Ref<T>
     */
    template <typename T, typename... Args>
    AssetHandle<T> Create(Args&&... args) {
        if (auto mgr = get<T>(); mgr) {
            return mgr->template Create<T>(std::forward<Args>(args)...);
        } else {
            return {};
        }
    }

    /**
     * @brief load asset from raw file
     *
     * @param relativePath
     * @return Ref<T>
     */
    template <typename T>
    AssetHandle<T> Load(const std::filesystem::path& filename) {
        auto relPath = cvtPath2Relative(filename);
        if (auto mgr = get<T>(); mgr) {
            return mgr->Load(filename);
        }
        return {};
    }

    /**
     * @brief load asset from meta file
     *
     * @param relativePath
     * @return Ref<T>
     */
    template <typename T>
    AssetHandle<T> LoadFromMeta(const std::filesystem::path& filename) {
        auto relPath = cvtPath2Relative(filename);
        if (auto& mgr = assure<T>(); !relPath.empty()) {
            return mgr.LoadFromMeta(filename);
        }
        return {};
    }

    template <typename T>
    bool Save(const AssetHandle<T>& ref) {
        return false;
    }

    template <typename T>
    bool IsRegisteredAsset(uint32_t typeID) const {
        auto id = TypeIDGenerator::GetID<T>();
        return createStrategyFnMap_.contains(id);
    }

private:
    std::unordered_map<uint32_t, std::unique_ptr<StorageBase>> storages_;
    std::unordered_map<uint32_t, CreateStrategyFn> createStrategyFnMap_;
    std::filesystem::path rootPath_;

    template <typename T>
    AssetManagerT<T>* get() {
        return const_cast<AssetManagerT<T>*>(std::as_const(*this).get<T>());
    }

    template <typename T>
    AssetManagerT<T>* const get() const {
        auto typeID = TypeIDGenerator::GetID<T>();
        if (auto it = storages_.find(typeID); it != storages_.end()) {
            return static_cast<AssetManagerT<T>* const>(it->second.get());
        } else {
            LOGE(log_tag::Asset, "asset don't registered");
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
