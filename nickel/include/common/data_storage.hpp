#pragma once
#include "common/storage.hpp"

namespace nickel {

template <typename T>
class DataStorage final : public Storage<T> {
public:
    using Base = Storage<T>;

    DataStorage() = default;
    DataStorage(const DataStorage&) = delete;
    DataStorage& operator=(const DataStorage&) = delete;

    void IncRefCount(DataID id) {
        if (!Exists(id)) {
            return;
        }
        auto value = GetDataID_ID(id);
        Base::pages_[value / SparseSetPageSize][value % SparseSetPageSize]
            .refCount++;
    }

    void DecRefCount(DataID id) {
        if (!Exists(id)) {
            return;
        }
        auto value = GetDataID_ID(id);

        auto& refCount =
            Base::pages_[value / SparseSetPageSize][value % SparseSetPageSize]
                .refCount;
        if (refCount == 0) {
            LOGE(log_tag::Nickel,
                 "trying decrease refcount when refcount == 0");
        } else {
            refCount--;
        }
    }

    uint32_t GetRefCount(DataID id) const {
        if (!Base::Exists(id)) {
            return 0;
        }
        auto value = GetDataID_ID(id);

        return Base::pages_[value / SparseSetPageSize]
                           [value % SparseSetPageSize]
                               .refCount;
    }

    T* GetWithRef(DataID id) {
        if (!Base::Exists(id)) {
            return nullptr;
        }
        auto value = GetDataID_ID(id);
        auto& data =
            Base::pages_[value / SparseSetPageSize][value % SparseSetPageSize];
        data.refCount++;
        return (T*)data.data;
    }
};


}