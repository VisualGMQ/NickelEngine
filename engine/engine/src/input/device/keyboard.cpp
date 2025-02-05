#include "nickel/input/device/keyboard.hpp"
#include "nickel/common/macro.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::input {
inline SDL_Scancode Key2Scancode(Key key) {
    return static_cast<SDL_Scancode>(key);
}

inline Key Scancode2Key(SDL_Scancode scancode) {
    return static_cast<Key>(scancode);
}

class Keyboard::ButtonImpl {
public:
    friend class Keyboard;

    ButtonImpl(const ButtonImpl&) = delete;
    ButtonImpl(ButtonImpl&&) = delete;
    ButtonImpl& operator=(const ButtonImpl&) = delete;
    ButtonImpl& operator=(ButtonImpl&&) = delete;

    SDL_Scancode GetKey() const { return m_key; }

    bool IsPressed() const { return m_is_pressing && !m_is_pressed; }

    bool IsPressing() const { return m_is_pressing && m_is_pressed; }

    bool IsReleased() const { return !m_is_pressing && m_is_pressed; }

    bool IsReleasing() const { return !m_is_pressing && !m_is_pressed; }

    void HandleEvent(const SDL_KeyboardEvent&);
    void Update();

private:
    SDL_Scancode m_key = SDL_SCANCODE_UNKNOWN;
    bool m_is_pressing = false;
    bool m_is_pressed = false;

    ButtonImpl() = default;
};

void Keyboard::ButtonImpl::HandleEvent(const SDL_KeyboardEvent& event) {
    NICKEL_RETURN_IF_FALSE(event.scancode == m_key);

    if (event.type == SDL_EVENT_KEY_DOWN) {
        m_is_pressing = true;
    } else if (event.type == SDL_EVENT_KEY_UP) {
        m_is_pressing = false;
    }
}

void Keyboard::ButtonImpl::Update() {
    m_is_pressed = m_is_pressing;
}

class Keyboard::Impl {
public:
    Impl();
    const ButtonImpl& GetKey(SDL_Scancode key) const;
    void HandleEvent(const SDL_Event& event);
    void Update();

private:
    ButtonImpl m_buttons[SDL_SCANCODE_COUNT];
};

Keyboard::Impl::Impl() {
    for (int i = 0; i < SDL_SCANCODE_COUNT; i++) {
        m_buttons[i].m_key = static_cast<SDL_Scancode>(i);
    }
}

const Keyboard::ButtonImpl& Keyboard::Impl::GetKey(SDL_Scancode key) const {
    return m_buttons[key];
}

void Keyboard::Impl::HandleEvent(const SDL_Event& event) {
    NICKEL_RETURN_IF_FALSE(
        event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP);

    m_buttons[event.key.scancode].HandleEvent(event.key);
}

void Keyboard::Impl::Update() {
    for (auto& btn : m_buttons) {
        btn.Update();
    }
}

Keyboard::Button::Button(const ButtonImpl& impl)
    : m_impl{impl} {
}

Key Keyboard::Button::GetKey() const {
    return Scancode2Key(m_impl.GetKey());
}

bool Keyboard::Button::IsPressed() const {
    return m_impl.IsPressed();
}

bool Keyboard::Button::IsPressing() const {
    return m_impl.IsPressing();
}

bool Keyboard::Button::IsReleased() const {
    return m_impl.IsReleased();
}

bool Keyboard::Button::IsReleasing() const {
    return m_impl.IsReleasing();
}

Keyboard::Keyboard()
    : m_impl{std::make_unique<Keyboard::Impl>()} {
}

Keyboard::~Keyboard() {
}

Keyboard::Button Keyboard::GetKey(Key key) const {
    return Button{m_impl->GetKey(Key2Scancode(key))};
}

void Keyboard::HandleEvent(const SDL_Event& event) {
    return m_impl->HandleEvent(event);
}

void Keyboard::Update() {
    return m_impl->Update();
}
} // namespace tl
