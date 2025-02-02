#include "nickel/graphics/texture_manager.hpp"

#include "nickel/nickel.hpp"
#include "nickel/common/memory/memory.hpp"
#include "nickel/graphics/texture.hpp"
#include "nickel/graphics/internal/texture_manager_impl.hpp"

namespace nickel::graphics {

TextureManager::TextureManager()
    : m_impl{std::make_unique<TextureManagerImpl>()} {
}

TextureManager::~TextureManager() { }

Texture TextureManager::Load(const std::string& filename, Format format) {
    return m_impl->Load(filename, format);
}

Texture TextureManager::Find(const std::string& filename) {
    return m_impl->Find(filename);
}

}
