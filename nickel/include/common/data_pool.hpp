#pragma once

#include "common/log.hpp"
#include "common/log_tag.hpp"
#include "common/singlton.hpp"
#include "common/data_storage.hpp"
#include "common/typeid_generator.hpp"
#include "stdpch.hpp"


namespace nickel {

class DataPool final : public Singlton<DataPool, true> {
public:
    template <typename T, typename... Args>
    DataID Emplace(Args&&... args) {
        auto& storage = assure<T>();

        auto [value, id] = storage.Allocate();
        new (value.data) T(std::forward<Args>(args)...);
        return id;
    }

    template <typename T>
    std::tuple<DataID, void*> Allocate() {
        auto& storage = assure<T>();

        auto [value, id] = storage.Allocate();
        return {id, value.data};
    }

    template <typename T>
    bool Exists(DataID id) const {
        if (auto storage = get<T>(); storage) {
            return storage->Exists(id);
        } else {
            return false;
        }
    }

    template <typename T>
    T* const Get(DataID id) const {
        if (auto storage = get<T>(); storage) {
            return storage->Get(id);
        } else {
            return nullptr;
        }
    }

    const void* GetData(uint32_t typeID, DataID id) const {
        if (auto it = storages_.find(typeID); it != storages_.end()) {
            return it->second->GetData(id);
        }
        return nullptr;
    }

    template <typename T>
    T* Get(DataID id) {
        return const_cast<T*>(std::as_const(*this).Get<T>(id));
    }

    template <typename T>
    void IncRefCount(DataID id) {
        if (auto storage = get<T>(); storage && storage->Exists(id)) {
            storage->IncRefCount(id);
        }
    }

    template <typename T>
    void DecRefCount(DataID id) {
        if (auto storage = get<T>(); storage && storage->Exists(id)) {
            storage->DecRefCount(id);
        }
    }

    template <typename T>
    void Destroy(DataID id) {
        if (auto storage = get<T>(); storage) {
            storage->Destroy(id);
        }
    }

    template <typename T>
    DataStorage<T>& GetStorage() {
        return assure<T>();
    }

    template <typename T>
    const DataStorage<T>& GetStorage() const {
        return assure<T>();
    }

private:
    std::unordered_map<uint32_t, std::unique_ptr<StorageBase>> storages_;

    template <typename T>
    DataStorage<T>& assure() {
        auto typeID = TypeIDGenerator::GetID<T>();
        if (auto it = storages_.find(typeID); it != storages_.end()) {
            return static_cast<DataStorage<T>&>(*it->second);
        } else {
            auto result =
                storages_.emplace(typeID, std::make_unique<DataStorage<T>>());
            return static_cast<DataStorage<T>&>(*result.first->second);
        }
    }

    template <typename T>
    DataStorage<T>* get() {
        return const_cast<DataStorage<T>*>(std::as_const(*this).get<T>());
    }

    template <typename T>
    DataStorage<T>* const get() const {
        auto typeID = TypeIDGenerator::GetID<T>();
        if (auto it = storages_.find(typeID); it != storages_.end()) {
            return static_cast<DataStorage<T>* const>(it->second.get());
        } else {
            return nullptr;
        }
    }
};

}  // namespace nickel