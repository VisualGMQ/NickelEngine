#pragma once

#include "stdpch.hpp"

namespace nickel {

namespace internal {

template <uint8_t I>
struct generateMaskByLength {
    static constexpr uint64_t value = (1ll << (I - 1)) | generateMaskByLength<I - 1>::value;
};


template <>
struct generateMaskByLength<1> {
    static constexpr uint64_t value = 1;
};

template <>
struct generateMaskByLength<0> {
    static constexpr uint64_t value = 0;
};

}

/**
 * @brief help tool to generate bit mask by mask length
 *  for example:
 *      I = 0 -> 0b0000
 *      I = 1 -> 0b0001
 *      I = 2 -> 0b0011
 *      I = 3 -> 0b0111
 *      I = 4 -> 0b1111
 * @tparam I mask length
 */
template <uint8_t I>
constexpr auto GenerateMaskByLength = internal::generateMaskByLength<I>::value;

constexpr size_t SparseSetPageSize = 1024;

using DataIDType = uint64_t;
constexpr uint8_t DataID_IDMask_Len = 32;
constexpr uint8_t DataID_VersionMask_Len = 64 - DataID_IDMask_Len;
constexpr uint64_t DataID_IDMask = GenerateMaskByLength<DataID_IDMask_Len>;
constexpr uint64_t DataID_VersionMask = GenerateMaskByLength<DataID_VersionMask_Len> << DataID_IDMask_Len;

}