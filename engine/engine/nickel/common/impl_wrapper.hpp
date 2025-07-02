#pragma once
#include "nickel/common/memory/refcountable.hpp"

namespace nickel {

template <typename ImplT>
class ImplWrapper {
public:
    template <typename T>
    friend class Ref;

    ImplWrapper() = default;

    ImplWrapper(ImplT* impl) : m_impl{impl} {}

    ImplWrapper(const ImplWrapper& o) : m_impl{o.m_impl} {
        if (m_impl) {
            ((RefCountable*)m_impl)->IncRefcount();
        }
    }

    ImplWrapper(ImplWrapper&& o) noexcept : m_impl{o.m_impl} {
        o.m_impl = nullptr;
    }

    ImplWrapper& operator=(const ImplWrapper& o) {
        if (o.m_impl != m_impl) {
            if (m_impl) {
                ((RefCountable*)m_impl)->DecRefcount();
            }
            m_impl = o.m_impl;
            if (m_impl) {
                ((RefCountable*)m_impl)->IncRefcount();
            }
        }
        return *this;
    }

    ImplWrapper& operator=(ImplWrapper&& o) noexcept {
        if (&o == this) {
            return *this;
        }
        if (o.m_impl != m_impl) {
            if (m_impl) {
                ((RefCountable*)m_impl)->DecRefcount();
            }
            m_impl = o.m_impl;
        }
        o.m_impl = nullptr;
        return *this;
    }

    virtual ~ImplWrapper() {
        if (m_impl) {
            ((RefCountable*)m_impl)->DecRefcount();
        }
    }

    const ImplT* GetImpl() const noexcept { return m_impl; }

    ImplT* GetImpl() noexcept { return m_impl; }

    operator bool() const noexcept { return m_impl; }

protected:
    ImplT* m_impl{};
};

}  // namespace nickel