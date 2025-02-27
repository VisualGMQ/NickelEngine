#pragma once
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/graphics/lowlevel/image.hpp"
#include "nickel/graphics/lowlevel/sampler.hpp"
#include "nickel/graphics/material.hpp"

namespace nickel::graphics {

class GLTFManagerImpl;

class Material3DImpl : public RefCountable {
public:
    struct TextureInfo {
        ImageView image;
        Sampler sampler;

        operator bool() const { return image && sampler; }
    };

    BufferView pbrParameters;
    TextureInfo basicTexture;
    TextureInfo normalTexture;
    TextureInfo metalicRoughnessTexture;
    TextureInfo occlusionTexture;
    BindGroup bindGroup;

    Material3DImpl(GLTFManagerImpl*);
    Material3DImpl(Material3D&&) = delete;
    Material3DImpl& operator=(Material3DImpl&&) = delete;
    Material3DImpl(const Material3DImpl&) = delete;
    Material3DImpl& operator=(const Material3DImpl&) = delete;

    void DecRefcount() override;

private:
    GLTFManagerImpl* m_mgr;
};

}  // namespace nickel::graphics