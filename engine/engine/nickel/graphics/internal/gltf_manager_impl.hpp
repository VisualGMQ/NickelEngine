#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/graphics/gltf.hpp"
#include "nickel/graphics/internal/gltf_model_impl.hpp"
#include "nickel/graphics/internal/material3d_impl.hpp"
#include "nickel/graphics/internal/mesh_impl.hpp"

namespace nickel::graphics {
class CommonResource;
class GLTFRenderPass;
class Node;
struct GLTFLoadData;

class GLTFManagerImpl {
public:
    explicit GLTFManagerImpl(Device device, CommonResource&, GLTFRenderPass&);
    ~GLTFManagerImpl();

    bool Load(const GLTFLoadData&, const GLTFLoadConfig&);
    bool Load(const Path&, const GLTFLoadConfig&);
    GLTFModel Find(const std::string&);
    void GC();
    void Remove(GLTFModelImpl&);
    void Clear();
    std::vector<std::string> GetAllGLTFModelNames() const;

    std::unordered_map<std::string, GLTFModelImpl*> m_models;

    BlockMemoryAllocator<GLTFModelResourceImpl> m_model_resource_allocator;
    BlockMemoryAllocator<GLTFModelImpl> m_model_allocator;
    BlockMemoryAllocator<MeshImpl> m_mesh_allocator;

    // TODO: extract material 3d to single material3D manager
    BlockMemoryAllocator<Material3DImpl> m_mtl_allocator;

    Material3DImpl* m_default_material;
    Buffer m_default_pbr_param_buffer;

    std::set<std::string> m_pending_delete;

private:
    void preorderNode(const GLTFLoadData& load_data,
                      const Node& node,
                      const GLTFModelResource& resource, std::span<const Mesh> meshes,
                      GLTFModelImpl& parent_model);
};

}  // namespace nickel::graphics