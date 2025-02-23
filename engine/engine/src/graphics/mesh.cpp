#include "nickel/graphics/mesh.hpp"

namespace nickel::graphics {

Mat44 GPUMesh::GetModelMat() const {
    return CreateTranslation(m_global_transform.p) *
           m_global_transform.q.ToMat();
}

void Scene::updateTransform(const Transform& transform) {
    for (auto& mesh : m_nodes) {
        preorderGPUMesh(transform, *mesh);
    }
}

void Scene::preorderGPUMesh(const Transform& parent_transform, GPUMesh& mesh) {
    mesh.m_global_transform = parent_transform * mesh.m_transform;

    for (auto& child : mesh.m_children) {
        preorderGPUMesh(mesh.m_global_transform, *child);
    }
}

}  // namespace nickel::graphics