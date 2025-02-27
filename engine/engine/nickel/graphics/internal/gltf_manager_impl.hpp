#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/graphics/gltf.hpp"

namespace nickel::graphics {

class GLTFManagerImpl {
public:
    ~GLTFManagerImpl();

    GLTFModel Load(const Path&);
    void GC();
    void Update();
    void Remove(GLTFModelImpl&);

    std::unordered_map<Path, GLTFModelImpl*> m_models;

    BlockMemoryAllocator<GLTFModelResourceImpl> m_model_resource_allocator;
    BlockMemoryAllocator<GLTFModelImpl> m_model_allocator;
    BlockMemoryAllocator<Material3DImpl> m_mtl_allocator;
};

}  // namespace nickel::graphics