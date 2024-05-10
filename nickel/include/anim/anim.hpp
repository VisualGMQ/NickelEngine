#pragma once

#include "anim/keyframe.hpp"
#include "common/handle.hpp"
#include "common/manager.hpp"
#include "common/singlton.hpp"
#include "common/filetype.hpp"
#include "common/timer.hpp"

namespace nickel {

class BasicAnimationTrack {
public:
    using PropertyLinkContainer = std::vector<std::string>;

    BasicAnimationTrack() = default;

    explicit BasicAnimationTrack(const mirrow::drefl::type* applyTarget,
                                 const PropertyLinkContainer& propLink)
        : propLink_(propLink), applyTypeInfo_(applyTarget) {}

    explicit BasicAnimationTrack(const mirrow::drefl::type* applyTarget,
                                 PropertyLinkContainer&& propLink)
        : propLink_(std::move(propLink)), applyTypeInfo_(applyTarget) {}

    virtual ~BasicAnimationTrack() = default;

    virtual mirrow::drefl::any GetValueAt(TimeType) const = 0;

    auto& PropertyLink() const { return propLink_; }

    auto GetApplyTarget() const { return applyTypeInfo_; }

    virtual toml::table Save2Toml() const = 0;

    auto& KeyFrames() const { return keyframes_; }
    void RemoveKeyFrame(size_t index) {
        keyframes_.erase(keyframes_.begin() + index);
    }
    void AddKeyFrame(size_t index, std::unique_ptr<IKeyFrame>&& frame) {
        keyframes_.insert(keyframes_.begin() + index, std::move(frame));
    }

    bool Empty() const { return keyframes_.empty(); }

    size_t Size() const { return keyframes_.size(); }

    virtual IKeyFrame& AppendKeyFrame() = 0;

    TimeType Duration() const {
        return keyframes_.empty() ? 0 : keyframes_.back()->timePoint;
    }

protected:
    std::vector<std::unique_ptr<IKeyFrame>> keyframes_;

private:
    const mirrow::drefl::type*
        applyTypeInfo_;  // which component should this track apply to
    PropertyLinkContainer propLink_;  // properties link in component
};

template <typename T>
class AnimationTrack final : public BasicAnimationTrack {
public:
    using keyframe_type = KeyFrame<T>;

    AnimationTrack() : BasicAnimationTrack(nullptr, {}) {}

    AnimationTrack(
        const mirrow::drefl::type* applyTarget,
        typename BasicAnimationTrack::PropertyLinkContainer&& propList)
        : BasicAnimationTrack(applyTarget, std::move(propList)) {
    }

    AnimationTrack(
        std::vector<keyframe_type>&& keyFrames,
        const mirrow::drefl::type* applyTarget,
        typename BasicAnimationTrack::PropertyLinkContainer&& propList)
        : BasicAnimationTrack(applyTarget, std::move(propList)) {
        for (auto& frame : keyFrames) {
            keyframes_.emplace_back(std::make_unique<keyframe_type>(frame));
        }
    }

    IKeyFrame& AppendKeyFrame() override {
        if (keyframes_.empty()) {
            keyframes_.emplace_back(std::make_unique<keyframe_type>());
        } else {
            auto& last = static_cast<keyframe_type&>(*keyframes_.back());
            keyframes_.emplace_back(std::make_unique<keyframe_type>(
                last.value, last.timePoint + 10, last.interpolate_));
        }
        return *keyframes_.back();
    }

    mirrow::drefl::any GetValueAt(TimeType t) const override {
        if (Empty()) {
            return {};
        }

        auto& keyFrames = KeyFrames();

        if (Size() == 1) {
            return mirrow::drefl::any_make_constref(
                static_cast<keyframe_type&>(*keyFrames[0]).value);
        }

        auto& last = static_cast<keyframe_type&>(*keyFrames.back());
        if (t >= last.timePoint) {
            return mirrow::drefl::any_make_constref(last.value);
        }

        for (int i = 0; i < keyFrames.size() - 1; i++) {
            auto& begin = static_cast<keyframe_type&>(*keyFrames[i]);
            auto& end = static_cast<keyframe_type&>(*keyFrames[i + 1]);

            if (begin.timePoint <= t && t < end.timePoint) {
                return mirrow::drefl::any_make_copy(
                    begin.interpolate_(static_cast<float>(t - begin.timePoint) /
                                           (end.timePoint - begin.timePoint),
                                       begin.value, end.value));
            }
        }

        return {};
    }

