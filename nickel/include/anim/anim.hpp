#pragma once

#include "anim/keyframe.hpp"
#include "core/handle.hpp"
#include "core/manager.hpp"
#include "core/singlton.hpp"
#include "misc/timer.hpp"
#include "refl/keyframe.hpp"

namespace nickel {

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

    auto& KeyPoints() const { return keyPoints_; }

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
                return begin.interpolate(
                    static_cast<float>(t) / (end.timePoint - begin.timePoint),
                    begin.value, end.value);
            }
        }

        return {};
    }

private:
    container_type keyPoints_;
};

class AnimTrackSerialMethods : public Singleton<AnimTrackSerialMethods, false> {
public:
    using serialize_fn_type = toml::table (*)(const AnimationTrack&);
    using deserialize_fn_type =
        std::unique_ptr<AnimationTrack> (*)(const toml::table&);
    using type_info_type = mirrow::drefl::type_info;

    bool Contain(type_info_type type);
    serialize_fn_type GetSerializeMethod(type_info_type type);
    deserialize_fn_type GetDeserializeMethod(type_info_type);

    template <typename T>
    auto& RegistMethod() {
        auto type_info = mirrow::drefl::reflected_type<T>();
        methods_.emplace(type_info,
                         std::make_pair(serialize<T>, deserialize<T>));
        return *this;
    }

private:
    std::unordered_map<type_info_type,
                       std::pair<serialize_fn_type, deserialize_fn_type>>
        methods_;

    template <typename T>
    static toml::table serialize(const AnimationTrack& animTrack) {
        auto& track = static_cast<const BasicAnimationTrack<T>&>(animTrack);
        toml::table tbl;
        tbl.emplace("keyframe",
                    mirrow::serd::srefl::serialize(track.KeyPoints()));
        auto type_info = mirrow::drefl::reflected_type<T>();

        std::string type;

        tbl.emplace("type", type_info.name());
        tbl.emplace("type", type);

        return tbl;
    }

    template <typename T>
    static std::unique_ptr<AnimationTrack> deserialize(const toml::table& tbl) {
        auto type_info = mirrow::drefl::reflected_type<T>();
        auto typeNode = tbl["type"];

        Assert(typeNode.is_string(), "type is not string");
        auto typeStr = typeNode.as_string()->get();
        Assert(typeStr == type_info.name(), "deserialize table type not fit");

        auto keyframes = tbl["keyframe"];
        Assert(keyframes.is_array(),
               "deserialize KeyPoint must has toml::array node");

        typename BasicAnimationTrack<T>::container_type track;
        mirrow::serd::srefl::deserialize(*keyframes.as_array(), track);
        return std::make_unique<BasicAnimationTrack<T>>(std::move(track));
    }
};

class Animation final {
public:
    using track_base_type = AnimationTrack;
    using track_pointer_type = std::unique_ptr<track_base_type>;
    using container_type = std::unordered_map<std::string, track_pointer_type>;

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

    AnimationPlayer(AnimationHandle anim, AnimationManager& mgr)
        : handle_(anim), mgr_(mgr) {}

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
            return;
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
                auto field =
                    mirrow::drefl::invoke_by_any_return_ref(var, &instance);
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

    bool IsValid() const { return mgr_.Has(handle_); }

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