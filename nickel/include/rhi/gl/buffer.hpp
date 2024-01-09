#pragma once

#include "pch.hpp"
#include "rhi/buffer.hpp"
#include "rhi/enums.hpp"

namespace nickel::rhi::gl {

class BufferBundle : public rhi::BufferBundle {
public:
    explicit BufferBundle(BufferUsageFlags, uint64_t size);
    BufferBundle(const BufferBundle&) = delete;
    BufferBundle& operator=(const BufferBundle&) = delete;

    BufferBundle(BufferBundle&& o) { swap(*this, o); }

    BufferBundle& operator=(BufferBundle&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    ~BufferBundle();

    GLuint Raw() const { return buffer_; }

    BufferUsageFlags Usage() const noexcept override { return usage_; }

    uint64_t Size() const noexcept override { return size_; }

    void* Map(uint64_t offset, uint64_t size) override;
    void Unmap() override;

private:
    GLuint buffer_ = 0;
    BufferUsageFlags usage_;
    GLenum glUsage_;
    uint64_t size_ = 0;

    void bind() const;

    friend void swap(BufferBundle& o1, BufferBundle& o2) noexcept {
        using std::swap;
        swap(o1.buffer_, o2.buffer_);
        swap(o1.usage_, o2.usage_);
        swap(o1.size_, o2.size_);
    }
};

}  // namespace nickel::rhi::gl