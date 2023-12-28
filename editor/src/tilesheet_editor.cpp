#include "tilesheet_editor.hpp"

void TilesheetViewCanva::additionalDraw(ImDrawList* drawList,
                                        const nickel::Texture& texture,
                                        const nickel::cgmath::Vec2& canvaMin) {
    auto halfSize = texture.Size() * 0.5;
    auto regionMin = nickel::cgmath::Vec2{-halfSize.w, -halfSize.h};
    auto regionMax = nickel::cgmath::Vec2{halfSize.w, halfSize.h};
    nickel::TilesheetInnerAccessor accessor{*tilesheet_};

    auto p1 = transformPt(regionMin) + canvaMin;
    auto p2 = transformPt(regionMax) + canvaMin;
    drawList->AddRect({p1.x, p1.y}, {p2.x, p2.y},
                      ImGui::GetColorU32({1, 1, 0, 1}), 1.0f);

    nickel::cgmath::Rect canvaRect{canvaMin, GetSize()};

    for (int i = 0; i < tilesheet_->Col() * tilesheet_->Row(); i++) {
        auto tile = tilesheet_->Get(i);
        auto tileP1 = transformPt(tile.region.position, -halfSize) + canvaMin;
        auto tileP2 =
            transformPt(tile.region.size + tile.region.position, -halfSize) +
            canvaMin;
        auto mousePos = nickel::cgmath::Vec2{ImGui::GetMousePos().x,
                                             ImGui::GetMousePos().y};
        nickel::cgmath::Rect tileRect{tileP1, tileP2 - tileP1};
        if (!canvaRect.IsIntersect(tileRect)) {
            continue;
        }
        auto intersectRect = canvaRect.Intersect(tileRect);
        if (intersectRect.IsPtIn(mousePos)) {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                if (fn_) {
                    fn_(tile);
                }
                owner_->Hide();
                ImGui::CloseCurrentPopup();
            }
            drawList->AddRectFilled({tileP1.x, tileP1.y}, {tileP2.x, tileP2.y},
                                    ImGui::GetColorU32({1, 1, 0, 0.3}), 1.0f);
        } else {
            drawList->AddRect({tileP1.x, tileP1.y}, {tileP2.x, tileP2.y},
                              ImGui::GetColorU32({1, 1, 0, 1}), 1.0f);
        }
    }
}

void TilesheetEditor::update() {
    auto& assetMgr = gWorld->res_mut<nickel::AssetManager>().get();
    if (ImGui::BeginPopupModal(GetTitle().c_str(), &show_)) {
        if (!assetMgr.Has(handle_)) {
            ImGui::Text("no tilesheet");
            ImGui::EndPopup();
            return;
        }

        auto& tilesheet = assetMgr.Get(handle_);
        nickel::TilesheetInnerAccessor accessor(tilesheet);
        ImGui::Text("Margin");
        ImGui::BeginColumns("##margin_column", 4,
                            ImGuiOldColumnFlags_NoBorder |
                                ImGuiOldColumnFlags_NoPreserveWidths);
        {
            ImGui::DragScalar("##margin_left", ImGuiDataType_U32,
                              &accessor.Margin().left, 1.0, nullptr, nullptr,
                              "left: %d");
            ImGui::NextColumn();
            ImGui::DragScalar("##margin_right", ImGuiDataType_U32,
                              &accessor.Margin().right, 1.0, nullptr, nullptr,
                              "right: %d");
            ImGui::NextColumn();
            ImGui::DragScalar("##margin_top", ImGuiDataType_U32,
                              &accessor.Margin().top, 1.0, nullptr, nullptr,
                              "top: %d");
            ImGui::NextColumn();
            ImGui::DragScalar("##margin_bottom", ImGuiDataType_U32,
                              &accessor.Margin().bottom, 1.0, nullptr, nullptr,
                              "bottom: %d");
        }
        ImGui::EndColumns();

        ImGui::Text("Spacing");
        ImGui::BeginColumns("##spacing_column", 2,
                            ImGuiOldColumnFlags_NoBorder |
                                ImGuiOldColumnFlags_NoPreserveWidths);
        {
            ImGui::DragScalar("##spacing_x", ImGuiDataType_U32,
                              &accessor.Spacing().x, 1.0, nullptr, nullptr,
                              "x: %d");
            ImGui::NextColumn();
            ImGui::DragScalar("##spacing_y", ImGuiDataType_U32,
                              &accessor.Spacing().y, 1.0, nullptr, nullptr,
                              "y: %d");
        }
        ImGui::EndColumns();

        uint32_t min = 1;
        ImGui::Text("Row&Col");
        ImGui::BeginColumns("##margin_column", 4,
                            ImGuiOldColumnFlags_NoBorder |
                                ImGuiOldColumnFlags_NoPreserveWidths);
        {
            ImGui::NextColumn();
            ImGui::DragScalar("##row", ImGuiDataType_U32, &accessor.Row(), 1.0f,
                              (void*)&min, nullptr, "row: %d");
            ImGui::NextColumn();
            ImGui::DragScalar("##col", ImGuiDataType_U32, &accessor.Col(), 1.0f,
                              (void*)&min, nullptr, "col: %d");
        }
        ImGui::EndColumns();

        auto newTilesheet = nickel::Tilesheet(
            assetMgr.TextureMgr(), tilesheet.Handle(), accessor.Col(),
            accessor.Row(), accessor.Margin(), accessor.Spacing());
        newTilesheet.AssociateFile(tilesheet.RelativePath());
        tilesheet = std::move(newTilesheet);
        auto canvasSize = ImGui::GetContentRegionAvail();
        viewCanva_.Resize({canvasSize.x, canvasSize.y});
        viewCanva_.Update();

        ImGui::EndPopup();
    }
}