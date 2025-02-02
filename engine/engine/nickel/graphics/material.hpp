#pragma once
#include "nickel/graphics/lowlevel/bind_group.hpp"
#include "nickel/common/math/math.hpp"

namespace nickel::graphics {
struct BufferView {
    std::optional<uint32_t> buffer;
    uint32_t offset{};
    uint64_t size{};
    uint32_t count{};

    operator bool() const noexcept {
        return buffer && size > 0 && count > 0;
    }
};

struct PBRParameters {
    Vec4 baseColor;
    float metalness = 1.0f;
    float roughness = 1.0f;
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
