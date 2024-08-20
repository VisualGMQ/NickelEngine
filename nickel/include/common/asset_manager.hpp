#pragma once
#include "common/asset.hpp"
#include "common/data_storage.hpp"
#include "common/log.hpp"
#include "common/log_tag.hpp"
#include "common/ref.hpp"
#include "common/storage.hpp"
#include "common/typeid_generator.hpp"
#include "stdpch.hpp"

namespace nickel {

template <typename T>
struct LoadStoreStrategy;

template <typename T>
concept IsLoadStoreStrategy = requires(T obj, const T cobj) {
    requires std::is_default_constructible_v<T>;
    typename T::AssetType;
    { obj.Load(std::filesystem::path{}, nullptr) } -> std::same_as<bool>;
    { obj.Save(typename T::AssetType{}) } -> std::same_as<bool>;
    { cobj.GetRelativePath() } -> std::same_as<const std::filesystem::path&>;
    obj.ChangeRelativePath(std::filesystem::path{});
    { T::GetMetaExtension() } -> std::same_as<std::string_view>;
    { T::IsImportable() } -> std::same_as<bool>;
};

struct LoadStoreStrategyHelper {
public:
    auto& GetRelativePath() const noexcept { return filename_; }

    void ChangeRelativePath(const std::filesystem::path& path) {
        filename_ = path;
    }

private:
    std::filesystem::path filename_;
};

template <typename T>
requires IsLoadStoreStrategy<LoadStoreStrategy<T>>
class AssetStorage final : public Storage<T, uint32_t, LoadStoreStrategy<T>> {
public:
    using Base = Storage<T, uint32_t, LoadStoreStrategy<T>>;
    using Strategy = LoadStoreStrategy<T>;

    struct Data {
        T* asset{};
        uint32_t* refcount{};
        LoadStoreStrategy<T>* strategy{};
        DataID id = InvalidDataID;
    };

    struct PreallocateData {
        explicit PreallocateData(Base::PreallocateResult result)
            : result_{result} {}

        DataID ID() const { return result_.id; }

        T* AssetMem() { return std::get<0>(result_.data); }

        uint32_t* Refcount() { return std::get<1>(result_.data); }

        LoadStoreStrategy<T>* StrategyMem() {
            return std::get<2>(result_.data);
        }

        void MakeAvailable() { result_.MakeAvailable(); }

        operator bool() const noexcept { return result_; }

    private:
        typename Base::PreallocateResult result_;
    };

    using Base::Exists;

    AssetStorage() = default;
    AssetStorage(const AssetStorage&) = delete;
    AssetStorage& operator=(const AssetStorage&) = delete;

    Data Allocate() {
        auto [data, id] = Base::Allocate();
        auto [asset, refcount, strategy] = data;
        *refcount = 0;

        return {asset, refcount, strategy, id};
    }

    void IncRefcount(DataID id) {
        if (!Base::Exists(id)) {
            return;
        }

        (*getRefcount(id))++;
    }

    void DecRefcount(DataID id) {
        if (!Base::Exists(id)) {
            return;
        }

        auto& refcount = *getRefcount(id);

        if (refcount == 0) {
            LOGE(log_tag::Nickel,
                 "trying decrease refcount when refcount == 0");
        } else {
            refcount--;
        }
    }

    uint32_t GetRefCount(DataID id) const {
        if (!Base::Exists(id)) {
            return 0;
        }

        return getRefcount(id);
    }

    T* GetWithRef(DataID id) {
        if (!Base::Exists(id)) {
            return nullptr;
        }

        auto [data, refcount, _] = Get(id);
        *refcount++;
        return data;
    }

    DataID Find(const std::filesystem::path& relativePath) {
        if (auto it = filenameDataMap_.find(relativePath);
            it != filenameDataMap_.end()) {
            return it->second;
        }
    }

    template <typename... Args>
    DataID Create(Args&&... args) {
        auto&& [asset, refcount, strategy, id] = Allocate();
        new (asset) T(std::forward<Args>(args)...);
        new (strategy) LoadStoreStrategy<T>();
        return id;
    }

    PreallocateData Preallocate() {
        return PreallocateData{Base::Preallocate()};
    }

    DataID Load(const std::filesystem::path& relativePath) {
        auto mems = Preallocate();

        new (mems.StrategyMem()) Strategy{};

        if (!mems.StrategyMem()->Load(relativePath, mems.AssetMem())) {
            mems.StrategyMem()->~LoadStoreStrategy<T>();
            return InvalidDataID;
        }

        *mems.Refcount() = 0;

        filenameDataMap_[relativePath] = mems.ID();

        mems.MakeAvailable();
        return mems.ID();
    }

    DataID Import(const std::filesystem::path& relativePath) {
        Assert(LoadStoreStrategy<T>::IsImportable(),
               "asset type must be importable");

        auto mems = Preallocate();

        new (mems.StrategyMem()) Strategy{};

        if (!mems.StrategyMem()->Import(relativePath, mems.AssetMem())) {
            mems.StrategyMem()->~LoadStoreStrategy<T>();
            return InvalidDataID;
        }

        *mems.Refcount() = 0;

        filenameDataMap_[relativePath] = mems.ID();

        mems.MakeAvailable();
        return mems.ID();
    }

    Data Get(DataID id) {
        auto [asset, refcount, strategy] = Base::Get(id);
        return {asset, refcount, strategy, id};
    }

    bool Save(DataID id) {
        if (!Exists(id)) return false;

        auto [asset, refcount, strategy] = Get(id);

        return strategy->Save(*asset);
    }

private:
    std::unordered_map<std::filesystem::path, DataID> filenameDataMap_;

