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
                                   ImGuiButtonFlags_MouseButtonRight |
                                   ImGuiButtonFlags_MouseButtonMiddle);
        ImGui::SetItemUsingMouseWheel();
        auto& io = ImGui::GetIO();
        if (ImGui::IsItemHovered() &&
            ImGui::IsMouseDragging(moveBtn_)) {
            cgmath::Vec2 delta(io.MouseDelta.x, io.MouseDelta.y);
            offset_ += delta;
        }

        if (ImGui::IsItemHovered() && io.MouseWheel != 0) {
            scale_ += io.MouseWheel * scaleIncStep;
        }

        if (ImGui::IsItemFocused() && ImGui::IsKeyDown(ImGuiKey_Space)) {
            offset_.Set(0, 0);
            scale_ = 1.0;
        }

        scale_ = scale_ < minScaleFactor ? minScaleFactor : scale_;

        // begin draw
        auto drawList = ImGui::GetWindowDrawList();
        auto assetMgr = nickel::ECS::Instance().World().res<AssetManager>();

        if (assetMgr->Has(handle_)) {
            auto& texture = assetMgr->Get(handle_);
            cgmath::Vec2 minPt{0, 0};
            cgmath::Vec2 maxPt{texture.Size()};

            minPt = transformPt(minPt, -texture.Size() * 0.5) + canvasMin;
            maxPt = transformPt(maxPt, -texture.Size() * 0.5) + canvasMin;
            auto api = ECS::Instance()
                           .World()
                           .res<nickel::rhi::Adapter>()
                           ->RequestAdapterInfo()
                           .api;
#ifdef NICKEL_HAS_VULKAN
            if (api == nickel::rhi::APIPreference::Vulkan) {
                auto imguiCtx =
                    ECS::Instance().World().res_mut<plugin::ImGuiVkContext>();
                drawList->AddImage(imguiCtx->GetTextureBindedDescriptorSet(texture),
                                {minPt.x, minPt.y}, {maxPt.x, maxPt.y});
            }
#elif defined(NICKEL_HAS_GLES3)
            if (api == nickel::rhi::APIPreference::GL) {
                drawList->AddImage((void*)static_cast<nickel::rhi::gl::TextureImpl*>(
                                       texture.RawTexture().Impl())
                                       ->id,
                                   {minPt.x, minPt.y}, {maxPt.x, maxPt.y});
            }
#endif

            additionalDraw(drawList, texture, canvasMin);
        }

        ImGui::PopClipRect();
    }

    ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(),
                                        ImGui::GetItemRectMax(), 0xFFFFFFFF);

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::EndGroup();
}