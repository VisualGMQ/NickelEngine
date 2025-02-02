#include "nickel/input/device/mouse.hpp"

#include "nickel/common/macro.hpp"
#include "nickel/nickel.hpp"
#include "nickel/video/internal/window_impl.hpp"

namespace nickel::input {
class Mouse::ButtonImpl {
public:
    friend class Mouse;

    Button::Type GetType() const { return m_type; }

    bool IsPressed() const { return m_is_pressing && !m_is_pressed; }

    bool IsPressing() const { return m_is_pressing && m_is_pressed; }

    bool IsReleased() const { return !m_is_pressing && m_is_pressed; }

    bool IsReleasing() const { return !m_is_pressing && !m_is_pressed; }

    void HandleEvent(const SDL_MouseButtonEvent&);
    void Update();

private:
    Button::Type m_type;
    bool m_is_pressed = false;
    bool m_is_pressing = false;
};

void Mouse::ButtonImpl::HandleEvent(const SDL_MouseButtonEvent& event) {
    NICKEL_RETURN_IF_FALSE(event.button == static_cast<int>(m_type));

    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        m_is_pressing = true;
    } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        m_is_pressing = false;
    }
}

void Mouse::ButtonImpl::Update() {
    m_is_pressed = m_is_pressing;
}

class Mouse::Impl {
public:
    Impl();
    const ButtonImpl& GetButton(Button::Type) const;
    const Vec2& GetPosition() const;
    const Vec2& GetOffset() const;
    void RelativeMode(bool enable);
    bool IsRelativeMode() const;
    float GetWheelDelta() const;
    void HandleEvent(const SDL_Event&);
    void Show(bool);
    void Update();

private:
    ButtonImpl m_buttons[5];
    Vec2 m_cur_pos;
    Vec2 m_offset;
    float m_wheel_delta{};
};

Mouse::Impl::Impl() {
    for (int i = 0; i < 5; i++) {
        m_buttons[i].m_type = static_cast<Button::Type>(i + 1);
    }
}

const Mouse::ButtonImpl& Mouse::Impl::GetButton(Button::Type type) const {
    return m_buttons[static_cast<int>(type) - 1];
}

void Mouse::Impl::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
        event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        m_buttons[event.button.button - 1].HandleEvent(event.button);
    }

    if (event.type == SDL_EVENT_MOUSE_MOTION) {
        m_cur_pos.x = event.motion.x;
        m_cur_pos.y = event.motion.y;
        m_offset.x = event.motion.xrel;
        m_offset.y = event.motion.yrel;
    }

    if (event.type == SDL_EVENT_MOUSE_WHEEL) {
        m_wheel_delta =
            event.wheel.y *
            (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1);
    }
}

void Mouse::Impl::Show(bool enable) {
    if (enable) {
        SDL_ShowCursor();
    } else {
        SDL_HideCursor();
    }
}

const Vec2& Mouse::Impl::GetPosition() const {
    return m_cur_pos;
}

const Vec2& Mouse::Impl::GetOffset() const {
    return m_offset;
}

void Mouse::Impl::RelativeMode(bool enable) {
    SDL_SetWindowRelativeMouseMode(
        Context::GetInst().GetWindow().GetImpl().m_window, enable);
}

bool Mouse::Impl::IsRelativeMode() const {
    return SDL_GetWindowRelativeMouseMode(
        Context::GetInst().GetWindow().GetImpl().m_window);
}

float Mouse::Impl::GetWheelDelta() const {
    return m_wheel_delta;
}

void Mouse::Impl::Update() {
    for (auto& btn : m_buttons) {
        btn.Update();
    }

    m_offset = {0, 0};
    m_wheel_delta = 0;
}

Mouse::Button::Button(const ButtonImpl& impl) : m_impl{impl} {}

Mouse::Button::Type Mouse::Button::GetType() const {
    return m_impl.GetType();
}

bool Mouse::Button::IsPressed() const {
    return m_impl.IsPressed();
}

bool Mouse::Button::IsPressing() const {
    return m_impl.IsPressing();
}

bool Mouse::Button::IsReleased() const {
    return m_impl.IsReleased();
}

bool Mouse::Button::IsReleasing() const {
    return m_impl.IsReleasing();
}

Mouse::Mouse() : m_impl{std::make_unique<Impl>()} {}

Mouse::~Mouse() {}

Mouse::Button Mouse::GetButton(Button::Type type) const {
    return Button{m_impl->GetButton(type)};
}

const Vec2& Mouse::GetPosition() const {
    return m_impl->GetPosition();
}

const Vec2& Mouse::GetOffset() const {
    return m_impl->GetOffset();
}

void Mouse::RelativeMode(bool enable) {
    m_impl->RelativeMode(enable);
}

bool Mouse::IsRelativeMode() const {
    return m_impl->IsRelativeMode();
}

float Mouse::GetWheelDelta() const {
    return m_impl->GetWheelDelta();
}

void Mouse::Show(bool enable) {
    m_impl->Show(enable);
}

void Mouse::HandleEvent(const SDL_Event& event) {
    m_impl->HandleEvent(event);
}

void Mouse::Update() {
    m_impl->Update();
}
}  // namespace nickel::input
