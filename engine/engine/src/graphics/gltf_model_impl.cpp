#include "nickel/graphics/internal/gltf_model_impl.hpp"

namespace nickel::graphics {

GLTFModelImpl::~GLTFModelImpl() {
    materials.clear();
    m_mtl_allocator.FreeAll();
}

void GLTFModelImpl::Move(const Vec3& offset) {
    m_transform.p += offset;
    m_should_update_transform = true;
}

void GLTFModelImpl::Scale(const Vec3& delta) {
    m_transform.scale += delta;
    m_should_update_transform = true;
}

void GLTFModelImpl::Rotate(const Quat& delta) {
    m_transform.q *= delta;
    m_should_update_transform = true;
}

void GLTFModelImpl::MoveTo(const Vec3& position) {
    m_transform.p = position;
    m_should_update_transform = true;
}

void GLTFModelImpl::ScaleTo(const Vec3& scale) {
    m_transform.scale = scale;
    m_should_update_transform = true;
}

void GLTFModelImpl::RotateTo(const Quat& q) {
    m_transform.q = q;
    m_should_update_transform = true;
}

void GLTFModelImpl::UpdateTransform() {
    if (m_should_update_transform) {
        tryUpdateSceneTransform();
    }
    m_should_update_transform = false;
}

void GLTFModelImpl::tryUpdateSceneTransform() {
    for (auto& scene : scenes) {
        scene.updateTransform(m_transform);
    }
}

}  // namespace nickel::graphics