#pragma once
#include "nickel/common/macro.hpp"
#include "nickel/common/memory/refcountable.hpp"

namespace nickel {

template <typename T, typename Allocator>
class Handle {
public:
    Handle() = default;

    Handle(T* payload, Allocator& allocator)
        : m_payload{payload}, m_allocator{&allocator} {}

    Handle(const Handle& o) {
        decAndTryRecycle();

        m_payload = o.m_payload;
        m_allocator = o.m_allocator;
        tryIncRefcount();
    }

    Handle(Handle&& o) noexcept {
        decAndTryRecycle();
        m_payload = o.m_payload;
        m_allocator = o.m_allocator;

        o.m_payload = nullptr;
        o.m_allocator = nullptr;
    }

    Handle& operator=(const Handle& o) {
        if (this != &o) {
            return *this;
        }

        decAndTryRecycle();
        m_payload = o.m_payload;
        m_allocator = o.m_allocator;
        tryIncRefcount();
        return *this;
    }

    Handle& operator=(Handle&& o) noexcept {
        if (this != &o) {
            return *this;
        }

        decAndTryRecycle();
        m_payload = o.m_payload;
        m_allocator = o.m_allocator;
        o.m_payload = nullptr;
        o.m_allocator = nullptr;
        return *this;
    }

    operator bool() const noexcept { return m_payload && m_allocator; }

    T* operator->() noexcept { return m_payload; }

    const T* operator->() const noexcept { return m_payload; }

    const T* Payload() const { return m_payload; }

    T* Payload() { return m_payload; }

private:
    T* m_payload{};
    Allocator* m_allocator{};

    void tryIncRefcount() const {
        if (m_payload) {
            ((RefCountable*)m_payload)->IncRefcount();
        }
    }

    void decAndTryRecycle() {
        NICKEL_RETURN_IF_FALSE(m_payload);

        RefCountable* refcount = (RefCountable*)m_payload;
        if (refcount->Refcount() > 0) {
            refcount->DecRefcount();
            if (refcount->Refcount() == 0) {
                NICKEL_RETURN_IF_FALSE(m_allocator);
                m_allocator->MarkAsGarbage(m_payload);
                m_payload = nullptr;
            }
        }
    }
};

}  // namespace nickel