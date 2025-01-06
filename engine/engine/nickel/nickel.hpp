#pragma once
#include "nickel/common/singleton.hpp"
#include "nickel/graphics/adapter.hpp"
#include "nickel/video/window.hpp"

namespace nickel {

class Application;

class Context : public Singlton<Context, true> {
public:
    Context();
    ~Context();

    bool ShouldExit() const noexcept;

    template <typename T>
    void RegisterApplication() {
        m_application = std::make_unique<T>();
    }
    
    void Exit() noexcept;
    video::Window& GetWindow();
    graphics::Adapter& GetGPUAdapter();

private:
    bool m_should_exit = false;
    std::unique_ptr<video::Window> m_window;
    std::unique_ptr<graphics::Adapter> m_graphics_context;
    std::unique_ptr<Application> m_application;
};

class Application {
public:
    virtual void OnInit() {}

    virtual void OnQuit() {}

    virtual void OnUpdate() {}

    virtual ~Application() = default;
};

}  // namespace nickel

void RegisterCustomApplication(nickel::Context&);