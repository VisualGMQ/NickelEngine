#pragma once

#include "graphics/rhi/impl/buffer.hpp"
#include "graphics/rhi/buffer.hpp"
#include "graphics/rhi/gl/glpch.hpp"
#include <optional>

namespace nickel::rhi::gl {

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
    void Flush() override;
    void Flush(uint64_t offset, uint64_t size) override;
    bool IsMappingCoherence() const override;
    GLenum Type() const { return type_; }

    GLuint id = 0;

private:
    enum Buffer::MapState mapState_ = Buffer::MapState::Unmapped;
    GLenum type_;
    BufferUsage usage_;
    uint64_t size_;
    uint64_t mappedOffset_;
    uint64_t mappedSize_;
    std::optional<bool> isMappingCoherence_;
    void* map_{};
};

}