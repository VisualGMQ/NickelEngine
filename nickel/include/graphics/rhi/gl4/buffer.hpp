#pragma once

#include "graphics/rhi/impl/buffer.hpp"
#include "graphics/rhi/buffer.hpp"
#include "glad/glad.h"

namespace nickel::rhi::gl4 {

class BufferImpl: public rhi::BufferImpl {
public:
    explicit BufferImpl(const Buffer::Descriptor&);
    ~BufferImpl();

    void Bind() const;
    void Unbind() const;

    GLuint id = 0;

private:
    GLenum type_;
};

}