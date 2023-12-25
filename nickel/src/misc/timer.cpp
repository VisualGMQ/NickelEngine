#include "misc/timer.hpp"
namespace nickel {

uint64_t Time::fps_ = 60;
uint64_t Time::fpsDuration_ = static_cast<uint64_t>(1000 / 60.0);

Time::Time() : elapse_(1) {
    curTime_ = clock_t::now();
}

Timer Timer::Null;

Timer::Timer(const std::filesystem::path& root,
             const std::filesystem::path& filename)
    : Asset(filename) {
    auto path = root / filename;

    do {
        auto result = toml::parse_file(path.string());
        if (!result) {
            LOGW(log_tag::Asset, "load timer from ", path,
                 " failed:", result.error());
            break;
        }

        auto& tbl = result.table();
        if (auto node = tbl.get("id"); node && node->is_integer()) {
            id_ = node->as_integer()->get();
        } else {
            break;
        }

        if (auto node = tbl.get("time"); node && node->is_integer()) {
            dstTime_ = node->as_integer()->get();
        } else {
            break;
        }

        if (auto node = tbl.get("loop"); node && node->is_integer()) {
            loop_ = node->as_integer()->get();
        } else {
            break;
        }
    } while (0);
}

void Time::Update(gecs::resource<gecs::mut<Time>> t) {
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
std::unique_ptr<Timer> LoadAssetFromToml(
    const toml::table& tbl, const std::filesystem::path& root) {
    if (auto path = tbl.get("path"); path && path->is_string()) {
        return std::make_unique<Timer>(root, path->as_string()->get());
    }
    return nullptr;
}

TimerHandle TimerManager::Create(const std::filesystem::path& path, TimerID id,
                                 TimeType time, int loop) {
    auto handle = TimerHandle::Create();
    auto timer = std::make_unique<Timer>(id, time, loop);
    timer->AssociateFile(path);
    storeNewItem(handle, std::move(timer));
    return handle;
}

TimerHandle TimerManager::Load(const std::filesystem::path& path) {
    auto timer = std::make_unique<Timer>(GetRootPath(), path);
    if (timer && *timer) {
        auto handle = TimerHandle::Create();
        storeNewItem(handle, std::move(timer));
        return handle;
    }

    return TimerHandle::Null();
}

}  // namespace nickel
