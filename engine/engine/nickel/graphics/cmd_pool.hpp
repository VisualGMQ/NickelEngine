#pragma once
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class CommandPoolImpl;

class NICKEL_API CommandPool {
public:
    CommandPool() = default;
    explicit CommandPool(CommandPoolImpl*);
    CommandPool(const CommandPool&);
    CommandPool(CommandPool&&) noexcept;
    CommandPool& operator=(const CommandPool&) noexcept;
    CommandPool& operator=(CommandPool&&) noexcept;
    ~CommandPool();

    const CommandPoolImpl& Impl() const noexcept;
    CommandPoolImpl& Impl() noexcept;

    operator bool() const noexcept;
    void Release();

    void Reset();

private:
    CommandPoolImpl* m_impl{};
};

}  // namespace nickel::graphics