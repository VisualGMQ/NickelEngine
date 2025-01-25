#include "nickel/input/device/device_manager.hpp"

namespace nickel::input {
class DeviceManager::Impl {
public:
    const Mouse& GetMouse() const { return m_mouse; }
    const Keyboard& GetKeyboard() const { return m_keyboard; }
    const FingerInput& GetFingerInput() const { return m_finger; }
    const GamepadManager& GetGamepadManager() const { return m_gamepad_mgr; }

    void HandleEvent(const SDL_Event& event);
    void Update();

private:
    Mouse m_mouse;
    Keyboard m_keyboard;
    FingerInput m_finger;
    GamepadManager m_gamepad_mgr;
};

void DeviceManager::Impl::HandleEvent(const SDL_Event& event) {
    m_mouse.HandleEvent(event);
    m_keyboard.HandleEvent(event);
    m_finger.HandleEvent(event);
    m_gamepad_mgr.HandleEvent(event);
}

void DeviceManager::Impl::Update() {
    m_mouse.Update();
    m_keyboard.Update();
    m_finger.Update();
    m_gamepad_mgr.Update();
}

DeviceManager::DeviceManager()
    : m_impl{std::make_unique<Impl>()} {
}

DeviceManager::~DeviceManager() {
}

const Mouse& DeviceManager::GetMouse() const {
    return m_impl->GetMouse();
}

const Keyboard& DeviceManager::GetKeyboard() const {
    return m_impl->GetKeyboard();
}

const FingerInput& DeviceManager::GetFingerInput() const {
    return m_impl->GetFingerInput();
}

const GamepadManager& DeviceManager::GetGamepadManager() const {
    return m_impl->GetGamepadManager();
}

void DeviceManager::HandleEvent(const SDL_Event& event) {
    m_impl->HandleEvent(event);
}

void DeviceManager::Update() {
    m_impl->Update();
}
}
