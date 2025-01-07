#pragma once
#include "nickel/common/singleton.hpp"
#include "nickel/graphics/adapter.hpp"
#include "nickel/video/window.hpp"
#include "nickel/common/dllexport.hpp"

namespace nickel {

class Application;

class NICKEL_API Context : public Singlton<Context, true> {
public:
    Context();
    ~Context();

    bool ShouldExit() const noexcept;

    void RegisterCustomApplication(std::unique_ptr<Application>&& app);

    void Exit() noexcept;
    video::Window& GetWindow();
    graphics::Adapter& GetGPUAdapter();
    Application* GetApplication();
    const Application* GetApplication() const noexcept;

private:
    bool m_should_exit = false;
    std::unique_ptr<video::Window> m_window;
    std::unique_ptr<graphics::Adapter> m_graphics_context;
    std::unique_ptr<Application> m_application;
};

class NICKEL_API Application {
public:
    virtual void OnInit() {}

    virtual void OnQuit() {}

    virtual void OnUpdate() {}

    virtual ~Application() = default;
};

}  // namespace nickel

