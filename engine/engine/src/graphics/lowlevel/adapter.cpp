#include "nickel/graphics/lowlevel//adapter.hpp"
#include "nickel/graphics/lowlevel/internal/adapter_impl.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

Adapter::Adapter(const video::Window::Impl& window) : m_impl{std::make_unique<AdapterImpl>(window)} {}

Adapter::~Adapter() {}

Device Adapter::GetDevice() const {
    return m_impl->GetDevice();
}

const AdapterImpl& Adapter::GetImpl() const {
    return *m_impl;
}

const Adapter::Limits& Adapter::GetLimits() const {
    return m_impl->GetLimits();
}

}  // namespace nickel::graphics