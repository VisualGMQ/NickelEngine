#include "nickel/common/memory/refcountable.hpp"

namespace nickel {

RefCountable::RefCountable() : m_refcount{1} {}

uint32_t RefCountable::Refcount() const noexcept {
    return m_refcount;
}

void RefCountable::IncRefcount() {
    if (m_refcount > 0) {
        m_refcount++;
    }
}

void RefCountable::DecRefcount() {
    if (m_refcount > 0) {
        m_refcount--;
    }
}

bool RefCountable::IsAlive() const noexcept {
    return m_refcount > 0;
}

}  // namespace nickel::graphics