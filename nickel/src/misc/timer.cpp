#include "misc/timer.hpp"

namespace nickel {

uint64_t Time::fps_ = 60;
uint64_t Time::fpsDuration_ = static_cast<uint64_t>(1000 / 60.0);

Time::Time() : elapse_(1) {
    curTime_ = clock_t::now();
}

void Time::Update(gecs::resource<gecs::mut<Time>> t) {
    t->elapse_ = clock_t::now() - t->curTime_;

    t->curTime_ = clock_t::now();
    if (t->Elapse() < fpsDuration_) {
        auto duration = fpsDuration_ - t->Elapse();
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));
    }
}

TimerHandle TimerManager::Create(TimeType time, TickFunc func) {
    auto handle = TimerHandle::Create();
    storeNewItem(handle, std::unique_ptr<Timer>(new Timer(handle, time, func)));
    return handle;
}

}  // namespace nickel
