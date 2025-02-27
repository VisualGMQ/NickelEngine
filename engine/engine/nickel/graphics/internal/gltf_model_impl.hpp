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
    GLTFModelImpl(GLTFManagerImpl* mgr);

    void DecRefcount() override;

    std::string m_name;
    Mat44 m_transform = Mat44::Identity();
    Mesh m_mesh;
    std::vector<GLTFModel> m_children;
    GLTFModelResource m_resource;

private:
    GLTFManagerImpl* m_mgr{};
};

}  // namespace nickel::graphics