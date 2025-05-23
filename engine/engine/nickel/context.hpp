#pragma once
#include "nickel/common/dllexport.hpp"
#include "nickel/common/singleton.hpp"
#include "nickel/fs/dialog.hpp"
#include "nickel/fs/storage.hpp"
#include "nickel/graphics/camera.hpp"
#include "nickel/graphics/context.hpp"
#include "nickel/graphics/lowlevel/adapter.hpp"
#include "nickel/graphics/texture_manager.hpp"
#include "nickel/input/device/device_manager.hpp"
#include "nickel/video/window.hpp"
#include "nickel/time/time.hpp"

#include "imgui.h"
#include "implot.h"
#include "misc/Level.hpp"
#include "misc/cpp/imgui_stdlib.h"
#include "nickel/graphics/debug_draw.hpp"
#include "nickel/graphics/gltf.hpp"
#include "nickel/physics/context.hpp"
#include "nickel/refl/custom/flags_refl.hpp"

union SDL_Event;

namespace nickel {
class Application;

class NICKEL_API Context : public Singlton<Context, true> {
public:
    Context() = default;
    ~Context();

    void Initialize();

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
    Level& GetCurrentLevel();
    const Level& GetCurrentLevel() const;
    const graphics::DebugDrawer& GetDebugDrawer() const;
    graphics::DebugDrawer& GetDebugDrawer();
    physics::Context& GetPhysicsContext();
    const physics::Context& GetPhysicsContext() const;
    const Time& GetTime() const;
    Camera& GetCamera();
    void ChangeCamera(std::unique_ptr<Camera>&&);

    void OnWindowResize();

    void EnableRender(bool);

    void Update();

    const Path& GetEngineRelativePath() const;

private:
    bool m_should_exit = false;
    SVector<uint32_t, 2> m_old_window_size;
    std::unique_ptr<video::Window> m_window;
    std::unique_ptr<graphics::Adapter> m_graphics_adapter;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<graphics::Context> m_graphics_ctx;
    std::unique_ptr<StorageManager> m_storage_mgr;
    std::unique_ptr<physics::Context> m_physics;
    std::unique_ptr<graphics::DebugDrawer> m_debug_drawer;
    Time m_time;
    input::DeviceManager m_device_mgr;
    std::unique_ptr<graphics::TextureManager> m_texture_mgr;
    std::unique_ptr<graphics::GLTFManager> m_gltf_mgr;
    std::unique_ptr<Level> m_level;

    std::unique_ptr<Application> m_application;

    Path m_engine_relative_path;

    void initCamera() {
        auto window_size = m_window->GetSize();
        float aspect = window_size.w / (float)window_size.h;

        m_camera = std::make_unique<FlyCamera>(Radians{Degrees{30.0f}}, aspect,
                                               0.01f, 10000.0f);
    }

    Path parseEngineProjectPath() const;
};

class NICKEL_API Application {
public:
    virtual void OnInit() {}

    virtual void OnQuit() {}

    virtual void OnUpdate(float delta_time) {}

    virtual ~Application() = default;
};

}  // namespace nickel