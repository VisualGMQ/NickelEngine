#include "common/timer.hpp"
#include "common/profile.hpp"

namespace nickel {

uint64_t Time::fps_ = 60;
uint64_t Time::fpsDuration_ = static_cast<uint64_t>(1000 / 60.0);

Time::Time() : elapse_(1) {
    curTime_ = clock_t::now();
}

Timer Timer::Null;

Timer::Timer(const std::filesystem::path& filename) : Asset(filename) {
    auto parse = toml::parse_file(filename.string());
    if (!parse) {
        LOGW(log_tag::Asset, "load timer from ", filename,
             " failed:", parse.error());
    } else {
        auto ptr = LoadAssetFromMetaTable<Timer>(parse.table());
        *this = std::move(*ptr);
    }
}

void Time::Update(gecs::resource<gecs::mut<Time>> t) {
    PROFILE_BEGIN();

    t->elapse_ = clock_t::now() - t->curTime_;

    t->curTime_ = clock_t::now();
    if (t->Elapse() < fpsDuration_) {
        auto duration = fpsDuration_ - t->Elapse();
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));
    }
}

void Timer::Save(const std::filesystem::path& path) {
    toml::table tbl;
    tbl.emplace("id", id_.value());
    tbl.emplace("time", dstTime_);
    tbl.emplace("loop", loop_);

    std::ofstream file(path);
    if (file) {
        file << tbl;
    }
}

template <>
std::unique_ptr<Timer> LoadAssetFromMetaTable(const toml::table& tbl) {
    TimerID id;
    TimeType time;
    int loop = 0;

    if (auto path = tbl.get("path"); path && path->is_string()) {
        if (auto node = tbl.get("id"); node && node->is_integer()) {
            id = node->as_integer()->get();
        }

        if (auto node = tbl.get("time"); node && node->is_integer()) {
            time = node->as_integer()->get();
        }

        if (auto node = tbl.get("loop"); node && node->is_integer()) {
            loop = node->as_integer()->get();
        }
    }

    return std::make_unique<Timer>(id, time, loop);
}

TimerHandle TimerManager::Create(const std::filesystem::path& path, TimerID id,
                                 TimeType time, int loop) {
    auto handle = TimerHandle::Create();
    auto timer = std::make_unique<Timer>(id, time, loop);
    timer->AssociateFile(path);
    storeNewItem(handle, std::move(timer));
    return handle;
}

TimerHandle TimerManager::Load(const std::filesystem::path& filename) {
    auto timer = std::make_unique<Timer>(filename);
    if (timer && *timer) {
        auto handle = TimerHandle::Create();
        storeNewItem(handle, std::move(timer));
        return handle;
    }

    return TimerHandle::Null();
}

}  // namespace nickel
