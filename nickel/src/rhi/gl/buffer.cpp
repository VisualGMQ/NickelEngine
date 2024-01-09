#include "rhi/gl/buffer.hpp"
#include "rhi/gl/enum_convert.hpp"
#include "rhi/gl/glcall.hpp"

namespace nickel::rhi::gl {

BufferBundle::BufferBundle(BufferUsageFlags usage, uint64_t size)
    : usage_{usage}, glUsage_{BufferUsageFlag2GL(usage)}, size_{size} {
    GL_CALL(glGenBuffers(1, &buffer_));
    bind();
    GL_CALL(glBufferData(Usage(), Size(), nullptr, GL_STATIC_DRAW));
}

BufferBundle::~BufferBundle() {
    GL_CALL(glDeleteBuffers(1, &buffer_));
}

void BufferBundle::bind() const {
    GL_CALL(glBindBuffer(glUsage_, buffer_));
}

void* BufferBundle::Map(uint64_t offset, uint64_t size) {
    bind();
    return glMapBufferRange(glUsage_, offset, size,
                            GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
}

void BufferBundle::Unmap() {
    bind();
    GL_CALL(glUnmapBuffer(glUsage_));
}

}  // namespace nickel::rhi::gl