#include "nickel/graphics/internal/texture_manager_impl.hpp"
#include "nickel/graphics/texture.hpp"
#include "nickel/nickel.hpp"

namespace nickel::graphics {
Texture TextureManagerImpl::Load(const Path& filename, Format format) {
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

Texture TextureManagerImpl::Find(const Path& filename) {
    if (auto it = m_textures.find(filename); it != m_textures.end()) {
        return it->second;
    }
    return {};
}

void TextureManagerImpl::GC() {
    // TODO: remove texture from manager
    m_texture_allocator.GC();
}
}  // namespace nickel::graphics
