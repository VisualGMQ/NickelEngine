#pragma once
#include "common/config.hpp"

namespace nickel {

/**
 * @brief data id, consist of `data_id | version_id`
 */
enum class DataID : DataIDType {};

inline constexpr DataIDType DataID2Num(DataID id) {
    return static_cast<DataIDType>(id);
}

inline constexpr DataID Num2DataID(DataIDType num) {
    return static_cast<DataID>(num);
}

inline constexpr DataIDType GetDataID_ID(DataID id) {
    return DataID2Num(id) & DataID_IDMask;
}

inline constexpr DataIDType GetDataID_Version(DataID id) {
    return (DataID2Num(id) & DataID_VersionMask) >> DataID_IDMask_Len;
}

inline constexpr DataID MakeupDataID(DataIDType id, DataIDType version) {
    return Num2DataID(id | (version << DataID_IDMask_Len));
}

inline constexpr DataID IncDataID_Version(DataID dataID) {
    auto version = GetDataID_Version(dataID);
    auto id = GetDataID_ID(dataID);
    return Num2DataID(((version + 1) << DataID_IDMask_Len) | id);
}

constexpr DataID InvalidDataID =
    Num2DataID(std::numeric_limits<uint64_t>::max());

inline bool operator==(DataID id1, DataID id2) {
    return DataID2Num(id1) == DataID2Num(id2);
}

inline bool operator!=(DataID id1, DataID id2) {
    return !(id1 == id2);
}

inline bool IsDataIDValid(DataID id) {
    return id != InvalidDataID;
}

class SparseSet {
public:
    using SparsePage = std::array<size_t, SparseSetPageSize>;
    static constexpr size_t InvalidIndex = std::numeric_limits<size_t>::max();

    /**
     * @brief create new id
     * 
     * @return DataID 
     */
    DataID Create() {
        if (elemCount_ >= dense_.size()) {
            auto newElem = dense_.size();
            dense_.push_back(newElem);
            elemCount_ = dense_.size();
            Index(Num2DataID(newElem)) = dense_.size() - 1;
            return Num2DataID(newElem);
        } else {
            return Num2DataID(dense_[elemCount_++]);
        }
    }

    /**
     * @brief Get id index in dense
     * 
     * @param id 
     * @return size_t& 
     */
    size_t& Index(DataID id) {
        auto value = GetDataID_ID(id);
        return assure(value)[value % SparseSetPageSize];
    }

    /**
    * @brief destroy id and inc it's version
    * 
    * @param id 
    * @return size_t 
    */
    size_t Destroy(DataID id) {
        if (!IsDataIDValid(id)) {
            return InvalidIndex;
        }

        auto value = GetDataID_ID(id);
        auto version = GetDataID_Version(id);
        auto pageIndex = value / SparseSetPageSize;
        auto valueIndex = value % SparseSetPageSize;

        if (pageIndex >= sparse_.size()) {
            return InvalidIndex;
        }

        auto& index = sparse_[pageIndex][valueIndex];
        if (index == InvalidIndex || dense_[index] != DataID2Num(id)) {
            return InvalidIndex;
        }

        dense_[index] =
            DataID2Num(IncDataID_Version(Num2DataID(dense_[index])));

        size_t dstSparseIndex = value;
        if (index < elemCount_ - 1) {
            auto lastElem = GetDataID_ID(Num2DataID(dense_[elemCount_ - 1]));
            dstSparseIndex = sparse_[lastElem / SparseSetPageSize]
                                    [lastElem % SparseSetPageSize];
            std::swap(dense_[elemCount_ - 1], dense_[index]);
            std::swap(sparse_[lastElem / SparseSetPageSize]
                             [lastElem % SparseSetPageSize],
                      index);
        }
        elemCount_--;
        return dstSparseIndex;
    }

    bool Exists(DataID dataID) const {
        if (!IsDataIDValid(dataID)) {
            return false;
        }

        auto id = GetDataID_ID(dataID);
        auto pageIndex = id / SparseSetPageSize;
        if (pageIndex >= sparse_.size()) {
            return false;
        }
        auto value = dense_[sparse_[pageIndex][id % SparseSetPageSize]];
        return value == id;
    }

    size_t ElemCount() const { return elemCount_; }

    auto& Dense() { return dense_; }

    auto& Dense() const { return dense_; }

    auto& Sparse() const { return sparse_; }

private:
    size_t elemCount_{};
    std::vector<DataIDType> dense_;
    std::vector<SparsePage> sparse_;

    SparsePage& assure(size_t id) {
        auto pageIndex = id / SparseSetPageSize;
        if (pageIndex >= sparse_.size()) {
            auto oldSize = sparse_.size();
            sparse_.resize(pageIndex + 1);
            for (size_t i = oldSize; i < sparse_.size(); i++) {
                sparse_[i].fill(InvalidIndex);
            }
        }
        return sparse_[pageIndex];
    }
};

}  // namespace nickel