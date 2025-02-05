#pragma once
#include "nickel/input/device/finger.hpp"
#include "nickel/input/device/gamepad.hpp"
#include "nickel/input/device/keyboard.hpp"
#include "nickel/input/device/mouse.hpp"

union SDL_Event;

namespace nickel::input {

class DeviceManager {
public:
    DeviceManager();
    ~DeviceManager();
    const Mouse& GetMouse() const;
    const Keyboard& GetKeyboard() const;
    const FingerInput& GetFingerInput() const;
    const GamepadManager& GetGamepadManager() const;

    void HandleEvent(const SDL_Event& event);
    void Update();

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}