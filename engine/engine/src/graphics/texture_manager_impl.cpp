#include "nickel/nickel.hpp"
#include "nickel/graphics/internal/texture_manager_impl.hpp"
#include "nickel/graphics/texture.hpp"

namespace nickel::graphics {
Texture TextureManagerImpl::Load(const std::string& filename, Format format) {
    if (auto it = m_textures.find(filename); it != m_textures.end()) {
        LOGE("texture {} already loaded", filename);
        return {};
    }

    auto impl = m_texture_allocator.Allocate(
        nickel::Context::GetInst().GetGPUAdapter().GetDevice(), filename,
        format);
    auto result = m_textures.emplace(filename, impl);
    if (!result.second) {
        LOGE("texture emplace construct failed");
        return {};
    }

    return result.first->second;
}

Texture TextureManagerImpl::Find(const std::string& filename) {
    if (auto it = m_textures.find(filename); it != m_textures.end()) {
        return it->second;
    }
    return {};
}
}
