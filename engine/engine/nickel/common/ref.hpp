#pragma once
#include "nickel/common/memory/refcountable.hpp"

namespace nickel {

template <typename T>
class Ref {
public:
    Ref() = default;
    Ref(T* payload) : m_payload{payload} {
        if (m_payload && m_payload->m_impl) {
            ((RefCountable*)m_payload->m_impl)->IncRefcount();
        }
    }

    Ref(const Ref& o) : m_payload{o.m_payload} {
        if (m_payload && m_payload->m_impl) {
            ((RefCountable*)m_payload->m_impl)->IncRefcount();
        }
    }

    Ref(Ref&& o) noexcept : m_payload{o.m_payload} { o.m_payload = nullptr; }

    Ref& operator=(const Ref& o) {
        if (&o != this) {
            if (m_payload && m_payload->m_impl) {
                ((RefCountable*)m_payload->m_impl)->DecRefcount();
            }
            m_payload = o.m_payload;
            if (m_payload && m_payload->m_impl) {
                ((RefCountable*)m_payload->m_impl)->IncRefcount();
            }
        }
        return *this;
    }

    Ref& operator=(Ref&& o) noexcept {
        if (&o != this) {
            m_payload = o.m_payload;
            o.m_payload = nullptr;
        }
        return *this;
    }

    virtual ~Ref() {
        if (m_payload && m_payload->m_impl) {
            ((RefCountable*)m_payload->m_impl)->DecRefcount();
        }
    }

    const T* GetPayload() const noexcept { return m_payload; }

    T* GetPayload() noexcept { return m_payload; }

    operator bool() const noexcept { return m_payload; }

    T* operator->() { return m_payload; }

    T* operator->() const { return m_payload; }

private:
    T* m_payload{};
};

}  // namespace nickel