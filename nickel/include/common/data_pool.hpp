#pragma once

#include "stdpch.hpp"
#include "common/singlton.hpp"
#include "common/sparse_set.hpp"
#include "common/log.hpp"
#include "common/log_tag.hpp"

namespace nickel {

struct TypeIDGenerator {
public:
    template <typename T>
    static uint32_t GetID() {
        static uint32_t id = id_++;
        return id;
    }

private:
    static uint32_t id_;
};

class DataStorageBase {
public:
    virtual ~DataStorageBase() = default;
};

template <typename T>
class DataStorage final : public DataStorageBase {
public:
    struct Data {
        uint32_t refCount{};
        T* data{};
    };

    struct AllocResult {
        Data& data;
        DataID id = InvalidDataID;
    };

    using StoragePage = std::array<Data, SparseSetPageSize>;

    DataStorage() = default;
    DataStorage(const DataStorage&) = delete;
    DataStorage& operator=(const DataStorage&) = delete;

    bool Exists(DataID id) const { return sparseSet_.Exists(id); }

    void IncRefCount(DataID id) {
        if (!Exists(id)) {
            return;
        }
        auto value = GetDataID_ID(id);
        pages_[value / SparseSetPageSize][value % SparseSetPageSize].refCount++;
    }

    void DecRefCount(DataID id) {
        if (!Exists(id)) {
            return;
        }
        auto value = GetDataID_ID(id);

        auto& refCount =
            pages_[value / SparseSetPageSize][value % SparseSetPageSize]
                .refCount;
        if (refCount == 0) {
            LOGE(log_tag::Nickel,
                 "trying decrease refcount when refcount == 0");
        } else {
            refCount--;
        }
    }

    T* const Get(DataID id) const {
        if (!Exists(id)) {
            return nullptr;
        }

        auto value = GetDataID_ID(id);
        return pages_[value / SparseSetPageSize][value % SparseSetPageSize]
            .data;
    }

    T* Get(DataID id) { return const_cast<T*>(std::as_const(*this).Get(id)); }

    T* GetWithRef(DataID id) {
        if (!Exists(id)) {
            return nullptr;
        }
        auto value = GetDataID_ID(id);
        auto& data =
            pages_[value / SparseSetPageSize][value % SparseSetPageSize];
        data.refCount++;
        return data.data;
    }

    AllocResult Allocate() {
        auto id = sparseSet_.Create();
        auto value = GetDataID_ID(id);
        auto& page = assure(value);
        std::allocator<T> alloc;

        auto index = value % SparseSetPageSize;
        page[index].data = alloc.allocate(1);

        return {page[index], id};
    }

    uint32_t GetRefCount(DataID id) const {
        if (!sparseSet_.Exists(id)) {
            return 0;
        }

        auto value = GetDataID_ID(id);
        return pages_[value / SparseSetPageSize][value % SparseSetPageSize]
            .refCount;
    }

    /**
     * @brief destruct data, don't free memory
     *
     * @param id
     */
    void Destroy(DataID id) {
        if (!Exists(id)) {
            return;
        }

        auto value = GetDataID_ID(id);
        auto& page = assure(value);
        std::allocator<T> alloc;
        auto& data = page[value % SparseSetPageSize];
        data.data->~T();
        data.refCount = 0;

        auto swapElem = sparseSet_.Destroy(id);
        std::swap(
            pages_[swapElem / SparseSetPageSize][swapElem % SparseSetPageSize],
            data);
    }

    ~DataStorage() {
        auto& dense = sparseSet_.Dense();
        int i = 0;
        for (; i < sparseSet_.ElemCount(); i++) {
            destroyComplete(Num2DataID(dense[i]));
        }

        for (; i < dense.size(); i++) {
            deallocate(Num2DataID(dense[i]));
        }
    }

private:
    SparseSet sparseSet_;
    std::vector<StoragePage> pages_;

