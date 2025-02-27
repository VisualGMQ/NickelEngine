#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/graphics/gltf.hpp"
#include "nickel/graphics/internal/gltf_model_impl.hpp"
#include "nickel/graphics/internal/material3d_impl.hpp"
#include "nickel/graphics/internal/mesh_impl.hpp"
#include "tiny_gltf.h"

namespace nickel::graphics {

class GLTFManagerImpl {
public:
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

private:
    void preorderNode(const tinygltf::Model& gltf_model,
                      const tinygltf::Node& node,
                      const GLTFModelResource& resource, std::span<Mesh> meshes,
                      GLTFModelImpl& parent_model);
    Mat44 calcNodeTransform(const tinygltf::Node& node);
};

}  // namespace nickel::graphics