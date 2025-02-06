#include "nickel/graphics/lowlevel/cmd.hpp"
#include "nickel/graphics/lowlevel/internal/cmd_impl.hpp"

namespace nickel::graphics {

Command::Command(CommandImpl& impl) : m_impl{&impl} {}

Command::~Command() {
    if (m_impl) {
        m_impl->PendingDelete();
    }
}

const CommandImpl& Command::Impl() const noexcept {
    return *m_impl;
}

CommandImpl& Command::Impl() noexcept {
    return *m_impl;
}

}  // namespace nickel::graphics
