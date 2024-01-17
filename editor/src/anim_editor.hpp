#pragma once
#include "imgui_plugin.hpp"
#include "widget.hpp"

class Sequence {
public:
    Sequence()
        : animMgr{gWorld->res_mut<nickel::AssetManager>()->AnimationMgr()} {}

    int GetItemCount() const {
        if (!animMgr.Has(player.Anim())) {
            return 0;
        }

        auto& anim = animMgr.Get(player.Anim());
        return anim.Tracks().size();
    }

    nickel::BasicAnimationTrack* Get(int index) {
        if (!animMgr.Has(player.Anim())) {
            return {};
        }

        auto& anim = animMgr.Get(player.Anim());
        return (anim.Tracks()[index]).get();
    }

    std::string GetItemLabel(int index) const {
        if (!animMgr.Has(player.Anim())) {
            return nullptr;
        }
        auto& track = animMgr.Get(player.Anim()).Tracks()[index];
        auto name = track->GetApplyTarget()->name();
        for (auto& prop : track->PropertyLink()) {
            name += "-" + prop;
        }

        return name;
    }

    nickel::AnimationPlayer player;
    gecs::entity entity = gecs::null_entity;
    nickel::AnimationManager& animMgr;
};

class AnimationEditor;

class TrackMenu final : public PopupMenu {
public:
    enum class Type {
        OnTrack,
        OnBlock,
    } type;

    void TrackMode(int index, int clickedFrame) {
        if (!IsVisible()) {
            this->clickedFrame_ = clickedFrame;
            trackIndex_ = index;
            type = Type::OnTrack;
        }
    }

    void KeyframeMode(int trackIndex, int keyframeIndex) {
        if (!IsVisible()) {
            this->trackIndex_ = trackIndex;
            this->keyframeIndex_ = keyframeIndex;
            type = Type::OnBlock;
        }
    }

    explicit TrackMenu(AnimationEditor& e)
        : PopupMenu("animEditorTrackMenu"), owner_{&e} {}

protected:
    int trackIndex_;
    int keyframeIndex_;
    int clickedFrame_;
    AnimationEditor* owner_{};

    void update() override;
};

class TrackNameMenu final : public PopupMenu {
public:
    int trackIndex;

    explicit TrackNameMenu(AnimationEditor& owner)
        : PopupMenu("animEditorTrackNameMenu"), owner_{&owner} {}

protected:
    AnimationEditor* owner_;

    void update() override;
};

class AnimationEditor final : public Window {
public:
    std::unique_ptr<Sequence> sequence;

    AnimationEditor() : trackMenu_(*this), trackNameMenu_(*this) {
        SetTitle("animation editor");
        sequence = std::make_unique<Sequence>();
    }

    void ChangePlayer(gecs::entity ent, nickel::AnimationHandle anim) {
        sequence->player.ChangeAnim(anim);
        sequence->entity = ent;
    }

    void Update() override;

private:
    int trackHeight_ = 20;
    int timelineBarHeight_ = 20;
    float timelineScale_ = 0.5;
    float timelineScaleBarWidth_ = 100;
    int timelineMaxTime_ = 100000;
    float timelineTimeStep_ = 1000;
    int timelineColMaxWidth_ = 100;
    TrackMenu trackMenu_;
    TrackNameMenu trackNameMenu_;
    int w1_ = 200;
    int w2_ = 400;
    std::optional<int> trackIdx_;
    std::optional<int> keyframeIdx_;

    void renderTrackNameAndTool(const ImVec2& canvasPos, const ImVec2& canvasSize,
                         ImDrawList*);
    void renderTimelineTopBar(const ImVec2& canvasPos, const ImVec2& canvasSize,
                              ImDrawList* drawList);
    void renderTrackNames(const ImVec2& canvasPos, const ImVec2& canvasSize,
                          ImDrawList*);
    void renderOneTrack(int index, const ImVec2& minPoint,
                        const ImVec2& canvasSize, ImDrawList*);
    void renderInspector(const ImVec2& canvasPos, const ImVec2& canvasSize,
                         ImDrawList*);
    void renderTimeCursor(const ImVec2& minPoint, float canvasH, ImDrawList*);
    float getTimelineColX(float value);
    ImVec2 getKeyFrameCenter(float value, const ImVec2& trackMinPoint);

    float getColWidth() { return timelineColMaxWidth_ * timelineScale_; }

    int getColNum() { return timelineMaxTime_ / timelineTimeStep_; }
};