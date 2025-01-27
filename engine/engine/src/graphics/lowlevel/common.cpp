#include "nickel/graphics/lowlevel/common.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

const void* ImageRawData::GetData() const {
    return m_data;
}

const SVector<uint32_t, 2> ImageRawData::GetExtent() const {
    return m_extent;
}

ImageRawData::ImageRawData(const std::string& filename) {
    int w, h;
    m_data = stbi_load(filename.c_str(), &w, &h, nullptr, STBI_rgb_alpha);
    if (!m_data) {
        LOGW("load image {} failed", filename);
    } else {
        m_extent.w = w;
        m_extent.h = h;
    }
}

ImageRawData::ImageRawData(ImageRawData&& o) noexcept
    : m_data{o.m_data}, m_extent{o.m_extent} {
    o.m_data = nullptr;
    o.m_extent = {0, 0};
}

ImageRawData& ImageRawData::operator=(ImageRawData&& o) noexcept {
    if (&o != this) {
        m_data = o.m_data;
        m_extent = o.m_extent;
        o.m_data = nullptr;
        o.m_extent = {0, 0};
    }
    return *this;
}

ImageRawData::~ImageRawData() {
    stbi_image_free(m_data);
}

ImageRawData::operator bool() const noexcept {
    return m_data && m_extent.w > 0 && m_extent.h > 0;
}

}  // namespace nickel::graphics