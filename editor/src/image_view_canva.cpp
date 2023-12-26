#include "image_view_canva.hpp"

using namespace nickel;

void ImageViewCanva::Update() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                        ImVec2(0, 0));  // Disable padding
    ImGui::PushStyleColor(ImGuiCol_ChildBg,
                          IM_COL32(50, 50, 50, 255));  // Set a background color

    ImGui::BeginGroup();
    {
        cgmath::Vec2 canvasMin{ImGui::GetCursorScreenPos().x,
                               ImGui::GetCursorScreenPos().y};
        auto canvasCenter = canvasMin + size_ * 0.5;
        auto canvasMax = canvasMin + size_;
        ImGui::PushClipRect({canvasMin.x, canvasMin.y},
                            {canvasMax.x, canvasMax.y}, true);

        ImGui::InvisibleButton("canvas", ImVec2{size_.w, size_.h},
                               ImGuiButtonFlags_MouseButtonLeft |
                                   ImGuiButtonFlags_MouseButtonRight);
        ImGui::SetItemUsingMouseWheel();
        auto& io = ImGui::GetIO();
        if (ImGui::IsItemHovered() &&
            ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            cgmath::Vec2 delta(io.MouseDelta.x, io.MouseDelta.y);
            offset_ += delta;
        }

        if (ImGui::IsItemHovered() && io.MouseWheel != 0) {
            scale_ += io.MouseWheel * scaleIncStep;
        }

        if (ImGui::IsItemFocused() && ImGui::IsKeyDown(ImGuiKey_Space)) {
            offset_.Set(0, 0);
        }

        scale_ = scale_ < minScaleFactor ? minScaleFactor : scale_;

        // begin draw
        auto drawList = ImGui::GetWindowDrawList();
        auto assetMgr = gWorld->res<AssetManager>();
        if (assetMgr->Has(handle_)) {
            auto& texture = assetMgr->Get(handle_);
            cgmath::Vec2 minPt{canvasMin.x, canvasMin.y};
            cgmath::Vec2 maxPt{texture.Size().w + canvasMin.x,
                               texture.Size().h + canvasMin.y};

            auto calcTrans = [&](const cgmath::Vec2& p) {
                return calcPtTransform(
                    p, offset_ + size_ * 0.5 - texture.Size() * 0.5, scale_,
                    canvasCenter);
            };

            minPt = calcTrans(minPt);
            maxPt = calcTrans(maxPt);
            drawList->AddImage(texture.Raw(), {minPt.x, minPt.y},
                               {maxPt.x, maxPt.y});
        }

        ImGui::PopClipRect();
    }

    ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(),
                                        ImGui::GetItemRectMax(), 0xFFFFFFFF);

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::EndGroup();
}