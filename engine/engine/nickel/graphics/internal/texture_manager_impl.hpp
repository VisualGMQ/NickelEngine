#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/fs/path.hpp"
#include "nickel/graphics/internal/texture_impl.hpp"
#include "nickel/graphics/lowlevel/enums.hpp"
#include "nickel/graphics/texture.hpp"

namespace nickel::graphics {

class TextureManagerImpl {
public:
    Texture Load(const Path& filename, Format format);
    Texture Find(const Path& filename);
    void GC();

private:
    std::unordered_map<Path, Texture> m_textures;
    BlockMemoryAllocator<TextureImpl> m_texture_allocator;
};

}  // namespace nickel::graphics
