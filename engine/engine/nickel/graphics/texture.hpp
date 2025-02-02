#pragma once

namespace nickel::graphics {

class TextureImpl;

class Texture {
public:
    Texture() = default;
    Texture(TextureImpl* impl);

    TextureImpl& GetImpl() noexcept;
    const TextureImpl& GetImpl() const noexcept;

    operator bool() const noexcept;
    
private:
    TextureImpl* m_impl{};
};

}