#include "nickel/input/device/finger.hpp"
#include "nickel/common/macro.hpp"

namespace nickel::input {
class FingerInput::TouchPointImpl {
public:
    void HandleEvent(const SDL_TouchFingerEvent&);
    void Update();

    bool IsPressed() const { return m_is_pressing && !m_is_pressed; }
    bool IsPressing() const { return m_is_pressing && m_is_pressed; }
    bool IsReleased() const { return !m_is_pressing && m_is_pressed; }
    bool IsReleasing() const { return !m_is_pressing && !m_is_pressed; }
    const Vec2& GetPosition() const { return m_cur_pos; }
    const Vec2& GetOffset() const { return m_offset; }

private:
    Vec2 m_cur_pos; // in [0, 1]
    Vec2 m_offset;  // in [0, 1]

    float m_pressure = 0;
    bool m_is_pressing = false;
    bool m_is_pressed = false;
};


void FingerInput::TouchPointImpl::HandleEvent(
    const SDL_TouchFingerEvent& event) {
    m_cur_pos.x = event.x;
    m_cur_pos.y = event.y;
    m_offset.x = event.dx;
    m_offset.y = event.dy;
    m_pressure = event.pressure;

    if (event.type == SDL_EVENT_FINGER_DOWN) {
        m_is_pressing = true;
    }
    if (event.type == SDL_EVENT_FINGER_UP) {
        m_is_pressing = false;
    }
}

void FingerInput::TouchPointImpl::Update() {
    m_is_pressed = m_is_pressing;
}

FingerInput::TouchPoint::TouchPoint(const TouchPointImpl& impl)
    : m_impl{impl} {
}

bool FingerInput::TouchPoint::IsPressed() const {
    return m_impl.IsPressed();
}

bool FingerInput::TouchPoint::IsPressing() const {
    return m_impl.IsPressing();
}

bool FingerInput::TouchPoint::IsReleased() const {
    return m_impl.IsReleased();
}

bool FingerInput::TouchPoint::IsReleasing() const {
    return m_impl.IsReleasing();
}

const Vec2& FingerInput::TouchPoint::GetPosition() const {
    return m_impl.GetPosition();
}

const Vec2& FingerInput::TouchPoint::GetOffset() const {
    return m_impl.GetOffset();
}

class FingerInput::Impl {
public:
    static constexpr uint32_t MaxFingerCount = 16;

    const TouchPointImpl& GetFinger(uint32_t idx) const {
        return m_fingers[idx];
    }

    uint32_t FingerMaxCount() const { return MaxFingerCount; }

    void HandleEvent(const SDL_Event&);
    void Update();

private:
    TouchPointImpl m_fingers[MaxFingerCount];
};

void FingerInput::Impl::HandleEvent(const SDL_Event& event) {
    NICKEL_RETURN_IF_FALSE(
        event.type == SDL_EVENT_FINGER_UP || event.type == SDL_EVENT_FINGER_DOWN
        ||
        event.type == SDL_EVENT_FINGER_MOTION);
    SDL_FingerID id = event.tfinger.fingerID;
    NICKEL_RETURN_IF_FALSE(id >= 0 && id < MaxFingerCount);

    m_fingers[id].HandleEvent(event.tfinger);
}

void FingerInput::Impl::Update() {
    for (auto& finger : m_fingers) {
        finger.Update();
    }
}

FingerInput::TouchPoint FingerInput::GetFinger(uint32_t idx) const {
    return TouchPoint{m_impl->GetFinger(idx)};
}

uint32_t FingerInput::FingerMaxCount() const {
    return m_impl->FingerMaxCount();
}

void FingerInput::HandleEvent(const SDL_Event& event) {
    m_impl->HandleEvent(event);
}

void FingerInput::Update() {
    m_impl->Update();
}

FingerInput::FingerInput()
    : m_impl{std::make_unique<Impl>()} {
}

FingerInput::~FingerInput() {
}
} // namespace tl
