#include "nickel/graphics/gltf.hpp"

#include "nickel/graphics/internal/gltf_manager_impl.hpp"
#include "nickel/graphics/internal/gltf_model_impl.hpp"
#include "nickel/graphics/internal/material3d_impl.hpp"
#include "nickel/graphics/texture_manager.hpp"

namespace nickel::graphics {

bool GLTFManager::Load(const Path& filename,
                       const GLTFLoadConfig& load_config) {
    return m_impl->Load(filename, load_config);
}

GLTFModel GLTFManager::Find(const std::string& name) {
    return m_impl->Find(name);
}

void GLTFManager::GC() {
    m_impl->GC();
}

GLTFManager::GLTFManager() : m_impl{std::make_unique<GLTFManagerImpl>()} {}

GLTFManager::~GLTFManager() {}

}  // namespace nickel::graphics
