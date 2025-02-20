#include "nickel/nickel.hpp"

#include "nickel/graphics/lowlevel/internal/adapter_impl.hpp"
#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/enum_convert.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel {

Context::Context() {
    LOGI("init video system");
    m_window = std::make_unique<video::Window>("sandbox", 1024, 720);

    LOGI("init graphics system");
    m_graphics_adapter =
        std::make_unique<graphics::Adapter>(m_window->GetImpl());

    initCamera();

    LOGI("init storage manager");
    // TODO: change org & app from config
    m_storage_mgr =
        std::make_unique<StorageManager>("visualgmq", "nickelengine");

    LOGI("init graphics context");
    m_graphics_ctx = std::make_unique<graphics::Context>(
        *m_graphics_adapter, *m_window, *m_storage_mgr);

    LOGI("init asset managers");
    m_gltf_mgr = std::make_unique<graphics::GLTFManager>();
    m_texture_mgr = std::make_unique<graphics::TextureManager>();

    LOGI("init physics context");
    m_physics = std::make_unique<physics::Context>();
}

Context::~Context() {
    if (m_application) {
        m_application->OnQuit();
    }
    m_application.reset();

    LOGI("release physics context");
    m_physics.reset();

    LOGI("release assets");
    m_gltf_mgr.reset();
    m_texture_mgr.reset();

    LOGI("clear graphics context");
    m_graphics_ctx.reset();

    LOGI("shutdown graphics system");
    m_graphics_adapter.reset();

    LOGI("shutdown window system");
    m_window.reset();
}

void Context::HandleEvent(const SDL_Event& event) {
    ImGui_ImplSDL3_ProcessEvent(&event);
    if (event.type == SDL_EVENT_QUIT) {
        Exit();
    }

    m_device_mgr.HandleEvent(event);
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
    return *m_graphics_adapter;
}

const input::DeviceManager& Context::GetDeviceManager() const {
    return m_device_mgr;
}

input::DeviceManager& Context::GetDeviceManager() {
    return m_device_mgr;
}

Application* Context::GetApplication() {
    return m_application.get();
}

const Application* Context::GetApplication() const noexcept {
    return m_application.get();
}

StorageManager& Context::GetStorageManager() {
    return *m_storage_mgr;
}

const StorageManager& Context::GetStorageManager() const {
    return *m_storage_mgr;
}

graphics::Context& Context::GetGraphicsContext() {
    return *m_graphics_ctx;
}

graphics::TextureManager& Context::GetTextureManager() {
    return *m_texture_mgr;
}

const graphics::TextureManager& Context::GetTextureManager() const {
    return *m_texture_mgr;
}

const graphics::GLTFManager& Context::GetGLTFManager() const {
    return *m_gltf_mgr;
}

graphics::GLTFManager& Context::GetGLTFManager() {
    return *m_gltf_mgr;
}

Camera& Context::GetCamera() {
    return *m_camera;
}

void Context::EnableRender(bool enable) {
    m_graphics_ctx->EnableRender(enable);
}

void Context::Update() {
    m_graphics_ctx->BeginFrame();

    auto app = GetApplication();
    if (app) {
        app->OnUpdate();
    }

    GetDeviceManager().Update();

    // TODO: use sub-step simulation
    m_physics->Update(0.3);
    
    m_gltf_mgr->Update();
    m_graphics_ctx->EndFrame();
    
    m_physics->GC();
    m_gltf_mgr->GC();
    m_texture_mgr->GC();
}

}  // namespace nickel