#pragma once
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/graphics/lowlevel/bind_group_layout.hpp"
#include "nickel/graphics/lowlevel/image.hpp"
#include "nickel/graphics/lowlevel/sampler.hpp"
#include "nickel/graphics/material.hpp"

namespace nickel::graphics {

class GLTFManagerImpl;

class Material3DImpl : public RefCountable {
public:
    BindGroup bindGroup;

    Material3DImpl(GLTFManagerImpl*, const Material3D::Descriptor&,
                   Buffer& camera_buffer, Buffer& view_buffer,
                   BindGroupLayout layout);
    Material3DImpl(Material3D&&) = delete;
    Material3DImpl& operator=(Material3DImpl&&) = delete;
    Material3DImpl(const Material3DImpl&) = delete;
    Material3DImpl& operator=(const Material3DImpl&) = delete;

    void DecRefcount() override;

private:
    GLTFManagerImpl* m_mgr;

    void pushTextureInfoBinding(BindGroup::Descriptor& desc,
                                const Material3D::TextureInfo& info,
                                uint32_t image_slot, uint32_t sampler_slot);
    void pushTextureBindingPoint(BindGroup::Descriptor& desc,
                                 const ImageView& view, uint32_t slot);
    void pushSamplerBindingPoint(BindGroup::Descriptor& desc,
                                 const Sampler& sampler, uint32_t slot);
};

}  // namespace nickel::graphics