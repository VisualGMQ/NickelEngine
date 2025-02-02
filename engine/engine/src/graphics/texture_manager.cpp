#include "nickel/graphics/texture_manager.hpp"

#include "nickel/graphics/internal/texture_manager_impl.hpp"
#include "nickel/graphics/texture.hpp"

namespace nickel::graphics {

TextureManager::TextureManager()
    : m_impl{std::make_unique<TextureManagerImpl>()} {}

TextureManager::~TextureManager() {}

Texture TextureManager::Load(const Path& filename, Format format) {
    return m_impl->Load(filename, format);
}

Texture TextureManager::Find(const Path& filename) {
    return m_impl->Find(filename);
}

void TextureManager::GC() {
    m_impl->GC();
}

}  // namespace nickel::graphics
