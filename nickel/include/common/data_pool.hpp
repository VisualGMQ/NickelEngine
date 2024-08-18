#pragma once

#include "common/data_storage.hpp"
#include "common/log.hpp"
#include "common/log_tag.hpp"
#include "common/ref.hpp"
#include "common/singlton.hpp"
#include "common/typeid_generator.hpp"
#include "stdpch.hpp"

namespace nickel {

class DataPool;

template <typename T>
using DataHandle = Ref<T, DataPool>;

class DataPool final : public Singlton<DataPool, true> {
public:
    template <typename, typename>
    friend class Ref;

    template <typename T, typename... Args>
    DataHandle<T> Emplace(Args&&... args) {
        auto& storage = assure<T>();

        auto [data, refcount, id] = storage.Allocate();
        new (data) T(std::forward<Args>(args)...);
        *refcount = 0;
        return DataHandle<T>{id};
    }

    template <typename T>
    std::tuple<DataID, T*> Allocate() {
        auto& storage = assure<T>();

        auto [data, refcount, id] = storage.Allocate();
        return {id, data};
    }

    template <typename T>
    bool Exists(DataHandle<T> handle) const {
        if (auto storage = get<T>(); storage) {
            return storage->Exists(static_cast<DataID>(handle));
        } else {
            return false;
        }
    }

    template <typename T>
    const T* Get(DataHandle<T> handle) const {
        if (auto storage = get<T>(); storage) {
            return storage->Get(static_cast<DataID>(handle)).value;
        } else {
            return nullptr;
        }
    }

    template <typename T>
    T* Get(DataHandle<T> handle) {
        if (auto storage = get<T>(); storage) {
            return storage->Get(static_cast<DataID>(handle)).value;
        } else {
            return nullptr;
        }
    }

    template <typename T>
    void Destroy(DataHandle<T> handle) {
        if (auto storage = get<T>(); storage) {
            storage->Destroy(static_cast<DataID>(handle));
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

    template <typename T>
    void incRefcount(DataID id) {
        if (auto storage = get<T>(); storage && storage->Exists(id)) {
            storage->IncRefcount(id);
        }
    }

    template <typename T>
    void decRefcount(DataID id) {
        if (auto storage = get<T>(); storage && storage->Exists(id)) {
            storage->DecRefcount(id);
        }
    }
};

}  // namespace nickel