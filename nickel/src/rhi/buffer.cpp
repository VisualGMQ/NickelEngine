#include "rhi/buffer.hpp"

namespace nickel::rhi {

/*
MemoryGuard::MemoryGuard(DeviceMemory& mem, void* memory)
    : mem_{&mem}, memory{memory} {}

MemoryGuard::MemoryGuard(DeviceMemory& mem, uint64_t offset, uint64_t size)
    : mem_{&mem} {
    memory = mem_->map(offset, size);
}

MemoryGuard::~MemoryGuard() {
    Unmap();
}

void MemoryGuard::Unmap() {
    if (memory) {
        mem_->unmap();
        memory = nullptr;
    }
}
*/

}