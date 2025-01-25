#include "nickel/input/device/mouse.hpp"
#include "nickel/common/macro.hpp"

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
    void HandleEvent(const SDL_Event&);
    void Update();

private:
    ButtonImpl m_buttons[5];
    Vec2 m_cur_pos;
    Vec2 m_offset;
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
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type ==
        SDL_EVENT_MOUSE_BUTTON_UP) {
        m_buttons[event.button.button - 1].HandleEvent(event.button);
    }

    if (event.type == SDL_EVENT_MOUSE_MOTION) {
        m_cur_pos.x = event.motion.x;
        m_cur_pos.y = event.motion.y;
        m_offset.x = event.motion.xrel;
        m_offset.y = event.motion.yrel;
    }
}

const Vec2& Mouse::Impl::GetPosition() const {
    return m_cur_pos;
}

const Vec2& Mouse::Impl::GetOffset() const {
    return m_offset;
}

void Mouse::Impl::Update() {
    for (auto& btn : m_buttons) {
        btn.Update();
    }
}

Mouse::Button::Button(const ButtonImpl& impl): m_impl{impl} {
}

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

Mouse::Mouse()
    : m_impl{std::make_unique<Impl>()} {
}

Mouse::~Mouse() {}

const Mouse::Button& Mouse::GetButton(Button::Type type) const {
    return Button{m_impl->GetButton(type)};
}

const Vec2& Mouse::GetPosition() const {
    return m_impl->GetPosition();
}

const Vec2& Mouse::GetOffset() const {
    return m_impl->GetOffset();
}

void Mouse::HandleEvent(const SDL_Event& event) {
    m_impl->HandleEvent(event);
}

void Mouse::Update() {
    m_impl->Update();
}
} // namespace tl
