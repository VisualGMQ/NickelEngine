#pragma once
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class CommandEncoderImpl;
class DeviceImpl;

class NICKEL_API Command {
public:
    explicit Command(CommandEncoderImpl&);
    ~Command();

    const CommandEncoderImpl& Impl() const noexcept;
    CommandEncoderImpl& Impl() noexcept;

private:
    CommandEncoderImpl* m_impl{};
};

}  // namespace nickel::graphics
