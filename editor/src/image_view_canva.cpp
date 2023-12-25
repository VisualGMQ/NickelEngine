#include "image_view_canva.hpp"

using namespace nickel;

inline cgmath::Vec2 calcPtTransform(const cgmath::Vec2& p,
                                    const cgmath::Vec2& offset, float scale,
                                    const cgmath::Vec2& center) {
    auto transP = p + offset;
    auto v = transP - center;
    return v * scale + center;
}

void ShowImage(const cgmath::Vec2& canvasSize, cgmath::Vec2& offset,
               float& scale, TextureHandle handle) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                        ImVec2(0, 0));  // Disable padding
    ImGui::PushStyleColor(ImGuiCol_ChildBg,
                          IM_COL32(50, 50, 50, 255));  // Set a background color

    ImGui::BeginGroup();
    {
        cgmath::Vec2 canvasMin{ImGui::GetCursorScreenPos().x,
                               ImGui::GetCursorScreenPos().y};
        auto canvasCenter = canvasMin + canvasSize * 0.5;
        auto canvasMax = canvasMin + canvasSize;
        ImGui::PushClipRect({canvasMin.x, canvasMin.y}, {canvasMax.x, canvasMax.y}, true);

        ImGui::InvisibleButton("canvas", ImVec2{canvasSize.x, canvasSize.y},
                               ImGuiButtonFlags_MouseButtonLeft |
                                   ImGuiButtonFlags_MouseButtonRight);
        ImGui::SetItemUsingMouseWheel();
        auto& io = ImGui::GetIO();
        if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            cgmath::Vec2 delta(io.MouseDelta.x, io.MouseDelta.y);
            offset += delta;
        }

        if (ImGui::IsItemHovered() && io.MouseWheel != 0) {
            scale += io.MouseWheel * 0.1;
        }

        if (ImGui::IsItemFocused() && ImGui::IsKeyDown(ImGuiKey_Space)) {
            offset.Set(0, 0);
        }

        constexpr float minScaleFactor = 0.0001;

        scale = scale < minScaleFactor ? minScaleFactor : scale;

        // begin draw
        auto drawList = ImGui::GetWindowDrawList();
        auto assetMgr = gWorld->res<AssetManager>();
        if (assetMgr->Has(handle)) {
            auto& texture = assetMgr->Get(handle);
            cgmath::Vec2 minPt{canvasMin.x, canvasMin.y};
            cgmath::Vec2 maxPt{texture.Size().w + canvasMin.x,
                               texture.Size().h + canvasMin.y};

            auto calcTrans = [&](const cgmath::Vec2& p) {
                return calcPtTransform(
                    p, offset + canvasSize * 0.5 - texture.Size() * 0.5, scale,
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