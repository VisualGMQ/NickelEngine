#include "nickel/graphics/mesh.hpp"

namespace nickel::graphics {

Mat44 Mesh::GetModelMat() const {
    return CreateTranslation(m_global_transform.p) *
           m_global_transform.q.ToMat();
}

void Mesh::updateTransform(const Transform& parent_transform) {
    m_transform = parent_transform * m_transform;
    for (auto& mesh : m_children) {
        preorderGPUMesh(m_transform, *mesh);
    }
}

void Mesh::preorderGPUMesh(const Transform& parent_transform, Mesh& mesh) {
    mesh.m_global_transform = parent_transform * mesh.m_transform;

    for (auto& child : mesh.m_children) {
        preorderGPUMesh(mesh.m_global_transform, *child);
    }
}

}  // namespace nickel::graphics