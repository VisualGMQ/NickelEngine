#pragma once
#include "image.hpp"
#include "nickel/common/math/smatrix.hpp"
#include "nickel/graphics/buffer.hpp"
#include "render_pass.hpp"

namespace nickel::graphics {

class CommandImpl;
class DeviceImpl;

class Command {
public:
    explicit Command(CommandImpl*);
    Command(const Command&);
    Command(Command&&) noexcept;
    Command& operator=(const Command&) noexcept;
    Command& operator=(Command&&) noexcept;
    ~Command();

    const CommandImpl& Impl() const noexcept;
    CommandImpl& Impl() noexcept;

private:
    CommandImpl* m_impl;
};



}  // namespace nickel::graphics
