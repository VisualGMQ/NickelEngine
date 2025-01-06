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
    if (m_application) {
        m_application->OnQuit();
    }
    
    LOGI("shutdown graphics system");
    m_graphics_context.reset();

    LOGI("shutdown window system");
    m_window.reset();
}

bool Context::ShouldExit() const noexcept {
    return m_should_exit;
}

void Context::RegisterCustomApplication(std::unique_ptr<Application>&& app) {
    m_application = std::move(app);
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

Application* Context::GetApplication() {
    return m_application.get();
}

const Application* Context::GetApplication() const noexcept {
    return m_application.get();
}

}  // namespace nickel