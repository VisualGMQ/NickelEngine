#pragma once

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

template <typename... Types>
class Storage : public StorageBase {
public:
    using Base = StorageBase;
    using SparseSetType = typename Base::SparseSetType;

    template <typename T>
    using StoragePage = std::array<T*, SparseSetType::PageSize>;
    using StorageList = std::tuple<std::vector<StoragePage<Types>>...>;

    using DataBundle = std::tuple<Types*...>;
    using ConstDataBundle = std::tuple<Types* const...>;

    struct AllocResult {
        DataBundle data;
        DataID id = InvalidDataID;
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

        auto getDatum = [&, pageIndex]<typename T>(StoragePage<T>& page) {
            return page[pageIndex];
        };

        auto getData = [&,
                        pageIndex]<size_t... Idx>(std::index_sequence<Idx...>) {
            return std::make_tuple(
                std::get<Idx>(pages_)[pageIndex][elemIndex]...);
        };

        return getData(std::make_index_sequence<sizeof...(Types)>());
    }

    DataBundle Get(DataID id) {
        if (!Exists(id)) {
            return {};
        }

        auto value = GetDataID_ID(id);
        auto pageIndex = value / SparseSetPageSize;
        auto elemIndex = value % SparseSetPageSize;

        auto getData = [&,
                        pageIndex]<size_t... Idx>(std::index_sequence<Idx...>) {
            return std::make_tuple(
                std::get<Idx>(pages_)[pageIndex][elemIndex]...);
        };

        return getData(std::make_index_sequence<sizeof...(Types)>());
    }

    /**
     * @brief allocate memory(didn't constructor) and return a new id
     *
     * @return AllocResult
     */
    AllocResult Allocate() {
        auto id = sparseSet_.Create();
        auto value = GetDataID_ID(id);
        auto index = value % SparseSetPageSize;

        auto allocDatum = [=]<typename T>(StoragePage<T>& page) -> T* {
            std::allocator<T> alloc;
            page[index] = alloc.allocate(1);
            return page[index];
        };

        auto pageBundle = assure(value);
        auto allocData = [&]<size_t... Idx>(std::index_sequence<Idx...>) {
            return std::make_tuple<Types*...>(
                allocDatum(*std::get<Idx>(pageBundle))...);
        };

        return {allocData(std::make_index_sequence<sizeof...(Types)>()), id};
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
        auto pageBundle = assure(value);
        auto dataIndex = value % SparseSetPageSize;
        auto swapElem = sparseSet_.Destroy(id);

        auto destructDatum = [=]<typename T>(StoragePage<T>& page) {
            auto& data = page[dataIndex];
            if (!data) {
                return;
            }

            data->~T();

            std::swap(page[swapElem % SparseSetPageSize], data);
        };

        auto destructData = [&]<size_t... Idx>(std::index_sequence<Idx...>) {
            (destructDatum(*std::get<Idx>(pageBundle)), ...);
        };

        destructData(std::make_index_sequence<sizeof...(Types)>());
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
    auto assure(size_t idx) {
        idx = idx / SparseSetPageSize;
        if (idx >= std::get<0>(pages_).size()) {
            auto resizePage = [idx]<typename T>(std::vector<StoragePage<T>>& pages) {
                pages.resize(idx + 1);
            };

            auto resizePages = [&]<size_t...Idx>(std::index_sequence<Idx...>) {
                (resizePage(std::get<Idx>(pages_)), ...);
            };

            resizePages(std::make_index_sequence<sizeof...(Types)>());
        }

        auto fetchData = [&]<size_t... Idx>(std::index_sequence<Idx...>) {
            return std::make_tuple<StoragePage<Types>*...>(&std::get<Idx>(pages_)[idx]...);
        };

        return fetchData(std::make_index_sequence<sizeof...(Types)>());
    }

    /**
     * @brief don't destruct data, but free memory
     *
     * @param id
     */
    void deallocate(DataID id) {
        auto value = GetDataID_ID(id);
        auto pageIndex = value / SparseSetPageSize;
        if (pageIndex >= std::get<0>(pages_).size()) return;

        auto elemIndex = value % SparseSetPageSize;

        auto deallocateDatum = []<typename T>(T* data) {
            if (!data) {
                return;
            }

            std::allocator<T> alloc;
            alloc.deallocate(data, 1);
        };

        auto deallocateData = [&]<size_t... Idx>(std::index_sequence<Idx...>) {
            (deallocateDatum(std::get<Idx>(pages_)[pageIndex][elemIndex]), ...);
        };

        deallocateData(std::make_index_sequence<sizeof...(Types)>());
    }

    /**
     * @brief destruct data and free memory
     *
     * @param id
     */
    void destroyComplete(DataID id) {
        auto value = GetDataID_ID(id);
        auto pageIndex = value / SparseSetPageSize;
        if (pageIndex >= std::get<0>(pages_).size()) return;

        auto elemIndex = value % SparseSetPageSize;

        auto destroyDatum = []<typename T>(T* data) {
            if (!data) {
                return;
            }

            std::allocator<T> alloc;
            data->~T();
            alloc.deallocate(data, 1);
        };

        auto destroyData = [&]<size_t... Idx>(std::index_sequence<Idx...>) {
            (destroyDatum(std::get<Idx>(pages_)[pageIndex][elemIndex]), ...);
        };

        destroyData(std::make_index_sequence<sizeof...(Types)>());
    }

private:
    StorageList pages_;
};

}  // namespace nickel