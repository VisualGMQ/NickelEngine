#pragma once

#include "core/handle.hpp"
#include "core/manager.hpp"
#include "misc/timer.hpp"
#include "misc/transform.hpp"

namespace nickel {

template <typename T>
struct Interpolation final {
    static T Linear(float t, T a, T b) { return a + (b - a) * t; };
};

template <typename T, typename TimeType>
struct BasicKeyFrame {
    using interpolate_func = std::function<T(float, T, T)>;
    using value_type = T;
    using time_type = TimeType;

    T value;
    TimeType timePoint;
    interpolate_func interpolate;

    static BasicKeyFrame Create(
        T value, TimeType t,
        interpolate_func interp = Interpolation<T>::Linear) {
        return {value, t, interp};
    }
};

template <typename T>
using KeyFrame = BasicKeyFrame<T, TimeType>;

class AnimationTrack {
public:
    AnimationTrack() = default;

    AnimationTrack(mirrow::drefl::type_info type_info)
        : type_info_(type_info) {}

    virtual ~AnimationTrack() = default;

    virtual mirrow::drefl::any GetValueAt(TimeType) const = 0;
    virtual bool Empty() const = 0;
    virtual size_t Size() const = 0;
    virtual TimeType Duration() const = 0;

    auto TypeInfo() const { return type_info_; }

private:
    mirrow::drefl::type_info type_info_;
};

template <typename T>
class BasicAnimationTrack final : public AnimationTrack {
public:
    using keyframe_type = KeyFrame<T>;
    using container_type = std::vector<keyframe_type>;
    using time_type = typename keyframe_type::time_type;

    BasicAnimationTrack()
        : AnimationTrack(mirrow::drefl::reflected_type<T>()) {}

    BasicAnimationTrack(const container_type& keyPoints)
        : AnimationTrack(mirrow::drefl::reflected_type<
                         typename keyframe_type::value_type>()),
          keyPoints_(keyPoints) {}

    BasicAnimationTrack(container_type&& keyPoints)
        : AnimationTrack(mirrow::drefl::reflected_type<
                         typename keyframe_type::value_type>()),
          keyPoints_(std::move(keyPoints)) {}

    bool Empty() const override { return keyPoints_.empty(); }

    size_t Size() const override { return keyPoints_.size(); }

    TimeType Duration() const override {
        return keyPoints_.empty() ? 0 : keyPoints_.back().timePoint;
    }

    mirrow::drefl::any GetValueAt(TimeType t) const override {
        if (Empty()) {
            return T{};
        }

        if (Size() == 1) {
            return keyPoints_[0].value;
        }

        auto& last = keyPoints_.back();
        if (t >= last.timePoint) {
            return last.value;
        }

        for (int i = 0; i < keyPoints_.size() - 1; i++) {
            auto& begin = keyPoints_[i];
            auto& end = keyPoints_[i + 1];

            if (begin.timePoint <= t && t < end.timePoint) {
                return begin.interpolate(static_cast<float>(t) / (end.timePoint - begin.timePoint),
                                         begin.value, end.value);
            }
        }

        return {};
    }

private:
    container_type keyPoints_;
};

class Animation final {
public:
    using track_base_type = AnimationTrack;
    using track_pointer_type = std::unique_ptr<track_base_type>;
    using container_type = std::map<std::string, track_pointer_type>;

    static Animation Null;

    Animation() = default;
    Animation(container_type&& tracks) : tracks_(std::move(tracks)) {
        lastTime_ = 0;
        for (auto& [name, track] : tracks_) {
            auto duration = track->Duration();
            lastTime_ = std::max<TimeType>(lastTime_, duration);
        }
    }

    auto& Tracks() const { return tracks_; }

    TimeType Duration() const { return lastTime_; }

private:
    container_type tracks_;
    TimeType lastTime_;
};

using AnimationHandle = Handle<Animation>;

class AnimationManager final : public Manager<Animation> {
public:
    AnimationHandle CreateFromTracks(
        typename Animation::container_type&& tracks);
    std::shared_ptr<Animation> CreateSolitaryFromTracks(
        typename Animation::container_type&& tracks);
};

class AnimationPlayer final {
public:
    enum class Direction {
        Forward = 1,
        Backward = -1,
    };

    using animation_type = Animation;

    AnimationPlayer(AnimationHandle anim, AnimationManager& mgr) : handle_(anim), mgr_(mgr) {}

    AnimationHandle Animation() const { return handle_; }

    void SetDir(Direction dir) { dir_ = dir; }

    Direction GetDir() const { return dir_; }

    void Step(TimeType step) {
        if (isPlaying_) {
            curTime_ += static_cast<int>(dir_) * step;
        }
    }

    void AsyncTo(mirrow::drefl::reference_any instance) {
        if (!mgr_.Has(handle_)) {
            return ;
        }

        auto type_info = instance.type();
        Assert(type_info.is_class(),
               "currently we only support do animation on class");
        auto class_info = type_info.as_class();

        auto& anim = mgr_.Get(handle_);

        for (auto& [name, track] : anim.Tracks()) {
            for (auto&& var : class_info.vars()) {
                // IMPROVE: maybe we can use unordered_map to store vars to
                // improve find effeciency? Or maybe we should cache the result?
                auto field = mirrow::drefl::invoke_by_any_return_ref(var, &instance);
                if (var.name() == name && field.type() == track->TypeInfo()) {
                    field.deep_set(track->GetValueAt(curTime_));
                }
            }
        }
    }

    void Play() { isPlaying_ = true; }

    bool IsPlaying() const { return isPlaying_; }

    void Stop() { isPlaying_ = false; }

    void SetTick(TimeType t) {
        curTime_ = std::clamp<int>(curTime_, 0, static_cast<int>(Duration()));
    }

    void Reset() {
        if (dir_ == Direction::Forward) {
            curTime_ = 0;
        } else {
            curTime_ = static_cast<int>(Duration());
        }
    }

    bool IsValid() const {
        return mgr_.Has(handle_);
    }

    TimeType Duration() const {
        if (mgr_.Has(handle_)) {
            auto& anim = mgr_.Get(handle_);
            return anim.Duration();
        }
        return 0;
    }

private:
    AnimationManager& mgr_;
    Direction dir_ = Direction::Forward;
    int curTime_ = 0;
    AnimationHandle handle_;
    bool isPlaying_ = false;
};


}  // namespace nickel