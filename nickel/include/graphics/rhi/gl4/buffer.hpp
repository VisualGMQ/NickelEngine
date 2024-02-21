#pragma once

#include "graphics/rhi/impl/buffer.hpp"
#include "graphics/rhi/buffer.hpp"
#include "graphics/rhi/gl4/glpch.hpp"

namespace nickel::rhi::gl4 {

class BufferImpl: public rhi::BufferImpl {
public:
    explicit BufferImpl(const Buffer::Descriptor&);
    ~BufferImpl();

    void Bind() const;
    void Unbind() const;

    enum Buffer::MapState MapState() const override;
    uint64_t Size() const override;
    void Unmap() override;
    void MapAsync(Flags<Buffer::Mode>, uint64_t offset, uint64_t size) override;
    void* GetMappedRange() override;
    void* GetMappedRange(uint64_t offset) override;
    void* GetMappedRange(uint64_t offset, uint64_t size) override;
    GLenum Type() const { return type_; }

    GLuint id = 0;

private:
    enum Buffer::MapState mapState_ = Buffer::MapState::Unmapped;
    GLenum type_;
    uint64_t size_;
    void* map_{};
};

}