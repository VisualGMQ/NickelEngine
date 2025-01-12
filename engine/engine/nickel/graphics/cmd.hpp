#pragma once
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class CommandImpl;
class DeviceImpl;

class NICKEL_API Command {
public:
    explicit Command(CommandImpl&);
    ~Command();

    const CommandImpl& Impl() const noexcept;
    CommandImpl& Impl() noexcept;

private:
    CommandImpl* m_impl{};
};

}  // namespace nickel::graphics
