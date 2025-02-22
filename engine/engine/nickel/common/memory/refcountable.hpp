#pragma once
#include <cstdint>

namespace nickel {
class RefCountable {
public:
    RefCountable();
    virtual ~RefCountable() = default;
    uint32_t Refcount() const noexcept;
    virtual void IncRefcount();
    virtual void DecRefcount();
    bool IsAlive() const noexcept;

private:
    uint32_t m_refcount;
};
} // namespace nickel::graphics
