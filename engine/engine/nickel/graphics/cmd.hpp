#pragma once
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class CommandImpl;
class DeviceImpl;

class NICKEL_API Command {
public:
    explicit Command(CommandImpl*);
    Command(const Command&);
    Command(Command&&) noexcept;
    Command& operator=(const Command&) noexcept;
    Command& operator=(Command&&) noexcept;
    ~Command();

    const CommandImpl& Impl() const noexcept;
    CommandImpl& Impl() noexcept;

    void Release();
    operator bool() const noexcept;

private:
    CommandImpl* m_impl{};
};

}  // namespace nickel::graphics
