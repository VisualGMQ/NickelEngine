#pragma once
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class Device;
class MemoryImpl;

class NICKEL_API Memory {
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

    void Release();

private:
    MemoryImpl* m_impl{};
};

}  // namespace nickel::graphics