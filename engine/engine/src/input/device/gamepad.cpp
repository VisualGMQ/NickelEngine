#include "nickel/input/device/gamepad.hpp"

#include "nickel/common/assert.hpp"
#include "nickel/common/macro.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::input {
inline SDL_GamepadButton ButtonType2SDL(Gamepad::Button::Type button) {
    return static_cast<SDL_GamepadButton>(button);
}

inline SDL_GamepadAxis AxisType2SDL(Gamepad::Axis::Type button) {
    return static_cast<SDL_GamepadAxis>(button);
}

inline Gamepad::Button::Type SDLGamepadButton2Type(SDL_GamepadButton button) {
    return static_cast<Gamepad::Button::Type>(button);
}

inline Gamepad::Axis::Type SDLGamepadAxis2Type(SDL_GamepadAxis button) {
    return static_cast<Gamepad::Axis::Type>(button);
}

class Gamepad::ButtonImpl {
public:
    friend class Gamepad;

    SDL_GamepadButton GetType() const { return m_button; }
    operator bool() const { return m_button != SDL_GAMEPAD_BUTTON_INVALID; }
    bool IsPressed() const { return m_is_pressing && !m_is_pressed; }
    bool IsPressing() const { return m_is_pressing && m_is_pressed; }
    bool IsReleased() const { return !m_is_pressing && m_is_pressed; }
    bool IsReleasing() const { return !m_is_pressing && !m_is_pressed; }

    void HandleEvent(const SDL_GamepadButtonEvent&);
    void Update();

private:
    SDL_GamepadButton m_button = SDL_GAMEPAD_BUTTON_INVALID;

    bool m_is_pressing = false;
    bool m_is_pressed = false;
};

void Gamepad::ButtonImpl::HandleEvent(const SDL_GamepadButtonEvent& event) {
    NICKEL_RETURN_IF_FALSE(event.button != SDL_GAMEPAD_BUTTON_INVALID);

    if (event.down) {
        m_is_pressing = true;
    } else {
        m_is_pressing = false;
    }
}

void Gamepad::ButtonImpl::Update() {
    m_is_pressed = m_is_pressing;
}

inline Gamepad::Button::Button(const ButtonImpl& impl)
    : m_impl{impl} {
}

Gamepad::Button::Type Gamepad::Button::GetType() const {
    return SDLGamepadButton2Type(m_impl.GetType());
}

Gamepad::Button::operator bool() const {
    return m_impl.m_button != SDL_GAMEPAD_BUTTON_INVALID;
}

bool Gamepad::Button::IsPressed() const {
    return m_impl.IsPressed();
}

bool Gamepad::Button::IsPressing() const {
    return m_impl.IsPressing();
}

bool Gamepad::Button::IsReleased() const {
    return m_impl.IsReleased();
}

bool Gamepad::Button::IsReleasing() const {
    return m_impl.IsReleasing();
}

class Gamepad::AxisImpl {
public:
    friend class Gamepad;

    void HandleEvent(const SDL_GamepadAxisEvent&);
    SDL_GamepadAxis GetType() const { return m_type; }
    double GetValue() const { return m_value; }

private:
    SDL_GamepadAxis m_type = SDL_GAMEPAD_AXIS_INVALID;
    double m_value = 0; // in [-1, 1]
};

void Gamepad::AxisImpl::HandleEvent(const SDL_GamepadAxisEvent& event) {
    // TODO: check which axis

    if (event.value > 0) {
        m_value = event.value / 32767.0f;
    } else {
        m_value = event.value / 32768.0f;
    }
}

Gamepad::Axis::Axis(const AxisImpl& impl)
    : m_impl{impl} {
}

Gamepad::Axis::Type Gamepad::Axis::GetType() const {
    return static_cast<Type>(m_impl.GetType());
}

double Gamepad::Axis::GetValue() const {
    return m_impl.GetValue();
}

class Gamepad::Impl {
public:
    explicit Impl(SDL_JoystickID id);
    ~Impl();

    SDL_JoystickID GetID() const;
    const AxisImpl& GetAxis(SDL_GamepadAxis) const;
    const ButtonImpl& GetButton(SDL_GamepadButton) const;
    void HandleEvent(const SDL_Event&);
    void Update();

private:
    SDL_Gamepad* m_gamepad{};
    SDL_JoystickID m_instance_id;

    ButtonImpl m_buttons[SDL_GAMEPAD_BUTTON_COUNT];
    AxisImpl m_axises[SDL_GAMEPAD_AXIS_COUNT];
};

