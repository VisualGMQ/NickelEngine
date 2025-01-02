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
    size_t Size() const noexcept;
    
private:
    MemoryImpl* m_impl;
};

}