#pragma once

#include "common/assert.hpp"
#include "common/log_tag.hpp"
#include "common/sparse_set.hpp"

namespace nickel {

class StorageBase {
public:
    using SparseSetType = SparseSet<SparseSetPageSize>;

    virtual ~StorageBase() = default;

    auto& GetSparseSet() const { return sparseSet_; }

    bool Exists(DataID id) const { return sparseSet_.Exists(id); }

protected:
    SparseSetType sparseSet_;
};

template <typename T, uint32_t PageSize>
struct StoragePageList {
    using Page = std::array<unsigned char[sizeof(T)], PageSize>;

    Page* pages_ = nullptr;

    StoragePageList() = default;

    ~StoragePageList() { delete[] pages_; }

    StoragePageList(StoragePageList&& other) : pages_{other.pages_} {
        other.pages_ = nullptr;
    }

    StoragePageList& operator=(StoragePageList&& other) {
        if (&other != this) {
            pages_ = other.pages_;
            other.pages_ = nullptr;
        }
        return *this;
    }

    void Resize(size_t size, size_t newSize) {
        Assert(size <= newSize);
        if (newSize == 0) {
            return;
        }
        Page* newPages = new Page[newSize];
        memset(newPages, 0, sizeof(Page) * newSize);
        memcpy(pages_, newPages, sizeof(Page) * size);
        delete[] pages_;
        pages_ = newPages;
    }

    T* Get(size_t majorIdx, size_t minorIdx) {
        return (T*)&pages_[majorIdx][minorIdx];
    }

    const T* Get(size_t majorIdx, size_t minorIdx) const {
        return (T*)&pages_[majorIdx][minorIdx];
    }

    void Destruct(size_t majorIdx, size_t minorIdx) {
        T* data = Get(majorIdx, minorIdx);
        data->~T();

        // for debug:
        memset(data, 0, sizeof(T));
    }

    // for debug
    void ResetMem(size_t majorIdx, size_t minorIdx) {
        T* data = Get(majorIdx, minorIdx);
        memset(data, 0, sizeof(T));
    }

    void Swap(size_t majorIdx1, size_t minorIdx1, size_t majorIdx2,
              size_t minorIdx2) {
        std::swap(pages_[majorIdx1][minorIdx1], pages_[majorIdx2][minorIdx2]);
    }
};

template <typename T, uint32_t PageSize>
requires(std::is_empty_v<T>)
struct StoragePageList<T, PageSize> {
    void Resize(size_t size, size_t newSize) {}

    T* Get(size_t majorIdx, size_t minorIdx) { return (T*)this; }

    const T* Get(size_t majorIdx, size_t minorIdx) const { return (T*)this; }

    void Swap(size_t majorIdx1, size_t minorIdx1, size_t majorIdx2,
              size_t minorIdx2) {}

    void ResetMem(size_t majorIdx, size_t minorIdx) {}

