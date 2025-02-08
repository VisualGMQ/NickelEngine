#include "nickel/graphics/lowlevel/cmd.hpp"
#include "nickel/graphics/lowlevel/internal/cmd_impl.hpp"

namespace nickel::graphics {

Command::Command(CommandEncoderImpl& impl) : m_impl{&impl} {}

Command::~Command() {
    if (m_impl) {
        m_impl->PendingDelete();
    }
}

const CommandEncoderImpl& Command::Impl() const noexcept {
    return *m_impl;
}

CommandEncoderImpl& Command::Impl() noexcept {
    return *m_impl;
}

}  // namespace nickel::graphics
