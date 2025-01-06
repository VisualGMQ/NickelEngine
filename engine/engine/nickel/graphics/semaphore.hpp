#pragma once

namespace nickel::graphics {

class SemaphoreImpl;

class Semaphore {
public:
    explicit Semaphore(SemaphoreImpl*);
    Semaphore(const Semaphore&);
    Semaphore(Semaphore&&) noexcept;
    Semaphore& operator=(const Semaphore&) noexcept;
    Semaphore& operator=(Semaphore&&) noexcept;
    ~Semaphore();

    const SemaphoreImpl& Impl() const noexcept;
    SemaphoreImpl& Impl() noexcept;
    
private:
    SemaphoreImpl* m_impl;
};

}