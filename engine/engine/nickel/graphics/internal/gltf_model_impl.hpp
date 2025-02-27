#pragma once
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/graphics/gltf.hpp"

namespace nickel::graphics {

struct GLTFModelResourceImpl : public RefCountable {
    explicit GLTFModelResourceImpl(GLTFManagerImpl* mgr);

    GLTFGPUResource m_gpu_resource;
    GLTFCPUData m_cpu_data;

    void DecRefcount() override;

private:
    GLTFManagerImpl* m_mgr;
};

class GLTFModelImpl final : public RefCountable {
public:
    GLTFModelImpl(GLTFManagerImpl* mgr, GLTFModelResource&& resource,
                  std::unique_ptr<Mesh>&& mesh);

    void Move(const Vec3& offset);
    void Scale(const Vec3& delta);
    void Rotate(const Quat& delta);

    void MoveTo(const Vec3& position);
    void ScaleTo(const Vec3& scale);
    void RotateTo(const Quat& q);

    void UpdateTransform();

    void DecRefcount() override;

    // TODO remove unique_ptr
    std::unique_ptr<Mesh> m_mesh;
    GLTFModelResource m_resource;

private:
    bool m_should_update_transform{false};
    GLTFManagerImpl* m_mgr{};

    void tryUpdateSceneTransform();
};

}  // namespace nickel::graphics