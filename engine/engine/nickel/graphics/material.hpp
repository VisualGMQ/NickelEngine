#pragma once
#include "nickel/common/math/math.hpp"
#include "nickel/graphics/lowlevel/bind_group.hpp"

namespace nickel::graphics {
struct BufferView {
    Buffer m_buffer;
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

struct Material3D final : public ImplWrapper<Material3DImpl> {
    using ImplWrapper::ImplWrapper;

    struct TextureInfo {
        ImageView image;
        Sampler sampler;

        operator bool() const { return image && sampler; }
    };

    struct Descriptor {
        BufferView pbrParameters;
        Buffer pbr_param_buffer;
        TextureInfo basicTexture;
        TextureInfo normalTexture;
        TextureInfo metalicRoughnessTexture;
        TextureInfo occlusionTexture;
    };
};
}  // namespace nickel::graphics
