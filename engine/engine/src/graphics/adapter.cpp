#include "nickel/graphics/adapter.hpp"
#include "nickel/graphics/internal/adapter_impl.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

Adapter::Adapter(const video::Window::Impl& window) : m_impl{std::make_unique<Impl>(window)} {}

Adapter::~Adapter() {}

const Adapter::Impl& Adapter::GetImpl() const {
    return *m_impl;
}

}  // namespace nickel::graphics