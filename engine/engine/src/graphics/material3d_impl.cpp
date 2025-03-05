#include "nickel/graphics/internal/material3d_impl.hpp"

#include "nickel/graphics/internal/gltf_manager_impl.hpp"

namespace nickel::graphics {

Material3DImpl::Material3DImpl(GLTFManagerImpl* mgr,
                               const Material3D::Descriptor& mtl_desc,
                               Buffer& camera_buffer, Buffer& view_buffer,
                               BindGroupLayout layout)
    : m_mgr{mgr} {
    BindGroup::Descriptor desc;

    // camera buffer
    {
        BindGroup::Entry entry;
        entry.m_shader_stage = ShaderStage::Vertex;
        entry.m_array_size = 1;
        BindGroup::BufferBinding binding;
        binding.m_buffer = camera_buffer;
        binding.m_type = BindGroup::BufferBinding::Type::Uniform;
        entry.m_binding.m_entry = binding;

        desc.m_entries[0] = entry;
    }

    // view buffer
    {
        BindGroup::Entry entry;
        entry.m_shader_stage = ShaderStage::Fragment;
        entry.m_array_size = 1;
        BindGroup::BufferBinding binding;
        binding.m_buffer = view_buffer;
        binding.m_type = BindGroup::BufferBinding::Type::Uniform;
        entry.m_binding.m_entry = binding;

        desc.m_entries[10] = entry;
    }

    // pbr parameters uniform buffer
    {
        BindGroup::Entry entry;
        entry.m_shader_stage = ShaderStage::Fragment;
        entry.m_array_size = 1;
        BindGroup::BufferBinding binding;
        binding.m_buffer = mtl_desc.pbr_param_buffer;
        binding.m_type = BindGroup::BufferBinding::Type::DynamicUniform;
        binding.m_offset = mtl_desc.pbrParameters.m_offset;
        binding.m_size = mtl_desc.pbrParameters.m_size;
        entry.m_binding.m_entry = binding;

        desc.m_entries[1] = entry;
    }

    pushTextureInfoBinding(desc, mtl_desc.basicTexture, 2, 6);
    pushTextureInfoBinding(desc, mtl_desc.normalTexture, 3, 7);
    pushTextureInfoBinding(desc, mtl_desc.metalicRoughnessTexture, 4, 8);
    pushTextureInfoBinding(desc, mtl_desc.occlusionTexture, 5, 9);

    m_bind_group = layout.RequireBindGroup(desc);
}

void Material3DImpl::pushTextureInfoBinding(BindGroup::Descriptor& desc,
                                            const Material3D::TextureInfo& info,
                                            uint32_t image_slot,
                                            uint32_t sampler_slot) {
    pushTextureBindingPoint(desc, info.image, image_slot);
    pushSamplerBindingPoint(desc, info.sampler, sampler_slot);
}

void Material3DImpl::pushTextureBindingPoint(BindGroup::Descriptor& desc,
                                             const ImageView& view,
                                             uint32_t slot) {
    BindGroup::Entry entry;
    entry.m_array_size = 1;
    entry.m_shader_stage = ShaderStage::Fragment;
    BindGroup::ImageBinding binding;
    binding.m_type = BindGroup::ImageBinding::Type::Image;
    binding.m_view = view;
    entry.m_binding.m_entry = binding;
    desc.m_entries[slot] = entry;
}

void Material3DImpl::pushSamplerBindingPoint(BindGroup::Descriptor& desc,
                                             const Sampler& sampler,
                                             uint32_t slot) {
    BindGroup::Entry entry;
    entry.m_array_size = 1;
    entry.m_shader_stage = ShaderStage::Fragment;
    BindGroup::SamplerBinding binding;
    binding.m_sampler = sampler;
    entry.m_binding.m_entry = binding;
    desc.m_entries[slot] = entry;
}

void Material3DImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_mgr->m_mtl_allocator.MarkAsGarbage(this);
    }
}

}  // namespace nickel::graphics