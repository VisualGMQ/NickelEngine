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


}