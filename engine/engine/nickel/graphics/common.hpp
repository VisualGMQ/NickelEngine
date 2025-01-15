#pragma once
#include "nickel/common/math/smatrix.hpp"

#include <string>

namespace nickel::graphics {

struct ImageRawData {
    const void* GetData() const;
    const SVector<uint32_t, 2> GetExtent() const;

    explicit ImageRawData(const std::string& filename);
    ImageRawData(ImageRawData&&) noexcept;
    ImageRawData& operator=(ImageRawData&&) noexcept;
    ImageRawData(const ImageRawData&) = delete;
    ImageRawData& operator=(const ImageRawData&) = delete;
    ~ImageRawData();
    operator bool() const noexcept;

private:
    void* m_data;
    SVector<uint32_t, 2> m_extent;
};

}
