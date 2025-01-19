#pragma once
#include "nickel/common/dllexport.hpp"
#include "nickel/common/singleton.hpp"
#include "nickel/fs/storage.hpp"
#include "nickel/graphics/camera.hpp"
#include "nickel/graphics/context.hpp"
#include "nickel/graphics/lowlevel/adapter.hpp"
#include "nickel/graphics/texture_manager.hpp"
#include "nickel/input/device/device_manager.hpp"
#include "nickel/video/window.hpp"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "nickel/graphics/gltf.hpp"
#include "nickel/physics/context.hpp"

union SDL_Event;

namespace nickel {
class Application;

class NICKEL_API Context : public Singlton<Context, true> {
public:
    Context();
    ~Context();

    void HandleEvent(const SDL_Event&);

    bool ShouldExit() const noexcept;

    void RegisterCustomApplication(std::unique_ptr<Application>&& app);

    void Exit() noexcept;
    video::Window& GetWindow();
    graphics::Adapter& GetGPUAdapter();
    const input::DeviceManager& GetDeviceManager() const;
    input::DeviceManager& GetDeviceManager();
    Application* GetApplication();
    const Application* GetApplication() const noexcept;
    StorageManager& GetStorageManager();
    const StorageManager& GetStorageManager() const;
    graphics::Context& GetGraphicsContext();
    graphics::TextureManager& GetTextureManager();
    const graphics::TextureManager& GetTextureManager() const;
    const graphics::GLTFManager& GetGLTFManager() const;
    graphics::GLTFManager& GetGLTFManager();
    Camera& GetCamera();

    void EnableRender(bool);

    void Update();

private:
    bool m_should_exit = false;
    std::unique_ptr<video::Window> m_window;
    std::unique_ptr<graphics::Adapter> m_graphics_adapter;
    std::unique_ptr<Application> m_application;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<graphics::Context> m_graphics_ctx;
    std::unique_ptr<StorageManager> m_storage_mgr;
    physics::Context m_physics;

    input::DeviceManager m_device_mgr;
    std::unique_ptr<graphics::TextureManager> m_texture_mgr;
    std::unique_ptr<graphics::GLTFManager> m_gltf_mgr;

    void initCamera() {
        auto window_size = m_window->GetSize();
        float aspect = window_size.w / (float)window_size.h;

        m_camera = std::make_unique<FlyCamera>(Radians{Degrees{45.0f}}, aspect,
                                               0.1f, 100.0f);
    }
};

class NICKEL_API Application {
public:
    virtual void OnInit() {}

    virtual void OnQuit() {}

    virtual void OnUpdate() {}

    virtual ~Application() = default;
};
}  // namespace nickel

