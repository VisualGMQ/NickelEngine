#include "graphics/rhi/buffer.hpp"
#include "graphics/rhi/vk/adapter.hpp"
#include "graphics/rhi/vk/buffer.hpp"
#include "graphics/rhi/vk/device.hpp"

namespace nickel::rhi {

Buffer::Buffer(AdapterImpl& adapter, DeviceImpl& device,
               const Buffer::Descriptor& desc) {
    switch (adapter.RequestAdapterInfo().api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
            break;
        case APIPreference::Vulkan:
            impl_ = new vulkan::BufferImpl(
                static_cast<vulkan::DeviceImpl&>(device),
                static_cast<vulkan::AdapterImpl&>(adapter).phyDevice, desc);
            break;
    }
}

void Buffer::Destroy() {
    delete impl_;
    impl_ = nullptr;
}

enum Buffer::MapState Buffer::MapState() const {
    return impl_->MapState();
}

uint64_t Buffer::Size() const {
    return impl_->Size();
}

void Buffer::Unmap() {
    return impl_->Unmap();
}

void Buffer::MapAsync(Mode mode, uint64_t offset, uint64_t size) {
    return impl_->MapAsync(mode, offset, size);
}

void* Buffer::GetMappedRange() {
    return impl_->GetMappedRange();
}

void* Buffer::GetMappedRange(uint64_t offset) {
    return impl_->GetMappedRange(offset);
}

void* Buffer::GetMappedRange(uint64_t offset, uint64_t size) {
    return impl_->GetMappedRange(offset, size);
}

}  // namespace nickel::rhi