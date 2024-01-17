#include "anim_editor.hpp"
#include "show_component.hpp"

void TrackNameMenu::update() {
    if (ImGui::BeginPopup(GetTitle().c_str())) {
        if (ImGui::Button("delete")) {
            auto& seq = owner_->sequence;
            if (!seq->animMgr.Has(seq->player.Anim())) {
                ImGui::EndPopup();
                return;
            }
            auto& anim = seq->animMgr.Get(seq->player.Anim());
            anim.RemoveTrack(trackIndex);
            Hide();
        }
        ImGui::EndPopup();
    }
}

void TrackMenu::update() {
    if (ImGui::BeginPopup(GetTitle().c_str())) {
        auto& seq = owner_->sequence;
        if (!seq->animMgr.Has(seq->player.Anim())) {
            Hide();
            ImGui::EndPopup();
            return ;
        }
        auto& anim = seq->animMgr.Get(seq->player.Anim());
        auto& track = anim.Tracks()[trackIndex_];
        switch (type) {
            case Type::OnTrack:
                if (ImGui::Button("add keyframe")) {
                    auto& newFrame = track->AppendKeyFrame();
                    newFrame.timePoint = clickedFrame_;
                    Hide();
                }
                break;
            case Type::OnBlock:
                if (ImGui::Button("delete")) {
                    track->RemoveKeyFrame(keyframeIndex_);
                    Hide();
                }
                break;
        }
        ImGui::EndPopup();
    }
}

void AnimationEditor::Update() {
    if (ImGui::Begin(GetTitle().c_str(), &show_)) {
        ImGuiIO& io = ImGui::GetIO();
        nickel::cgmath::Vec2 mousePos(io.MousePos.x, io.MousePos.y);

        int sequenceCount = sequence->GetItemCount();
        if (sequenceCount == 0) {
            ImGui::End();
            return;
        }

        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        if (ImGui::BeginChild("track names", ImVec2(w1_, contentSize.y))) {
            ImVec2 canvasPos = ImGui::GetCursorScreenPos();
            ImVec2 canvasSize = ImGui::GetContentRegionAvail();
            renderTrackNameAndTool(canvasPos, canvasSize, ImGui::GetWindowDrawList());
        }
        ImGui::EndChild();
        ImGui::SameLine();

        // splitter
        constexpr float hsplitterW = 10;
        ImGui::InvisibleButton("hsplitter1",
                               ImVec2(hsplitterW, timelineBarHeight_));
        if (ImGui::IsItemActive()) w1_ += ImGui::GetIO().MouseDelta.x;

        auto splitLineTopPoint = ImGui::GetWindowPos() +
                                 ImGui::GetWindowContentRegionMin() +
                                 ImVec2(w1_, 0);
        ImGui::GetWindowDrawList()->AddLine(
            splitLineTopPoint, splitLineTopPoint + ImVec2(0, contentSize.y),
            0xFFFFFFFF);

        ImGui::SameLine();

        if (ImGui::BeginChild("tracks", ImVec2(w2_, contentSize.y), false,
                              ImGuiWindowFlags_HorizontalScrollbar)) {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 canvasPos = ImGui::GetCursorScreenPos();
            ImVec2 canvasSize = ImGui::GetContentRegionAvail();

            renderTimelineTopBar(canvasPos, canvasSize, drawList);
            auto contentMinPoint = canvasPos + ImVec2(0, timelineBarHeight_ + 2);
            ImGui::SetCursorScreenPos(contentMinPoint);
            for (int i = 0; i < sequenceCount; i++) {
                renderOneTrack(i, contentMinPoint, canvasSize, drawList);
            }
            ImGui::SetNextItemWidth(timelineScaleBarWidth_);
            renderTimeCursor(contentMinPoint, canvasSize.y, drawList);
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // splitter
        ImGui::InvisibleButton("hsplitter2",
                               ImVec2(hsplitterW, ImGui::GetWindowSize().y));
        if (ImGui::IsItemActive()) w2_ += ImGui::GetIO().MouseDelta.x;

        ImGui::SameLine();

        if (ImGui::BeginChild(
                "inspector",
                ImVec2(contentSize.x - w1_ - w2_ - 2 * hsplitterW, contentSize.y),
                false, ImGuiWindowFlags_HorizontalScrollbar)) {
            renderInspector(ImGui::GetCursorScreenPos(),
                            ImGui::GetContentRegionAvail(),
                            ImGui::GetWindowDrawList());
        }
        ImGui::EndChild();

        ImGui::PopStyleVar();

        trackMenu_.Update();
        trackNameMenu_.Update();

        auto reg = gWorld->cur_registry();
        if (reg->alive(sequence->entity) && sequence->player.IsPlaying()) {
            sequence->player.Sync(sequence->entity, *reg);
        }
    }

    ImGui::End();
}

void AnimationEditor::renderTrackNameAndTool(const ImVec2& canvasPos,
                                      const ImVec2& canvasSize,
                                      ImDrawList* drawList) {
    ImGui::SetCursorScreenPos(canvasPos);
    if (gWorld->cur_registry()->alive(sequence->entity)) {
        if (ImGui::Button("Play", ImVec2(w1_ / 3.0, timelineBarHeight_))) {
            sequence->player.Play();
        }
        ImGui::SameLine();
        if (ImGui::Button("Pause", ImVec2(w1_ / 3.0, timelineBarHeight_))) {
            sequence->player.Stop();
        }
        ImGui::SameLine();
        if (ImGui::Button("Rewind", ImVec2(w1_ / 3.0, timelineBarHeight_))) {
            sequence->player.Reset();
        }
    } else {
        ImGui::Text("no bind entity");
    }
    renderTrackNames(canvasPos, canvasSize, drawList);
}

void AnimationEditor::renderTimeCursor(const ImVec2& minPoint, float canvasH,
                                       ImDrawList* drawList) {
    auto currentFrame = sequence->player.GetTick();
    float x =  currentFrame / timelineTimeStep_ * getColWidth();
    drawList->AddLine(minPoint + ImVec2(x, 0),
                      minPoint + ImVec2(x, canvasH), 0xFF0000FF,
                      3.0f);
}

void AnimationEditor::renderTrackNames(const ImVec2& canvasPos,
                                       const ImVec2& canvasSize,
                                       ImDrawList* drawList) {
    drawList->AddLine(canvasPos + ImVec2(0, timelineBarHeight_),
                      canvasPos + ImVec2(canvasSize.x, timelineBarHeight_),
                      0xFFFFFFFF);
    for (int i = 0; i < sequence->GetItemCount(); i++) {
        auto label = sequence->GetItemLabel(i);
        auto y = timelineBarHeight_ + i * trackHeight_;
        ImGui::SetCursorScreenPos(ImVec2(canvasPos.x, canvasPos.y + y));
        ImGui::Button(label.c_str(), ImVec2(canvasSize.x, timelineBarHeight_));

        if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
            trackNameMenu_.Show();
            trackNameMenu_.trackIndex = i;
        }

        drawList->AddLine(
            canvasPos + ImVec2(0, timelineBarHeight_ + (i + 1) * trackHeight_),
            canvasPos +
                ImVec2(canvasSize.x, timelineBarHeight_ + (i + 1) * trackHeight_),
            0xFFFFFFFF);
    }
}

