#pragma once
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class FenceImpl;

class NICKEL_API Fence {
public:
    Fence() = default;
    explicit Fence(FenceImpl*);
    Fence(const Fence&);
    Fence(Fence&&) noexcept;
    Fence& operator=(const Fence&) noexcept;
    Fence& operator=(Fence&&) noexcept;
    ~Fence();

    const FenceImpl& Impl() const noexcept;
    FenceImpl& Impl() noexcept;

    operator bool() const noexcept;
    void Release();

private:
    FenceImpl* m_impl{};
};

}  // namespace nickel::graphics