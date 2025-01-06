#include "nickel/nickel.hpp"

namespace nickel {

Context::Context() {
    LOGI("init video system");
    m_window = std::make_unique<video::Window>("sandbox", 1024, 720);

    LOGI("init graphics system");
    m_graphics_context =
        std::make_unique<graphics::Adapter>(m_window->GetImpl());
}

Context::~Context() {
    LOGI("shutdown graphics system");
    m_graphics_context.reset();

    LOGI("shutdown window system");
    m_window.reset();
}

bool Context::ShouldExit() const noexcept {
    return m_should_exit;
}

void Context::Exit() noexcept {
    m_should_exit = true;
}

video::Window& Context::GetWindow() {
    return *m_window;
}

graphics::Adapter& Context::GetGPUAdapter() {
    return *m_graphics_context;
}

}  // namespace nickel