#include "graphics/rhi/gl4/buffer.hpp"
#include "graphics/rhi/gl4/glcall.hpp"

namespace nickel::rhi::gl4 {

GLenum getBufferType(const Buffer::Descriptor& desc) {
    if (desc.usage & BufferUsage::Vertex) {
        return GL_ARRAY_BUFFER;
    }
    if (desc.usage & BufferUsage::Index) {
        return GL_ELEMENT_ARRAY_BUFFER;
    }
    if (desc.usage & BufferUsage::Indirect) {
        return GL_DISPATCH_INDIRECT_BUFFER;
    }
    if (desc.usage & BufferUsage::Storage)  {
        return GL_SHADER_STORAGE_BUFFER;
    }
    if (desc.usage & BufferUsage::Uniform) {
        return GL_UNIFORM_BUFFER;
    }
    if (desc.usage & BufferUsage::QueryResolve) {
        return GL_QUERY_BUFFER;
    }
    return GL_ARRAY_BUFFER;
}

BufferImpl::BufferImpl(const Buffer::Descriptor& desc): type_{getBufferType(desc)} {
    GL_CALL(glGenBuffers(1, &id));

}

BufferImpl::~BufferImpl() {
    GL_CALL(glDeleteBuffers(1, &id));
}

void BufferImpl::Bind() const {
    GL_CALL(glBindBuffer(type_, id));
}

void BufferImpl::Unbind() const {
    GL_CALL(glBindBuffer(type_, 0));
}

}