#include "nickel/graphics/internal/gltf_model_impl.hpp"

#include "nickel/graphics/internal/gltf_manager_impl.hpp"
#include "nickel/graphics/internal/material3d_impl.hpp"

namespace nickel::graphics {

GLTFModelResourceImpl::GLTFModelResourceImpl(GLTFManagerImpl* mgr)
    : m_mgr{mgr} {}

void GLTFModelResourceImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_mgr->m_model_resource_allocator.MarkAsGarbage(this);
    }
}

GLTFModelImpl::GLTFModelImpl(GLTFManagerImpl* mgr, GLTFModelResource&& resource,
                             std::unique_ptr<Mesh>&& mesh)
    : m_resource{std::move(resource)}, m_mesh{std::move(mesh)}, m_mgr{mgr} {}

void GLTFModelImpl::Move(const Vec3& offset) {
    m_mesh->m_transform.p += offset;
    m_should_update_transform = true;
}

void GLTFModelImpl::Scale(const Vec3& delta) {
    m_mesh->m_transform.scale += delta;
    m_should_update_transform = true;
}

void GLTFModelImpl::Rotate(const Quat& delta) {
    m_mesh->m_transform.q *= delta;
    m_should_update_transform = true;
}

void GLTFModelImpl::MoveTo(const Vec3& position) {
    m_mesh->m_transform.p = position;
    m_should_update_transform = true;
}

void GLTFModelImpl::ScaleTo(const Vec3& scale) {
    m_mesh->m_transform.scale = scale;
    m_should_update_transform = true;
}

void GLTFModelImpl::RotateTo(const Quat& q) {
    m_mesh->m_transform.q = q;
    m_should_update_transform = true;
}

void GLTFModelImpl::UpdateTransform() {
    if (m_should_update_transform) {
        tryUpdateSceneTransform();
    }
    m_should_update_transform = false;
}

void GLTFModelImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_mgr->m_model_allocator.MarkAsGarbage(this);
        m_mgr->Remove(*this);
    }
}

void GLTFModelImpl::tryUpdateSceneTransform() {
    m_mesh->updateTransform({});
}

}  // namespace nickel::graphics