#include "nickel/graphics/device.hpp"
#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

Device::Device(const Adapter::Impl& ctx_impl): m_impl{std::make_unique<Impl>(ctx_impl)} {}

Device::~Device() {}

}  // namespace nickel::graphics