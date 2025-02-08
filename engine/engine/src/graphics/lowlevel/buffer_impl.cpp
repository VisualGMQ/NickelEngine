#include "nickel/graphics/lowlevel/internal/buffer_impl.hpp"
#include "nickel/graphics/lowlevel/buffer.hpp"
#include "nickel/graphics/lowlevel/internal/common.hpp"
#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/enum_convert.hpp"
#include "nickel/graphics/lowlevel/internal/memory_impl.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"

namespace nickel::graphics {

BufferImpl::BufferImpl(DeviceImpl& dev, VkPhysicalDevice phyDev,
                       const Buffer::Descriptor& desc)
    : m_device{dev} {
    std::vector<uint32_t> indices;
    if (dev.m_queue_indices.HasSeparateQueue()) {
        indices.emplace_back(dev.m_queue_indices.m_graphics_index.value());
        indices.emplace_back(dev.m_queue_indices.m_present_index.value());
    } else {
        indices.emplace_back(dev.m_queue_indices.m_graphics_index.value());
    }

    m_size = desc.m_size;
    createBuffer(dev, desc);
    allocateMem(
        dev, phyDev,
        getMemoryProperty(phyDev, desc));

    VK_CALL(vkBindBufferMemory(dev.m_device, m_buffer, m_memory->m_memory, 0));
}

void BufferImpl::createBuffer(DeviceImpl& device,
                              const Buffer::Descriptor& desc) {
    std::vector<uint32_t> indices;
    if (device.m_queue_indices.m_graphics_index ==
        device.m_queue_indices.m_present_index) {
        indices.push_back(device.m_queue_indices.m_graphics_index.value());
    } else {
        indices.push_back(device.m_queue_indices.m_graphics_index.value());
        indices.push_back(device.m_queue_indices.m_present_index.value());
    }

    VkBufferCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    ci.size = desc.m_size;
    ci.usage = static_cast<VkBufferUsageFlagBits>(BufferUsage2Vk(desc.m_usage));
    ci.queueFamilyIndexCount = indices.size();
    ci.pQueueFamilyIndices = indices.data();
    if (indices.size() > 1) {
        ci.sharingMode = VK_SHARING_MODE_CONCURRENT;
    } else {
        ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    VK_CALL(vkCreateBuffer(device.m_device, &ci, nullptr, &m_buffer));
}

void BufferImpl::allocateMem(DeviceImpl& device, VkPhysicalDevice phyDevice,
                             VkMemoryPropertyFlags flags) {
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(device.m_device, m_buffer, &requirements);
    auto type = FindMemoryType(phyDevice, requirements, flags);

    if (!type) {
        LOGE("find corresponding memory type failed");
    } else {
        m_memory = new (std::nothrow) MemoryImpl{
            m_device, static_cast<uint64_t>(requirements.size), type.value()};
    }
}

VkMemoryPropertyFlags BufferImpl::getMemoryProperty(
    VkPhysicalDevice phyDevice, const Buffer::Descriptor& desc) {
    VkMemoryPropertyFlags mem_props{};
    switch (desc.m_memory_type) {
        case MemoryType::CPULocal:
            mem_props = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            break;
        case MemoryType::Coherence: {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(phyDevice, &props);
            if (props.limits.nonCoherentAtomSize == 0) {
                LOGW("your GPU don't support coherence memory type");
                mem_props = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
                m_is_mapping_coherence = false;
            } else {
                mem_props = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                m_is_mapping_coherence = true;
            }
        } break;
        case MemoryType::GPULocal:
            mem_props = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
    }
    return mem_props;
}

BufferImpl::~BufferImpl() {
    if (m_map_state != Buffer::MapState::Unmapped) {
        Unmap();
    }
    delete m_memory;
    vkDestroyBuffer(m_device.m_device, m_buffer, nullptr);
}

enum Buffer::MapState BufferImpl::MapState() const {
    return m_map_state;
}

uint64_t BufferImpl::Size() const {
    return m_size;
}

void BufferImpl::Unmap() {
    if (m_map_state == Buffer::MapState::Mapped) {
        Flush();
        vkUnmapMemory(m_device.m_device, m_memory->m_memory);
        m_map_state = Buffer::MapState::Unmapped;
        m_map = nullptr;
    }
}

void BufferImpl::MapAsync(uint64_t offset, uint64_t size) {
    if (m_map_state == Buffer::MapState::Unmapped) {
        VK_CALL(vkMapMemory(m_device.m_device, m_memory->m_memory, offset, size, 0, &m_map));
        if (m_map) {
            m_mapped_offset = offset;
            m_mapped_size = size;
            m_map_state = Buffer::MapState::Mapped;
        }
    }
}

void BufferImpl::MapAsync() {
    if (m_map_state == Buffer::MapState::Unmapped) {
        VK_CALL(
            vkMapMemory(m_device.m_device, m_memory->m_memory, 0, m_size, 0, &
                m_map));
        if (m_map) {
            m_mapped_offset = 0;
            m_mapped_size = m_size;
            m_map_state = Buffer::MapState::Mapped;
        }
    }
}


void* BufferImpl::GetMappedRange() {
    return GetMappedRange(0);
}

void* BufferImpl::GetMappedRange(uint64_t offset) {
    return (char*)m_map + offset;
}

void BufferImpl::Flush() {
    if (!m_is_mapping_coherence) {
        VkMappedMemoryRange range{};
        range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        range.memory = m_memory->m_memory;
        range.offset = m_mapped_offset;
        range.size = m_mapped_size;
        VK_CALL(vkFlushMappedMemoryRanges(m_device.m_device, 1, &range));
    }
}

void BufferImpl::Flush(uint64_t offset, uint64_t size) {
    VkMappedMemoryRange range{};
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = m_memory->m_memory;
    range.offset = offset;
    range.size = size;
    VK_CALL(vkFlushMappedMemoryRanges(m_device.m_device, 1, &range));
}

void BufferImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_device.m_pending_delete_buffers.push_back(this);
    }
}

void BufferImpl::BuffData(void* data, size_t size, size_t offset) {
    Buffer::Descriptor desc;
    desc.m_memory_type = MemoryType::Coherence;
    desc.m_size = size;
    desc.m_usage = BufferUsage::CopySrc;
    Buffer buffer = m_device.CreateBuffer(desc);
    buffer.MapAsync(0, size);
    void* map = buffer.GetMappedRange();
    memcpy(map, data, size);
    buffer.Unmap();

    CommandEncoder encoder = m_device.CreateCommandEncoder();
    CopyEncoder copy_encoder = encoder.BeginCopy();
    copy_encoder.copyBufferToBuffer(buffer.Impl(), 0, *this, offset, size);
    copy_encoder.End();
    Command cmd = encoder.Finish();

    m_device.Submit(cmd, {}, {}, {});
    m_device.WaitIdle();
}
} // namespace nickel::graphics
