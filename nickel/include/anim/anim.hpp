#pragma once

#include "anim/keyframe.hpp"
#include "core/handle.hpp"
#include "core/manager.hpp"
#include "core/singlton.hpp"
#include "misc/timer.hpp"

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
    virtual bool Empty() const = 0;
    virtual size_t Size() const = 0;
    virtual TimeType Duration() const = 0;

    auto& PropertyLink() const { return propLink_; }

    auto GetApplyTarget() const { return applyTypeInfo_; }

    virtual toml::table Save2Toml() const = 0;

private:
    const mirrow::drefl::type*
        applyTypeInfo_;  // which component should this track apply to
    PropertyLinkContainer propLink_;  // properties link in component
};

template <typename T>
class AnimationTrack final : public BasicAnimationTrack {
public:
    using keyframe_type = KeyFrame<T>;
    using container_type = std::vector<keyframe_type>;
    using time_type = typename keyframe_type::time_type;

    AnimationTrack() : BasicAnimationTrack(nullptr, {}) {}

    AnimationTrack(
        container_type&& keyPoints, const mirrow::drefl::type* applyTarget,
        typename BasicAnimationTrack::PropertyLinkContainer&& propList)
        : BasicAnimationTrack(applyTarget, std::move(propList)),
          keyPoints_(std::move(keyPoints)) {}

    bool Empty() const override { return keyPoints_.empty(); }

    size_t Size() const override { return keyPoints_.size(); }

    TimeType Duration() const override {
        return keyPoints_.empty() ? 0 : keyPoints_.back().timePoint;
    }

    auto& KeyPoints() const { return keyPoints_; }

    mirrow::drefl::any GetValueAt(TimeType t) const override {
        if (Empty()) {
            return {};
        }

        if (Size() == 1) {
            return mirrow::drefl::any_make_constref(keyPoints_[0].value);
        }

        auto& last = keyPoints_.back();
        if (t >= last.timePoint) {
            return mirrow::drefl::any_make_constref(last.value);
        }

        for (int i = 0; i < keyPoints_.size() - 1; i++) {
            auto& begin = keyPoints_[i];
            auto& end = keyPoints_[i + 1];

            if (begin.timePoint <= t && t < end.timePoint) {
                return mirrow::drefl::any_make_copy(begin.interpolate(
                    static_cast<float>(t) / (end.timePoint - begin.timePoint),
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
        for (auto& point : keyPoints_) {
            pointArr.push_back(point.Save2Toml());
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

        container_type keyPoints;
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

private:
    container_type keyPoints_;
};

class AnimTrackLoadMethods : public Singlton<AnimTrackLoadMethods, false> {
public:
    using deserialize_fn_type =
        std::unique_ptr<BasicAnimationTrack> (*)(const toml::table&);
    using type_info_type = const mirrow::drefl::type*;

    bool Contain(type_info_type type);
    deserialize_fn_type Find(type_info_type type);

    template <typename T>
    auto& RegistMethod() {
        auto type_info = mirrow::drefl::typeinfo<T>();
        methods_.emplace(
            type_info,
            [](const toml::table& tbl) -> std::unique_ptr<BasicAnimationTrack> {
                return AnimationTrack<T>::LoadFromToml(tbl);
            });
        return *this;
    }

private:
    std::unordered_map<type_info_type, deserialize_fn_type> methods_;
};

class Animation final : public Asset {
public:
    using track_base_type = BasicAnimationTrack;
    using track_pointer_type = std::unique_ptr<track_base_type>;
    using container_type = std::vector<track_pointer_type>;

    static Animation Null;

    explicit operator bool() const { return !tracks_.empty(); }

    Animation() = default;

    Animation(container_type&& tracks) : tracks_(std::move(tracks)) {
        for (auto& track : tracks_) {
            auto duration = track->Duration();
            lastTime_ = std::max<TimeType>(lastTime_, duration);
        }
    }

    Animation(const std::filesystem::path& root,
              const std::filesystem::path& filename);

    auto& Tracks() const { return tracks_; }

    TimeType Duration() const { return lastTime_; }

    /**
     * @brief save animation info to toml file(as asset file)
     */
    void Save(const std::filesystem::path& path) const;

    toml::table Save2Toml() const override {
        toml::table tbl;
        tbl.emplace("path", RelativePath().string());
        return tbl;
    }

private:
    container_type tracks_;
    TimeType lastTime_ = 0;
};

template <>
std::unique_ptr<Animation> LoadAssetFromToml(const toml::table& tbl,
                                             const std::filesystem::path& root);

using AnimationHandle = Handle<Animation>;

class AnimationManager final : public Manager<Animation> {
public:
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

    AnimationPlayer(AnimationManager& mgr) : mgr_(&mgr) {}

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
        curTime_ = std::clamp<int>(curTime_, 0, static_cast<int>(Duration()));
    }

    bool Empty() const { return !handle_; }

    AnimationHandle GetAnim() const { return handle_; }

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
    AnimationManager* mgr_;
    Direction dir_ = Direction::Forward;
    int curTime_ = 0;
    AnimationHandle handle_;
    bool isPlaying_ = false;
};

}  // namespace nickel