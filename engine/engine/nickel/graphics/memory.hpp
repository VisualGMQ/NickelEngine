#pragma once

namespace nickel::graphics {

class Device;
class MemoryImpl;

class Memory {
public:
    explicit Memory(MemoryImpl*);
    Memory(const Memory&) noexcept;
    Memory(Memory&&) noexcept;
    Memory& operator=(const Memory&) noexcept;
    Memory& operator=(Memory&&) noexcept;
    ~Memory();

    const MemoryImpl& Impl() const noexcept;
    MemoryImpl& Impl() noexcept;

    operator bool() const noexcept;

    size_t Size() const noexcept;

private:
    MemoryImpl* m_impl;
};

}  // namespace nickel::graphics