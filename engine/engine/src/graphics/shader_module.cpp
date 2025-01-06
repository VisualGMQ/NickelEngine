#include "nickel/graphics/shader_module.hpp"
#include "nickel/graphics/internal/shader_module_impl.hpp"

namespace nickel::graphics {

ShaderModule::ShaderModule(ShaderModuleImpl* impl) : m_impl{impl} {}

ShaderModule::ShaderModule(const ShaderModule& o) : m_impl{o.m_impl} {
    if (m_impl) {
        m_impl->IncRefcount();
    }
}

ShaderModule::ShaderModule(ShaderModule&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

ShaderModule& ShaderModule::operator=(const ShaderModule& o) noexcept {
    if (&o != this) {
        if (m_impl) {
            m_impl->DecRefcount();
        }
        m_impl = o.m_impl;
        if (m_impl) {
            m_impl->IncRefcount();
        }
    }
    return *this;
}

ShaderModule& ShaderModule::operator=(ShaderModule&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

ShaderModule::~ShaderModule() {
    if (m_impl) {
        m_impl->DecRefcount();
    }
}

ShaderModule::operator bool() const noexcept {
    return m_impl;
}

const ShaderModuleImpl& ShaderModule::Impl() const noexcept {
    return *m_impl;
}

ShaderModuleImpl& ShaderModule::Impl() noexcept {
    return *m_impl;
}


}  // namespace nickel::graphics