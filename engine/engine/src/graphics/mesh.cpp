#include "nickel/graphics/mesh.hpp"

namespace nickel::graphics {

void Scene::updateTransform(const Transform& transform) {
    auto mat = transform.ToMat();
    for (auto& mesh : m_nodes) {
        preorderGPUMesh(mat, *mesh);
    }
}

void Scene::preorderGPUMesh(const Mat44& parent_mat, GPUMesh& mesh) {
    mesh.m_model_mat = parent_mat * mesh.m_local_model_mat;

    for (auto& child : mesh.m_children) {
        preorderGPUMesh(mesh.m_model_mat, *child);
    }
}

}  // namespace nickel::graphics