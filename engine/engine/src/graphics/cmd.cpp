#include "nickel/graphics/cmd.hpp"
#include "nickel/graphics/internal/cmd_impl.hpp"

namespace nickel::graphics {

Command::Command(CommandImpl* impl) : m_impl{impl} {}

Command::Command(const Command& o) : m_impl{o.m_impl} {
    m_impl->IncRefcount();
}

Command::Command(Command&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

Command& Command::operator=(const Command& o) noexcept {
    if (&o != this) {
        m_impl->DecRefcount();
        m_impl = o.m_impl;
        m_impl->IncRefcount();
    }
    return *this;
}

Command& Command::operator=(Command&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

Command::~Command() {
    m_impl->DecRefcount();
}

const CommandImpl& Command::Impl() const noexcept {
    return *m_impl;
}

CommandImpl& Command::Impl() noexcept {
    return *m_impl;
}

void Command::Release() {
    if (m_impl) {
        m_impl->DecRefcount();
        m_impl = nullptr;
    }
}

Command::operator bool() const noexcept {
    return m_impl;
}

}  // namespace nickel::graphics
