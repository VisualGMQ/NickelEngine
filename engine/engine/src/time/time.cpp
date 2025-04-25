#include "nickel/time/time.hpp"

namespace nickel {

Time::Time() {
    m_last_frame_time = std::chrono::system_clock::now();
}

uint64_t Time::CurrentTime() const {
    auto current_time = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(current_time);
}

float Time::DeltaTime() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(m_delta_time)
               .count() /
           1000.0f;
}

uint32_t Time::FPS() const {
    float delta = DeltaTime();
    if (delta == 0) {
        return 10000;
    }
    return 1.0 / delta;
}

void Time::Update() {
    auto current_time = std::chrono::system_clock::now();

    m_delta_time = current_time - m_last_frame_time;
    m_last_frame_time = current_time;
}

}  // namespace nickel