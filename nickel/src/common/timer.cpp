#include "common/timer.hpp"
#include "common/profile.hpp"

namespace nickel {

uint64_t Time::fps_ = 60;
uint64_t Time::fpsDuration_ = static_cast<uint64_t>(1000 / 60.0);

Time::Time() : elapse_(1) {
    curTime_ = clock_t::now();
}

Timer Timer::Null;

void Time::Update(gecs::resource<gecs::mut<Time>> t) {
    PROFILE_BEGIN();

    t->elapse_ = clock_t::now() - t->curTime_;

    t->curTime_ = clock_t::now();
    if (t->Elapse() < fpsDuration_) {
        auto duration = fpsDuration_ - t->Elapse();
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));
    }
}

void Timer::Update(const Time& t) {
    if (!isTicking_) return;

    curTime_ += t.Elapse();
    if (curTime_ > dstTime_) {
        curTime_ = 0;
        if (loop_ > 0) {
            loop_--;
        }
        if (shouldSendEvent && id_) {
            // gWorld->cur_registry()->event_dispatcher<TimerEvent>().enqueue(
            //     TimerEvent{id_.value()});
        }
        if (loop_ == 0) {
            Stop();
        }
    }
}

bool Timer::Load(const toml::table& tbl) {
    if (auto node = tbl.get("id"); node && node->is_integer()) {
        id_ = node->as_integer()->get();
    } else {
        return false;
    }

    if (auto node = tbl.get("time"); node && node->is_integer()) {
        dstTime_ = node->as_integer()->get();
    } else {
        return false;
    }

    if (auto node = tbl.get("loop"); node && node->is_integer()) {
        loop_ = node->as_integer()->get();
    } else {
        return false;
    }

    dstTime_ = 0;
    return true;
}

bool Timer::Save(toml::table& tbl) const {
    NICKEL_TOML_EMPLACE_NODE(tbl, "id", id_.value());
    NICKEL_TOML_EMPLACE_NODE(tbl, "time", dstTime_);
    NICKEL_TOML_EMPLACE_NODE(tbl, "loop", loop_);
    return true;
}

}  // namespace nickel
