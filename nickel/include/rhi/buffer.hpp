#pragma once

#include <cstdint>
#include <utility>
#include "rhi/enums.hpp"

namespace nickel::rhi {

/*
struct MemoryGuard {
    void* memory = nullptr;

    MemoryGuard(DeviceMemory& mem, uint64_t offset, uint64_t size);
    MemoryGuard(DeviceMemory& mem, void* memory);
    MemoryGuard(const MemoryGuard&) = delete;
    MemoryGuard& operator=(const MemoryGuard&) = delete;

    MemoryGuard(MemoryGuard&& o) { swap(*this, o); }

    MemoryGuard& operator=(MemoryGuard&& o) {
        if (this != &o) {
            swap(*this, o);
        }
        return *this;
    }

    ~MemoryGuard();

    void Unmap();

private:
    DeviceMemory* mem_{};

    friend void swap(MemoryGuard& o1, MemoryGuard& o2) {
        using std::swap;
        swap(o1.memory, o2.memory);
        swap(o1.mem_, o2.mem_);
    }
};
*/

class BufferBundle {
public:
    virtual ~BufferBundle() = default;

    virtual BufferUsageFlags Usage() const noexcept = 0;

    virtual uint64_t Size() const noexcept = 0;

    virtual void* Map(uint64_t offset, uint64_t size) = 0;
    virtual void Unmap() = 0;
};

}