    T* getAsset(DataID id) { return std::get<0>(Base::Get(id)); }

    const T* getAsset(DataID id) const { return std::get<0>(Base::Get(id)); }

    uint32_t* getRefcount(DataID id) { return std::get<1>(Base::Get(id)); }

    const uint32_t* getRefcount(DataID id) const {
        return std::get<1>(Base::Get(id));
    }

    Strategy* getStrategy(DataID id) { return std::get<2>(Base::Get(id)); }

    const Strategy* getStrategy(DataID id) const {
        return std::get<2>(Base::Get(id));
    }
};

class AssetManager;

template <typename T>
using AssetHandle = Ref<T, AssetManager>;

class AssetManager final : public Singlton<AssetManager, true> {
public:
    using TypeID = uint32_t;

    template <typename, typename>
    friend class Ref;

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
    requires IsLoadStoreStrategy<LoadStoreStrategy<T>>
    void RegisterExternalAssetType(const std::vector<std::string>& extensions) {
        auto id = TypeIDGenerator::GetID<T>();

        if (storages_.contains(id)) {
            LOGE(log_tag::Asset, "asset already registered");
            return;
        }

        for (auto& ext : extensions) {
            importFileExtensions_[ext] = id;
        }
        if (LoadStoreStrategy<T>::IsExternal()) {
            importFileExtensions_[LoadStoreStrategy<T>::GetMetaExtension()] =
                id;
        }

        storages_[id] = std::make_unique<AssetStorage<T>>();
    }

    /**
     * @brief register a type as asset, associate it with it's meta file
     *
     * @tparam T
     */
    template <typename T>
    requires IsLoadStoreStrategy<LoadStoreStrategy<T>>
    void RegisterInternalAssetType() {
        auto id = TypeIDGenerator::GetID<T>();

        if (storages_.contains(id)) {
            LOGE(log_tag::Asset, "asset already registered");
            return;
        }

        if (!LoadStoreStrategy<T>::IsImportable()) {
            importFileExtensions_[std::string(
                LoadStoreStrategy<T>::GetMetaExtension())] = id;
        }

        storages_.emplace(id, std::make_unique<AssetStorage<T>>());
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
            return AssetHandle<T>{mgr->template Create(std::forward<Args>(args)...)};
        } else {
            return {};
        }
    }

    /**
     * @brief import asset from raw file
     *
     * @param relativePath
     * @return Ref<T>
     */
    template <typename T>
    AssetHandle<T> Import(const std::filesystem::path& filename) {
        auto relPath = cvtPath2Relative(filename);
        if (auto mgr = get<T>(); mgr) {
            return AssetHandle<T>{mgr->Import(filename)};
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
    AssetHandle<T> Load(const std::filesystem::path& filename) {
        auto relPath = cvtPath2Relative(filename);
        if (auto mgr = get<T>(); mgr && !relPath.empty()) {
            return AssetHandle<T>{mgr->Load(filename)};
        }
        return {};
    }

    template <typename T>
    bool Exists(AssetHandle<T> handle) {
        if (auto mgr = get<T>(); mgr) {
            return mgr->Exists(static_cast<DataID>(handle));
        }
        return false;
    }

    template <typename T>
    T* Get(AssetHandle<T> handle) {
        if (auto mgr = get<T>(); mgr) {
            return mgr->Get(static_cast<DataID>(handle)).asset;
        }
        return nullptr;
    }

    template <typename T>
    const LoadStoreStrategy<T>* GetStrategy(AssetHandle<T> handle) const {
        if (auto mgr = get<T>(); mgr) {
            return mgr->Get(static_cast<DataID>(handle)).strategy;
        }
        return nullptr;
    }

    template <typename T>
    LoadStoreStrategy<T>* GetStrategy(AssetHandle<T> handle) {
        if (auto mgr = get<T>(); mgr) {
            return mgr->Get(static_cast<DataID>(handle)).strategy;
        }
        return nullptr;
    }

    template <typename T>
    std::filesystem::path GetRelativePath(AssetHandle<T> handle) const {
        if (auto mgr = get<T>(); mgr) {
            return mgr->Get(static_cast<DataID>(handle)).strategy->GetRelativePath();
        }
        return {};
    }

    template <typename T>
    bool Save(AssetHandle<T> ref) {
        return false;
    }

    template <typename T>
    bool IsRegisteredAsset(TypeID typeID) const {
        auto id = TypeIDGenerator::GetID<T>();
        return storages_.contains(id);
    }

private:
    std::unordered_map<TypeID, std::unique_ptr<StorageBase>> storages_;
    std::unordered_map<std::string, TypeID> importFileExtensions_;
    std::filesystem::path rootPath_;

    template <typename T>
    AssetStorage<T>* get() {
        return const_cast<AssetStorage<T>*>(std::as_const(*this).get<T>());
    }

    template <typename T>
    AssetStorage<T>* const get() const {
        auto typeID = TypeIDGenerator::GetID<T>();
        if (auto it = storages_.find(typeID); it != storages_.end()) {
            return static_cast<AssetStorage<T>* const>(it->second.get());
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

    template <typename T>
    void decRefcount(DataID id) {
        if (auto mgr = get<T>(); mgr) {
            mgr->DecRefcount(id);
        }
    }

    template <typename T>
    void incRefcount(DataID id) {
        if (auto mgr = get<T>(); mgr) {
            mgr->IncRefcount(id);
        }
    }
};

}  // namespace nickel
