#include "nickel/graphics/mesh.hpp"

namespace nickel::graphics {

void Scene::updateTransform(const Transform& transform) {
    auto mat = transform.ToMat();
    for (auto& mesh : nodes) {
        preorderGPUMesh(mat, *mesh);
    }
}

void Scene::preorderGPUMesh(const Mat44& parent_mat, GPUMesh& mesh) {
    mesh.modelMat = parent_mat * mesh.localModelMat;

    for (auto& child : mesh.children) {
        preorderGPUMesh(mesh.modelMat, *child);
    }
}

}  // namespace nickel::graphics