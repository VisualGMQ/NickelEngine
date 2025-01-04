#include "nickel/graphics/cmd_pool.hpp"

#include "nickel/graphics/internal/cmd_pool_impl.hpp"

namespace nickel::graphics {

CommandPool::CommandPool(CommandPoolImpl* impl) : m_impl{impl} {}

CommandPool::CommandPool(const CommandPool& o) : m_impl{o.m_impl} {
    m_impl->IncRefcount();
}

CommandPool::CommandPool(CommandPool&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

CommandPool& CommandPool::operator=(const CommandPool& o) noexcept {
    if (&o != this) {
        m_impl->DecRefcount();
        m_impl = o.m_impl;
        m_impl->IncRefcount();
    }
    return *this;
}

CommandPool& CommandPool::operator=(CommandPool&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

CommandPool::~CommandPool() {
    m_impl->DecRefcount();
}

const CommandPoolImpl& CommandPool::Impl() const noexcept {
    return *m_impl;
}

CommandPoolImpl& CommandPool::Impl() noexcept {
    return *m_impl;
}

}  // namespace nickel::graphics