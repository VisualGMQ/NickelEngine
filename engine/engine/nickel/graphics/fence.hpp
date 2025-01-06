#pragma once

namespace nickel::graphics {

class FenceImpl;

class Fence {
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

private:
    FenceImpl* m_impl;
};

}  // namespace nickel::graphics