    StoragePage& assure(size_t idx) {
        idx = idx / SparseSetPageSize;
        if (idx >= pages_.size()) {
            pages_.resize(idx + 1);
        }
        return pages_[idx];
    }

    /**
     * @brief don't destruct data, but free memory
     *
     * @param id
     */
    void deallocate(DataID id) {
        auto value = GetDataID_ID(id);
        auto pageIndex = value / SparseSetPageSize;
        if (pageIndex >= pages_.size()) return;

        auto elemIndex = value % SparseSetPageSize;
        auto& page = pages_[pageIndex];
        if (!page[elemIndex].data) {
            return;
        }

        auto& data = page[elemIndex];
        std::allocator<T> alloc;
        alloc.deallocate(data.data, 1);
        data.data = nullptr;
        data.refCount = 0;
    }

    /**
     * @brief destruct data and free memory
     *
     * @param id
     */
    void destroyComplete(DataID id) {
        auto value = GetDataID_ID(id);
        auto pageIndex = value / SparseSetPageSize;
        if (pageIndex >= pages_.size()) return;

        auto elemIndex = value % SparseSetPageSize;
        auto& page = pages_[pageIndex];
        if (!page[elemIndex].data) {
            return;
        }

        std::allocator<T> alloc;
        auto& data = page[elemIndex];
        alloc.deallocate(data.data, 1);
        data.data = nullptr;
        data.refCount = 0;
    }
};

template <typename T>
struct Ref {
public:
    Ref(): id_{InvalidDataID} {}
    explicit Ref(DataID id): id_{id} {}

    Ref(const Ref& ref);
    Ref(Ref&& ref);
    Ref& operator=(const Ref<T>& ref);
    Ref& operator=(Ref<T>&& ref);
    ~Ref();

    T* GetData();
    T* const GetData() const;
    bool Valid() const;

    operator bool() const {
        return Valid();
    }

    operator DataID() const { return id_; }

private:
    DataID id_;
};

class DataPool final: public Singlton<DataPool, true> {
public:
    template <typename T, typename... Args>
    Ref<T> Emplace(Args&&... args) {
        auto& storage = assure<T>();

        auto [value, id] = storage.Allocate();
        new (value.data) T(std::forward<Args>(args)...);
        value.refCount = 1;
        return Ref<T>{id};
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
    DataStorage<T>& GetStorage() { return assure<T>(); }

    template <typename T>
    const DataStorage<T>& GetStorage() const { return assure<T>(); }
    
private:
    std::unordered_map<uint32_t, std::unique_ptr<DataStorageBase>> storages_;

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

template <typename T>
bool Ref<T>::Valid() const {
    return id_ != InvalidDataID && DataPool::Instance().Exists<T>(id_);
}

template <typename T>
T* Ref<T>::GetData() {
    return DataPool::Instance().Get<T>(id_);
}

template <typename T>
T* const Ref<T>::GetData() const {
    return DataPool::Instance().Get<T>(id_);
}

template <typename T>
Ref<T>::Ref(const Ref& o) : id_{o.id_} {
    id_ = o.id_;
    DataPool::Instance().IncRefCount<T>(id_);
}

template <typename T>
Ref<T>::Ref(Ref&& o) : id_{o.id_} {
    o.id_ = InvalidDataID;
}

template <typename T>
Ref<T>& Ref<T>::operator=(const Ref<T>& o) {
    if (&o != this) {
        id_ = o.id_;
        DataPool::Instance().IncRefCount<T>(id_);
    }
    return *this;
}

template <typename T>
Ref<T>& Ref<T>::operator=(Ref<T>&& o) {
    if (&o != this) {
        id_ = o.id_;
        o.id_ = InvalidDataID;
    }
    return *this;
}

template <typename T>
Ref<T>::~Ref() {
    if (DataPool::HasInstance()) {
        DataPool::Instance().DecRefCount<T>(id_);
    }
}

}  // namespace nickel