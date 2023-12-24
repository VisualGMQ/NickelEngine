#include "image_view_canva.hpp"

using namespace nickel;

inline cgmath::Vec2 calcPtTransform(const cgmath::Vec2& p,
                                    const cgmath::Vec2& offset, float scale,
                                    const cgmath::Vec2& center) {
    auto transP = p + offset;
    auto v = transP - center;
    return v * scale + center;
}

void TestImageDraw(cgmath::Vec2& offset, float& scale, TextureHandle handle) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                        ImVec2(0, 0));  // Disable padding
    ImGui::PushStyleColor(ImGuiCol_ChildBg,
                          IM_COL32(50, 50, 50, 255));  // Set a background color
    if (ImGui::BeginChild("canvas", ImVec2(0.0f, 0.0f), true,
                          ImGuiWindowFlags_NoMove)) {
        cgmath::Vec2 canvasMin{ImGui::GetCursorScreenPos().x,
                               ImGui::GetCursorScreenPos().y};
        cgmath::Vec2 canvasSize{ImGui::GetContentRegionAvail().x,
                                ImGui::GetContentRegionAvail().y};
        auto canvasCenter = canvasMin + canvasSize * 0.5;

        ImGui::InvisibleButton("canvas_btn", ImVec2{canvasSize.x, canvasSize.y},
                               ImGuiButtonFlags_MouseButtonLeft |
                                   ImGuiButtonFlags_MouseButtonRight);
        auto& io = ImGui::GetIO();
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            cgmath::Vec2 delta(io.MouseDelta.x, io.MouseDelta.y);
            offset += delta;
        }

        if (ImGui::IsItemHovered() && io.MouseWheel != 0) {
            scale += io.MouseWheel * 0.01;
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

            static auto calcTrans = [&](const cgmath::Vec2& p) {
                return calcPtTransform(p, offset, scale, canvasCenter);
            };

            minPt = calcTrans(minPt);
            maxPt = calcTrans(maxPt);
            drawList->AddImage(texture.Raw(), {minPt.x, minPt.y},
                               {maxPt.x, maxPt.y});
        }
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::EndChild();
}