void AnimationEditor::renderTimelineTopBar(const ImVec2& canvasPos,
                                           const ImVec2& canvasSize,
                                           ImDrawList* drawList) {
    ImGui::InvisibleButton(
        "timeline",
        ImVec2((timelineColMaxWidth_ * timelineMaxTime_ / timelineTimeStep_) *
                   timelineScale_,
               timelineBarHeight_));

    float colWidth = getColWidth();
    int colNum = getColNum();

    if (ImGui::IsItemActive() &&
        ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        auto currentFrame = (ImGui::GetMousePos().x - canvasPos.x) / colWidth *
                       timelineTimeStep_;
        currentFrame = std::clamp<float>(
            currentFrame, 0, getColNum() * getColWidth() * timelineTimeStep_);
        if (ImGui::BeginTooltip()) {
            static char buf[64] = {0};
            snprintf(buf, sizeof(buf), "%f", currentFrame);
            ImGui::Text("%s", buf);
        }
        ImGui::EndTooltip();
        sequence->player.SetTick(currentFrame);
        auto reg = gWorld->cur_registry();
        if (reg->alive(sequence->entity)) {
            sequence->player.Sync(sequence->entity, *reg);
        }
    }
    drawList->AddLine(
        canvasPos + ImVec2(0, timelineBarHeight_),
        canvasPos + ImVec2(colWidth * getColNum(), timelineBarHeight_),
        0xFFFFFFFF);
    for (int i = 0; i < colNum; i++) {
        auto lineTop = canvasPos + ImVec2{i * colWidth, 15};
        auto lineBottom = canvasPos + ImVec2(i * colWidth, timelineBarHeight_);
        drawList->AddLine(lineTop, lineBottom, 0xFFFFFFFF);
        char buf[128] = {0};
        if (i == 0) {
            snprintf(buf, sizeof(buf), "ms");
        } else {
            snprintf(buf, sizeof(buf), "%.2f", i * timelineTimeStep_);
        }
        drawList->AddText({lineTop.x, canvasPos.y}, 0xFFFFFFFF, buf);
    }
}

