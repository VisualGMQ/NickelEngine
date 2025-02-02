#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/graphics/texture.hpp"
#include "nickel/graphics/internal/texture_impl.hpp"
#include "nickel/graphics/lowlevel/enums.hpp"

namespace nickel::graphics {

class TextureManagerImpl {
public:
    Texture Load(const std::string& filename, Format format);
    Texture Find(const std::string& filename);

private:
    std::unordered_map<std::string, Texture> m_textures;
    BlockMemoryAllocator<TextureImpl> m_texture_allocator;
};


}