    void Destruct(size_t majorIdx, size_t minorIdx) {}
};

template <typename... Types>
class Storage : private StoragePageList<Types, SparseSetPageSize>...,
                public StorageBase {
public:
    using Base = StorageBase;
    using SparseSetType = typename Base::SparseSetType;

    template <typename T>
    using PageList = StoragePageList<T, SparseSetPageSize>;

    using DataBundle = std::tuple<Types*...>;
    using ConstDataBundle = std::tuple<const Types*...>;

    struct AllocResult {
        DataBundle data;
        DataID id = InvalidDataID;
    };

    struct PreallocateResult {
        DataBundle data;
        DataID id;

        PreallocateResult(DataBundle data, DataID id, Storage* owner)
            : data{data}, id{id}, owner_{owner} {}

        void MakeAvailable() {
            if (operator bool()) {
                Assert(owner_->sparseSet_.Create() == id,
                       "internal error! prefetched id is not valid");
            }
        }

        operator bool() const noexcept { return id != InvalidDataID && owner_; }

    private:
        Storage* owner_{};
    };

    using Base::Exists;
    using Base::GetSparseSet;

    Storage() = default;
    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;

    ConstDataBundle Get(DataID id) const {
        if (!Exists(id)) {
            return {};
        }

        auto value = GetDataID_ID(id);
        auto pageIndex = value / SparseSetPageSize;
        auto elemIndex = value % SparseSetPageSize;

        return std::make_tuple(PageList<Types>::Get(pageIndex, elemIndex)...);
    }

    DataBundle Get(DataID id) {
        if (!Exists(id)) {
            return {};
        }

        auto value = GetDataID_ID(id);
        auto pageIndex = value / SparseSetPageSize;
        auto elemIndex = value % SparseSetPageSize;

        return std::make_tuple(PageList<Types>::Get(pageIndex, elemIndex)...);
    }

    /**
     * @brief allocate memory(didn't constructor) and return a new id
     *
     * @return AllocResult
     */
    AllocResult Allocate() {
        auto id = sparseSet_.Create();
        auto value = GetDataID_ID(id);
        auto pageIndex = value / SparseSetPageSize;
        auto elemIndex = value % SparseSetPageSize;

        assure(value);

        return {std::make_tuple(PageList<Types>::Get(pageIndex, elemIndex)...),
                id};
    }

    /**
     * @brief emplace data. Storage will manage data lifetime
     *
     * @param mems
     * @return DataID
     */
    DataID EmplaceData(Types*... mems) {
        auto id = sparseSet_.Create();
        auto value = GetDataID_ID(id);
        auto pageIndex = value / SparseSetPageSize;
        auto elemIndex = value % SparseSetPageSize;

        assure(value);

        (memcpy(PageList<Types>::Get(pageIndex, elemIndex), mems,
                sizeof(Types)),
         ...);

        return id;
    }

    /**
     * @brief pre-allocate memory and an ID. you can construct data on memories
     * you can call PreallocateResult::MakeAvailable() to make ID & memories
     * available. Or if not, no need to free memory or destroy id
     *
     * @return AllocResult
     */
    PreallocateResult Preallocate() {
        DataID id = sparseSet_.PrefetchNextID();
        auto value = GetDataID_ID(id);
        auto pageIndex = value / SparseSetPageSize;
        auto elemIndex = value % SparseSetPageSize;

        assure(value);

        return {std::make_tuple(PageList<Types>::Get(pageIndex, elemIndex)...),
                id, this};
    }

    /**
     * @brief destruct data, don't free memory
     *
     * @param id
     */
    void Destroy(DataID id) {
        if (!StorageBase::Exists(id)) {
            return;
        }

        auto value = GetDataID_ID(id);
        auto pageIndex = value / SparseSetPageSize;
        auto elemIndex = value % SparseSetPageSize;
        auto swapElem = sparseSet_.Destroy(id);
        auto oldPageIndex = swapElem / SparseSetPageSize;
        auto oldElemIndex = swapElem % SparseSetPageSize;

        (PageList<Types>::Destruct(pageIndex, elemIndex), ...);
        (PageList<Types>::Swap(pageIndex, elemIndex, oldPageIndex,
                               oldElemIndex),
         ...);
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
    size_t count_ = 0;  // page count

    void assure(size_t idx) {
        if (idx < count_ * SparseSetPageSize) {
            return;
        }

        size_t newSize = count_ + (idx == 0 ? 1 : (idx / SparseSetPageSize));
        (PageList<Types>::Resize(count_, newSize), ...);
        count_ = newSize;
    }

    /**
     * @brief don't destruct data, but free memory
     *
     * @param id
     */
    void deallocate(DataID id) {
        auto value = GetDataID_ID(id);
        auto pageIndex = value / SparseSetPageSize;
        if (pageIndex >= count_) return;

        auto elemIndex = value % SparseSetPageSize;

        // not necessary, for debug
        (PageList<Types>::ResetMem(pageIndex, elemIndex), ...);
    }

    /**
     * @brief destruct data and free memory
     *
     * @param id
     */
    void destroyComplete(DataID id) {
        auto value = GetDataID_ID(id);
        auto pageIndex = value / SparseSetPageSize;
        if (pageIndex >= count_) return;

        auto elemIndex = value % SparseSetPageSize;

        (PageList<Types>::Destruct(pageIndex, elemIndex), ...);
    }
};

}  // namespace nickel