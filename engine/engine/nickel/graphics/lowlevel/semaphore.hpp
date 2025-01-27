#pragma once
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class SemaphoreImpl;

class NICKEL_API Semaphore {
public:
    Semaphore() = default;
    explicit Semaphore(SemaphoreImpl*);
    Semaphore(const Semaphore&);
    Semaphore(Semaphore&&) noexcept;
    Semaphore& operator=(const Semaphore&) noexcept;
    Semaphore& operator=(Semaphore&&) noexcept;
    ~Semaphore();

    operator bool() const noexcept;

    const SemaphoreImpl& Impl() const noexcept;
    SemaphoreImpl& Impl() noexcept;
    void Release();
    
private:
    SemaphoreImpl* m_impl{};
};

}