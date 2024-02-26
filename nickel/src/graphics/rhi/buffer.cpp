#include "graphics/rhi/buffer.hpp"
#ifdef NICKEL_HAS_VULKAN
#include "graphics/rhi/vk/adapter.hpp"
#include "graphics/rhi/vk/buffer.hpp"
#include "graphics/rhi/vk/device.hpp"
#endif
#include "graphics/rhi/null/buffer.hpp"
#include "graphics/rhi/gl4/buffer.hpp"
#include "graphics/rhi/gl4/adapter.hpp"

namespace nickel::rhi {

Buffer::Buffer(AdapterImpl& adapter, DeviceImpl& device,
               const Buffer::Descriptor& desc) {
    switch (adapter.RequestAdapterInfo().api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
            impl_ = new gl4::BufferImpl(desc);
            break;
        case APIPreference::Vulkan:
        #ifdef NICKEL_HAS_VULKAN
            impl_ = new vulkan::BufferImpl(
                static_cast<vulkan::DeviceImpl&>(device),
                static_cast<vulkan::AdapterImpl&>(adapter).phyDevice, desc);
        #endif
            break;
        case APIPreference::Null:
            impl_ = new null::BufferImpl{};
            break;
    }
}

void Buffer::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
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

void Buffer::MapAsync(Flags<Mode> mode, uint64_t offset, uint64_t size) {
    impl_->MapAsync(mode, offset, size);
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

void Buffer::Flush() {
    impl_->Flush();
}

void Buffer::Flush(uint64_t offset, uint64_t size) {
    impl_->Flush(offset, size);
}

bool Buffer::IsMappingCoherence() const {
    return impl_->IsMappingCoherence();
}

}  // namespace nickel::rhi