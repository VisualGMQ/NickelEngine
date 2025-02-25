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

    void RemoveTexture(TextureImpl* texture);

    BlockMemoryAllocator<TextureImpl> m_allocator;

private:
    std::unordered_map<Path, TextureImpl*> m_textures;
};

}  // namespace nickel::graphics