    toml::table Save2Toml() const override {
        toml::table tbl;
        tbl.emplace("apply_target", GetApplyTarget()->name());

        toml::array arr;
        for (auto& prop : PropertyLink()) {
            arr.push_back(prop);
        }
        tbl.emplace("property_link", std::move(arr));

        toml::array pointArr;
        for (auto& point : KeyFrames()) {
            pointArr.push_back(point->Save2Toml());
        }

        tbl.emplace("key_points", pointArr);
        tbl.emplace("value_type", mirrow::drefl::typeinfo<T>()->name());

        return tbl;
    }

    static std::unique_ptr<AnimationTrack> LoadFromToml(
        const toml::table& tbl) {
        if (auto node = tbl.get("value_type"); node && node->is_string()) {
            if (node->as_string()->get() !=
                mirrow::drefl::typeinfo<T>()->name()) {
                LOGW(log_tag::Asset,
                     "animation track value type not satisfied");
                return nullptr;
            }
        } else {
            return nullptr;
        }

        const mirrow::drefl::type* applyTargetType = nullptr;
        if (auto node = tbl.get("apply_target"); node && node->is_string()) {
            applyTargetType = mirrow::drefl::typeinfo(node->as_string()->get());
        }
        if (!applyTargetType) {
            LOGW(log_tag::Asset, "load animation from toml failed! the type "
                                 "info of apply_target don't exists");
            return nullptr;
        }

        PropertyLinkContainer propLink;
        if (auto node = tbl.get("property_link"); node && node->is_array()) {
            auto& arr = *node->as_array();
            for (auto& elem : arr) {
                if (!elem.is_string()) {
                    LOGW(log_tag::Asset,
                         "load animation from toml failed! property_link array "
                         "don't contain string");
                    return nullptr;
                }

                propLink.push_back(elem.as_string()->get());
            }
        } else {
            LOGW(log_tag::Asset, "load animation from toml failed! "
                                 "property_link field don't exists");
        }

        std::vector<keyframe_type> keyPoints;
        if (auto node = tbl.get("key_points"); node && node->is_array()) {
            auto& arr = *node->as_array();
            for (auto& elem : arr) {
                if (!elem.is_table()) {
                    LOGW(log_tag::Asset, "load animation from toml failed! "
                                         "key_points field don't exists");
                    return nullptr;
                }
                keyPoints.push_back(
                    keyframe_type::LoadFromToml(*elem.as_table()));
            }
        }

        return std::make_unique<AnimationTrack>(
            std::move(keyPoints), applyTargetType, std::move(propLink));
    }
};

class AnimTrackLoadMethods : public Singlton<AnimTrackLoadMethods, false> {
public:
    using deserialize_fn_type =
        std::unique_ptr<BasicAnimationTrack> (*)(const toml::table&);
    using type_info_type = const mirrow::drefl::type*;
    using create_fn_type = std::unique_ptr<BasicAnimationTrack> (*)(
        const mirrow::drefl::type*,
        typename BasicAnimationTrack::PropertyLinkContainer&&);

    struct Method {
        create_fn_type create;
        deserialize_fn_type deserialize;
    };

    bool Contain(type_info_type type);
    Method Find(type_info_type type) const;

    template <typename T>
    auto& RegistMethod() {
        auto type_info = mirrow::drefl::typeinfo<T>();
        methods_.emplace(
            type_info,
            Method{[](const mirrow::drefl::type* applyTarget,
                      typename BasicAnimationTrack::PropertyLinkContainer&&
                          propList) -> std::unique_ptr<BasicAnimationTrack> {
                       return std::make_unique<AnimationTrack<T>>(
                           applyTarget, std::move(propList));
                   },
                   [](const toml::table& tbl)
                       -> std::unique_ptr<BasicAnimationTrack> {
                       return AnimationTrack<T>::LoadFromToml(tbl);
                   }});
        return *this;
    }

private:
    std::unordered_map<type_info_type, Method> methods_;
};

class Animation final : public Asset {
public:
    using track_base_type = BasicAnimationTrack;
    using track_pointer_type = std::unique_ptr<track_base_type>;
    using container_type = std::vector<track_pointer_type>;

