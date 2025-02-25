#include "nickel/graphics/internal/texture_manager_impl.hpp"
#include "nickel/nickel.hpp"

namespace nickel::graphics {
Texture TextureManagerImpl::Load(const Path& filename, Format format) {
    if (auto it = m_textures.find(filename); it != m_textures.end()) {
        LOGE("texture {} already loaded", filename);
        return {};
    }

    auto result = m_textures.emplace(
        filename,
        m_allocator.Allocate(
            this, nickel::Context::GetInst().GetGPUAdapter().GetDevice(),
            filename, format));
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
    m_allocator.GC();
}

void TextureManagerImpl::RemoveTexture(TextureImpl* texture) {
    for (auto it = m_textures.begin(); it != m_textures.end(); ++it) {
        if (it->second == texture) {
            m_textures.erase(it);
            return;
        }
    }
}

}  // namespace nickel::graphics
