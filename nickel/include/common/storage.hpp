#pragma once

#include "common/log_tag.hpp"
#include "common/sparse_set.hpp"

namespace nickel {

class StorageBase {
public:
    struct Data {
        uint32_t refCount;
        void* data{};
    };

    using SparseSetType = SparseSet<SparseSetPageSize>;
    using StoragePage = std::array<Data, SparseSetType::PageSize>;
    using StorageList = std::vector<StoragePage>;

    virtual ~StorageBase() = default;

    auto& GetSparseSet() const { return sparseSet_; }

    bool Exists(DataID id) const { return sparseSet_.Exists(id); }

    const Data* GetData(DataID id) const {
        if (!Exists(id)) {
            return nullptr;
        }

        auto value = GetDataID_ID(id);
        return &pages_[value / SparseSetPageSize][value % SparseSetPageSize];
    }

protected:
    SparseSetType sparseSet_;
    StorageList pages_;
};

template <typename T>
class Storage : public StorageBase {
public:
    using Base = StorageBase;

    struct AllocResult {
        Data& data;
        DataID id = InvalidDataID;
    };

    Storage() = default;
    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;

    const T* Get(DataID id) const {
        if (!Exists(id)) {
            return nullptr;
        }

        auto value = GetDataID_ID(id);
        return (T*)(pages_[value / SparseSetPageSize][value % SparseSetPageSize]
                        .data);
    }

    T* Get(DataID id) { return const_cast<T*>(std::as_const(*this).Get(id)); }

    AllocResult Allocate() {
        auto id = sparseSet_.Create();
        auto value = GetDataID_ID(id);
        auto& page = assure(value);
        std::allocator<T> alloc;

        auto index = value % SparseSetPageSize;
        page[index].data = alloc.allocate(1);

        return {page[index], id};
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
        ((T*)(data.data))->~T();
        data.refCount = 0;

        auto swapElem = sparseSet_.Destroy(id);
        std::swap(
            pages_[swapElem / SparseSetPageSize][swapElem % SparseSetPageSize],
            data);
    }

    ~Storage() {
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
    auto& assure(size_t idx) {
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
        alloc.deallocate((T*)data.data, 1);
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
        auto dataWithT = (T*)data.data;
        dataWithT->~T();
        alloc.deallocate(dataWithT, 1);
        data.data = nullptr;
        data.refCount = 0;
    }
};

}  // namespace nickel