#include "nickel/graphics/gltf.hpp"

#include "nickel/graphics/internal/gltf_manager_impl.hpp"
#include "nickel/graphics/internal/gltf_model_impl.hpp"
#include "nickel/graphics/internal/material3d_impl.hpp"
#include "nickel/graphics/texture_manager.hpp"

namespace nickel::graphics {

void GLTFModel::Move(const Vec3& offset) {
    m_impl->Move(offset);
}

void GLTFModel::Scale(const Vec3& delta) {
    m_impl->Scale(delta);
}

void GLTFModel::Rotate(const Quat& delta) {
    m_impl->Rotate(delta);
}

void GLTFModel::MoveTo(const Vec3& position) {
    m_impl->MoveTo(position);
}

void GLTFModel::ScaleTo(const Vec3& scale) {
    m_impl->ScaleTo(scale);
}

void GLTFModel::RotateTo(const Quat& q) {
    m_impl->RotateTo(q);
}

void GLTFModel::UpdateTransform() {
    m_impl->UpdateTransform();
}

GLTFModel GLTFManager::Load(const Path& filename) {
    return m_impl->Load(filename);
}

void GLTFManager::GC() {
    m_impl->GC();
}

GLTFManager::GLTFManager() : m_impl{std::make_unique<GLTFManagerImpl>()} {}

GLTFManager::~GLTFManager() {}

void GLTFManager::Update() {
    m_impl->Update();
}

}  // namespace nickel::graphics