    static Animation Null;

    explicit operator bool() const { return !tracks_.empty(); }

    Animation() = default;

    Animation(container_type&& tracks) : tracks_(std::move(tracks)) {}

    explicit Animation(const toml::table&);
    explicit Animation(const std::filesystem::path&);

    auto& Tracks() const { return tracks_; }

    void RemoveTrack(size_t index) { tracks_.erase(tracks_.begin() + index); }

    void AddTrack(std::unique_ptr<BasicAnimationTrack>&& track) {
        tracks_.emplace_back(std::move(track));
    }

    TimeType Duration() const {
        TimeType time = 0;
        for (auto& track : tracks_) {
            time = std::max(time, track->Duration());
        }
        return time;
    }

    toml::table Save2Toml() const override {
        toml::table tbl;
        tbl.emplace("path", RelativePath().string());
        toml::array arr;
        for (auto& track : tracks_) {
            arr.push_back(track->Save2Toml());
        }
        tbl.emplace("tracks", arr);
        return tbl;
    }

private:
    container_type tracks_;
};

template <>
std::unique_ptr<Animation> LoadAssetFromMetaTable(const toml::table& tbl);

using AnimationHandle = Handle<Animation>;

class AnimationManager final : public Manager<Animation> {
public:
    static FileType GetFileType() { return FileType::Animation; }

    std::shared_ptr<Animation> CreateSolitaryFromTracks(
        typename Animation::container_type&& tracks);
    AnimationHandle Load(const std::filesystem::path&);
};

class AnimationPlayer final {
public:
    enum class Direction {
        Forward = 1,
        Backward = -1,
    };

    using animation_type = Animation;

    AnimationPlayer()
        : mgr_{&ECS::Instance().World().res_mut<AnimationManager>().get()} {}

    AnimationPlayer(const AnimationPlayer&) = delete;

    AnimationPlayer(AnimationPlayer&& o) { swap(o, *this); }

    AnimationPlayer& operator=(AnimationPlayer&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    AnimationPlayer& operator=(const AnimationPlayer&) = delete;

    AnimationHandle Anim() const { return handle_; }

    void SetDir(Direction dir) { dir_ = dir; }

    Direction GetDir() const { return dir_; }

    void ChangeAnim(AnimationHandle anim) {
        handle_ = anim;
        Reset();
    }

    void Step(TimeType step) {
        if (isPlaying_) {
            curTime_ += static_cast<int>(dir_) * step;
        }
    }

    void Play() { isPlaying_ = true; }

    bool IsPlaying() const { return isPlaying_; }

    void Stop() { isPlaying_ = false; }

    void SetTick(TimeType t) {
        curTime_ = std::clamp<int>(t, 0, static_cast<int>(Duration()));
    }

    auto GetTick() const { return curTime_; }

    bool Empty() const { return !handle_; }

    void Reset() {
        if (dir_ == Direction::Forward) {
            curTime_ = 0;
        } else {
            curTime_ = static_cast<int>(Duration());
        }
    }

    bool IsValid() const { return mgr_->Has(handle_); }

    TimeType Duration() const {
        if (mgr_->Has(handle_)) {
            auto& anim = mgr_->Get(handle_);
            return anim.Duration();
        }
        return 0;
    }

    void Sync(gecs::entity, gecs::registry);

private:
    AnimationManager* mgr_{};
    Direction dir_ = Direction::Forward;
    int curTime_ = 0;
    AnimationHandle handle_;
    bool isPlaying_ = false;

    void getTarget(const mirrow::drefl::type* type,
                   const std::vector<std::string>& propertyLink, size_t idx,
                   mirrow::drefl::any& obj);

    friend void swap(AnimationPlayer& o1, AnimationPlayer& o2) {
        using std::swap;

        swap(o1.mgr_, o2.mgr_);
        swap(o1.dir_, o2.dir_);
        swap(o1.curTime_, o2.curTime_);
        swap(o1.handle_, o2.handle_);
        swap(o1.isPlaying_, o2.isPlaying_);
    }
};

}  // namespace nickel