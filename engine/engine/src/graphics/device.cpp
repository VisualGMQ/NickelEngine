#include "nickel/graphics/device.hpp"
#include "nickel/graphics/adapter.hpp"
#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

Device::Device(const AdapterImpl& adapter_impl,
               const SVector<uint32_t, 2>& window_size)
    : m_impl{std::make_unique<DeviceImpl>(adapter_impl, window_size)} {}

Device::~Device() {}

DeviceImpl& Device::Impl() {
    return *m_impl;
}

const DeviceImpl& Device::Impl() const {
    return *m_impl;
}

}  // namespace nickel::graphics