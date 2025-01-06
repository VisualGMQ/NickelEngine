#pragma once

namespace nickel::graphics {

class FenceImpl;

class Fence {
public:
    explicit Fence(FenceImpl*);
    Fence(const Fence&);
    Fence(Fence&&) noexcept;
    Fence& operator=(const Fence&) noexcept;
    Fence& operator=(Fence&&) noexcept;
    ~Fence();

    const FenceImpl& Impl() const noexcept;
    FenceImpl& Impl() noexcept;

private:
    FenceImpl* m_impl;
};

}  // namespace nickel::graphics