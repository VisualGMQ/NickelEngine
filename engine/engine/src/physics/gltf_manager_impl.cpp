#include "nickel/graphics/internal/gltf_manager_impl.hpp"

#include "nickel/graphics/internal/gltf_loader.hpp"
#include "nickel/graphics/internal/gltf_model_impl.hpp"
#include "nickel/nickel.hpp"

namespace nickel::graphics {

GLTFManagerImpl::~GLTFManagerImpl() {
    m_models.clear();

    m_model_allocator.FreeAll();
    m_model_resource_allocator.FreeAll();
    m_mtl_allocator.FreeAll();
}

GLTFModel GLTFManagerImpl::Load(const Path& filename) {
    if (auto it = m_models.find(filename); it != m_models.end()) {
        LOGW("{} model already exists", filename);
        return it->second;
    }

    GLTFLoader loader;
    GLTFModel model = loader.Load(
        filename, nickel::Context::GetInst().GetGPUAdapter(), *this);

    if (model) {
        m_models[filename] = model.GetImpl();
        return model;
    }

    LOGE("{} load failed");
    return {};
}

void GLTFManagerImpl::GC() {
    m_model_allocator.GC();
    m_model_resource_allocator.GC();
    m_mtl_allocator.GC();
    // TODO: remove models from m_models;
}

void GLTFManagerImpl::Update() {
    for (auto& [_, model] : m_models) {
        model->UpdateTransform();
    }
}

void GLTFManagerImpl::Remove(GLTFModelImpl& impl) {
    for (auto it = m_models.begin(); it != m_models.end(); it++) {
        if (it->second == &impl) {
            m_models.erase(it);
            return;
        }
    }
}

}  // namespace nickel::graphics