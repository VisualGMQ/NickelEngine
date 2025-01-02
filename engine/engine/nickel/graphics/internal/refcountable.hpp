#pragma once

namespace nickel::graphics {

class RefCountable {
public:
    RefCountable();
    uint32_t Refcount() const noexcept;
    void IncRefcount();
    void DecRefcount();
    bool IsAlive() const noexcept;

private:
    uint32_t m_refcount;
};

}  // namespace nickel::graphics