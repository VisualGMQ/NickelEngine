#pragma once
#include "common/storage.hpp"
#include "common/log.hpp"
#include "common/log_tag.hpp"

namespace nickel {

template <typename T>
class DataStorage : public Storage<T, uint32_t> {
public:
    using Base = Storage<T, uint32_t>;

    DataStorage() = default;
    DataStorage(const DataStorage&) = delete;
    DataStorage& operator=(const DataStorage&) = delete;

    template <bool IsConst>
    struct Data {
        std::conditional_t<IsConst, const T*, T*> value;
        std::conditional_t<IsConst, const uint32_t*, uint32_t*> refcount;
    };

    struct AllocResult {
        T* data;
        uint32_t* refcount;
        DataID id;
    };

    void IncRefcount(DataID id) {
        if (!Base::Exists(id)) {
            return;
        }
        auto value = GetDataID_ID(id);
        uint32_t& refcount = *Get(id).refcount;
        refcount++;
    }

    void DecRefcount(DataID id) {
        if (!Base::Exists(id)) {
            return;
        }
        auto value = GetDataID_ID(id);

        uint32_t& refcount = *Get(id).refcount;
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
        auto value = GetDataID_ID(id);

        return *Get(id).refcount;
    }

    Data<false> Get(DataID id) {
        auto result = Base::Get(id);
        auto [asset, refcount] = result;
        return Data<false>{asset, refcount};
    }

    Data<true> Get(DataID id) const {
        auto result = Base::Get(id);
        auto [asset, refcount] = result;
        return Data<true>{asset, refcount};
    }

    T* GetWithRef(DataID id) {
        if (!Base::Exists(id)) {
            return nullptr;
        }
        auto value = GetDataID_ID(id);
        auto [data, refcount] = Get(id);
        (*refcount)++;
        return data;
    }

    AllocResult Allocate() {
        auto result = Base::Allocate();
        AllocResult r = {std::get<0>(result.data), std::get<1>(result.data), result.id};
        *r.refcount = 0;
        return r;
    }
};

}  // namespace nickel