Gamepad::Impl::Impl(SDL_JoystickID id)
    : m_instance_id{id} {
    NICKEL_RETURN_IF_FALSE_LOGE(m_instance_id != 0, "invalid gamepad id");

    m_gamepad = SDL_OpenGamepad(m_instance_id);
    NICKEL_RETURN_IF_FALSE_LOGE(m_gamepad, "gamepad can't be open");

    for (int i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; i++) {
        m_buttons[i].m_button = static_cast<SDL_GamepadButton>(i);
    }
    for (int i = 0; i < SDL_GAMEPAD_AXIS_COUNT; i++) {
        m_axises[i].m_type = static_cast<SDL_GamepadAxis>(i);
    }
}

void Gamepad::Impl::HandleEvent(const SDL_Event& event) {
    if ((event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN ||
         event.type == SDL_EVENT_GAMEPAD_BUTTON_UP) &&
        event.gbutton.which == m_instance_id) {
        m_buttons[event.gbutton.button].HandleEvent(event.gbutton);
    }

    if (event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION &&
        event.gaxis.which == m_instance_id) {
        m_axises[event.gaxis.axis].HandleEvent(event.gaxis);
    }
}

void Gamepad::Impl::Update() {
    for (auto& button : m_buttons) {
        button.Update();
    }
}

Gamepad::Impl::~Impl() {
    SDL_CloseGamepad(m_gamepad);
}

SDL_JoystickID Gamepad::Impl::GetID() const {
    return SDL_GetGamepadID(m_gamepad);
}

const Gamepad::AxisImpl& Gamepad::Impl::GetAxis(SDL_GamepadAxis type) const {
    NICKEL_ASSERT(type != SDL_GAMEPAD_AXIS_INVALID);
    return m_axises[type];
}

const Gamepad::ButtonImpl& Gamepad::Impl::GetButton(
    SDL_GamepadButton type) const {
    NICKEL_ASSERT(type != SDL_GAMEPAD_BUTTON_INVALID);
    return m_buttons[type];
}

Gamepad::Gamepad(ID id)
    : m_impl{std::make_unique<Impl>(static_cast<SDL_JoystickID>(id))} {
}

Gamepad::Axis Gamepad::GetAxis(Axis::Type type) const {
    return Axis{m_impl->GetAxis(AxisType2SDL(type))};
}

Gamepad::Button Gamepad::GetButton(Button::Type type) const {
    return Button{m_impl->GetButton(ButtonType2SDL(type))};
}

Gamepad::ID Gamepad::GetID() const {
    return m_impl->GetID();
}

Gamepad::Impl& Gamepad::GetImpl() {
    return *m_impl;
}

const Gamepad::Impl& Gamepad::GetImpl() const {
    return *m_impl;
}

class GamepadManager::Impl {
public:
    std::vector<Gamepad>& GetGamepads() { return m_gamepads; }
    const std::vector<Gamepad>& GetGamepads() const { return m_gamepads; }
    void HandleEvent(const SDL_Event&);
    void Update();

private:
    std::vector<Gamepad> m_gamepads;
};

void GamepadManager::Impl::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_GAMEPAD_ADDED) {
        SDL_JoystickID id = event.gdevice.which;
        m_gamepads.emplace_back(id);
    } else if (event.type == SDL_EVENT_GAMEPAD_REMOVED) {
        std::erase_if(m_gamepads,
                      [&](const Gamepad& gamepad) {
                          return gamepad.GetImpl().GetID() ==
                                 event.gdevice.which;
                      });
    } else if (event.type == SDL_EVENT_GAMEPAD_REMAPPED) {
        // TODO: not finish
    } else {
        for (auto& gamepad : m_gamepads) {
            gamepad.GetImpl().HandleEvent(event);
        }
    }
}

void GamepadManager::Impl::Update() {
    for (auto& gamepad : m_gamepads) {
        gamepad.GetImpl().Update();
    }
}

void GamepadManager::HandleEvent(const SDL_Event& event) {
    return m_impl->HandleEvent(event);
}

void GamepadManager::Update() {
    return m_impl->Update();
}

const std::vector<Gamepad>& GamepadManager::GetGamepads() const {
    return m_impl->GetGamepads();
}

std::vector<Gamepad>& GamepadManager::GetGamepads() {
    return m_impl->GetGamepads();
}

const Gamepad* GamepadManager::GetByID(Gamepad::ID id) const {
    auto& gamepads = m_impl->GetGamepads();
    auto it = std::ranges::find_if(gamepads, [=](const Gamepad& gamepad) {
        return gamepad.GetID() == id;
    });
    if (it != gamepads.end()) {
        return &(*it);
    }
    return nullptr;
}

Gamepad* GamepadManager::GetByID(Gamepad::ID id) {
    return const_cast<Gamepad*>(std::as_const(*this).GetByID(id));
}

GamepadManager::GamepadManager()
    : m_impl{std::make_unique<Impl>()} {
}

GamepadManager::~GamepadManager() {
}
} // namespace tl::input