void AnimationEditor::renderOneTrack(int index, const ImVec2& minPoint,
                                     const ImVec2& canvasSize,
                                     ImDrawList* drawList) {
    static char id[64] = {0};
    unsigned int color = 0xFFFFFFFF;
    auto& keyframes = sequence->Get(index)->KeyFrames();
    auto trackMinPoint = minPoint + ImVec2(0, trackHeight_ * index);
    snprintf(id, sizeof(id), "##track%d", index);

    // track background
    ImGui::SetNextItemAllowOverlap();
    ImGui::SetCursorScreenPos(trackMinPoint);
    ImGui::Button(id, ImVec2(getColWidth() * getColNum(), trackHeight_));

    // track background event handle
    if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
        trackMenu_.TrackMode(
            index, ((ImGui::GetMousePos().x - trackMinPoint.x) + ImGui::GetScrollX()) /
                       getColWidth() * timelineTimeStep_);
        trackMenu_.Show();
    }

    for (int i = 0; i < keyframes.size(); i++) {
        auto keyPoint = &keyframes[i]->timePoint;
        if (i < keyframes.size() - 1) {
            drawList->AddLine(
                getKeyFrameCenter(*keyPoint, trackMinPoint),
                getKeyFrameCenter(keyframes[i + 1]->timePoint, trackMinPoint),
                0xFFFFFFAA);
        }

        float x = getTimelineColX(*keyPoint);
        constexpr float blockWidth = 5;
        auto blockMinPoint = trackMinPoint + ImVec2(x - blockWidth * 0.5, 3);

        auto blockMaxPoint =
            blockMinPoint + ImVec2(blockWidth, trackHeight_ - 3);

        unsigned int color = 0xFFFF00FF;
        auto offsetX = -ImGui::GetScrollX();
        ImRect rect(blockMinPoint, blockMaxPoint);
        auto mousePos = ImGui::GetMousePos();

        // track block
        ImGui::SetCursorScreenPos(blockMinPoint);
        snprintf(id, sizeof(id), "block%d-%d", index, i);
        ImGui::InvisibleButton(id, blockMaxPoint - blockMinPoint);

        // track block event handle
        ImGui::GetIO().MouseDragThreshold = 0.1;

        if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
            trackMenu_.KeyframeMode( index, i);
            trackMenu_.Show();
        }

        if (ImGui::IsItemFocused()) {
            if (ImGui::IsItemActive() &&
                ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                *keyPoint += ImGui::GetMouseDragDelta().x / getColWidth() *
                             timelineTimeStep_;
                *keyPoint = std::clamp<int>(*keyPoint, 0, timelineMaxTime_);
                ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
                if (ImGui::BeginTooltip()) {
                    static char buf[64] = {0};
                    snprintf(buf, sizeof(buf), "%d", *keyPoint);
                    ImGui::Text("%s", buf);
                }
                ImGui::EndTooltip();
            }

            trackIdx_ = index;
            keyframeIdx_ = i;
        }

        if (trackIdx_ && keyframeIdx_ && trackIdx_.value() == index &&
            keyframeIdx_.value() == i) {
            color = 0xFF00FFFF;
        }

        // draw track block
        drawList->AddRectFilled(blockMinPoint, blockMaxPoint, color);
        drawList->AddLine(
            trackMinPoint + ImVec2(0, trackHeight_),
            trackMinPoint +
                ImVec2(getColWidth() * timelineMaxTime_, trackHeight_),
            0xFFFFFFFF);
    }
}

void AnimationEditor::renderInspector(const ImVec2& canvasPos,
                                      const ImVec2& canvasSize,
                                      ImDrawList* drawList) {
    ImGui::GetWindowDrawList()->AddLine(
        ImVec2(canvasPos.x, canvasPos.y),
        ImVec2(canvasPos.x, canvasPos.y + canvasSize.y), 0xFFFFFFFF);

    auto reg = gWorld->cur_registry();
    if (trackIdx_ && keyframeIdx_ &&
        trackIdx_.value() < sequence->GetItemCount() &&
        keyframeIdx_.value() < sequence->Get(trackIdx_.value())->Size()) {
        if (auto track = sequence->Get(trackIdx_.value()); track) {
            auto value = track->KeyFrames()[keyframeIdx_.value()]->GetValue();
            if (auto fn =
                    ComponentShowMethods::Instance().Find(value.type_info());
                fn) {
                fn(value.type_info(), value.type_info()->name(), value, *reg,
                   {});
            } else {
                ImGui::Text("don't know how to show this component");
            }
        }
    } else {
        ImGui::Text("no component");
    }
}

ImVec2 AnimationEditor::getKeyFrameCenter(float value,
                                          const ImVec2& trackMinPoint) {
    return trackMinPoint + ImVec2(getTimelineColX(value), trackHeight_ * 0.5);
}

float AnimationEditor::getTimelineColX(float value) {
    return (value / timelineTimeStep_) * getColWidth();
}