#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/graphics/gltf.hpp"
#include "nickel/graphics/internal/gltf_model_impl.hpp"
#include "nickel/graphics/internal/material3d_impl.hpp"
#include "nickel/graphics/internal/mesh_impl.hpp"

namespace nickel::graphics {
class CommonResource;

class GLTFManagerImpl {
public:
    explicit GLTFManagerImpl(Device device, CommonResource*);
    ~GLTFManagerImpl();

    bool Load(const Path&, const GLTFLoadConfig& load_config);
    GLTFModel Find(const std::string&);
    void GC();
    void Remove(GLTFModelImpl&);

    std::unordered_map<std::string, GLTFModelImpl*> m_models;

    BlockMemoryAllocator<GLTFModelResourceImpl> m_model_resource_allocator;
    BlockMemoryAllocator<GLTFModelImpl> m_model_allocator;
    BlockMemoryAllocator<Material3DImpl> m_mtl_allocator;
    BlockMemoryAllocator<MeshImpl> m_mesh_allocator;

    Material3DImpl* m_default_material;
    Buffer m_default_pbr_param_buffer;

private:
    CommonResource* m_common_resource;
    
    void preorderNode(const tinygltf::Model& gltf_model,
                      const tinygltf::Node& node,
                      const GLTFModelResource& resource, std::span<Mesh> meshes,
                      GLTFModelImpl& parent_model);
};

}  // namespace nickel::graphics