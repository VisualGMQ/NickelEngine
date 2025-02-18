#pragma once
#include "nickel/graphics/lowlevel/bind_group.hpp"
#include "nickel/common/math/math.hpp"

namespace nickel::graphics {
struct BufferView {
    std::optional<uint32_t> m_buffer;
    uint32_t m_offset{};
    uint64_t m_size{};
    uint32_t m_count{};

    operator bool() const noexcept {
        return m_buffer && m_size > 0 && m_count > 0;
    }
};

struct PBRParameters {
    Vec4 m_base_color;
    float m_metallic = 1.0f;
    float m_roughness = 1.0f;
};

class Material3DImpl;

struct Material3D final {
    Material3D(Material3DImpl* impl);
    Material3D(const Material3D&) noexcept;
    Material3D(Material3D&&) noexcept;
    Material3D& operator=(Material3D&&) noexcept;
    Material3D& operator=(const Material3D&) noexcept;
    ~Material3D();

    Material3DImpl* GetImpl();
    const Material3DImpl* GetImpl() const;

private:
    Material3DImpl* m_impl{};
};
}
