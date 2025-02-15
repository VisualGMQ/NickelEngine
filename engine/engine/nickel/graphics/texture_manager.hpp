#pragma once
#include "nickel/fs/path.hpp"

#include <memory>

#include "nickel/graphics/texture.hpp"
#include "nickel/graphics/lowlevel/enums.hpp"

namespace nickel::graphics {

class TextureManagerImpl;

class TextureManager {
public:
    TextureManager();
    ~TextureManager();
    Texture Load(const Path& filename, Format format);
    Texture Find(const Path& filename);
    void GC();
    
private:
    std::unique_ptr<TextureManagerImpl> m_impl;
};

}
