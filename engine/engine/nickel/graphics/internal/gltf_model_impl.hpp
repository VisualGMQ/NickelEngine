#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/graphics/lowlevel/buffer.hpp"
#include "nickel/graphics/material.hpp"
#include "nickel/graphics/mesh.hpp"
#include "nickel/graphics/texture.hpp"
#include "nickel/graphics/internal/material3d_impl.hpp"

namespace nickel::graphics {

class GLTFModelImpl : public RefCountable {
public:
    Buffer pbr_parameter_buffer;
    std::vector<Buffer> dataBuffers;
    std::vector<Texture> textures;
    std::vector<Sampler> samplers;
    std::vector<Scene> scenes;
    std::vector<Material3D> materials;
    BlockMemoryAllocator<Material3DImpl> m_mtl_allocator;

    ~GLTFModelImpl();

    void Move(const Vec3& offset);
    void Scale(const Vec3& delta);
    void Rotate(const Quat& delta);
    
    void MoveTo(const Vec3& position);
    void ScaleTo(const Vec3& scale);
    void RotateTo(const Quat& q);

    void UpdateTransform();

private:
    Transform m_transform;
    bool m_should_update_transform{false};

    void tryUpdateSceneTransform();
};

}