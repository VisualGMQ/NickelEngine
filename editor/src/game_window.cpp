#include "game_window.hpp"
#include "context.hpp"
#include "util.hpp"

GameWindow::GameWindow() {
}

void GameWindow::update() {
    auto& texture = EditorContext::Instance().texture;
    auto drawList = ImGui::GetWindowDrawList();
    ImGuizmo::SetDrawlist(drawList);

    // draw game content
    auto api = nickel::ECS::Instance()
                   .World()
                   .res<nickel::rhi::Adapter>()
                   ->RequestAdapterInfo()
                   .api;
#ifdef NICKEL_HAS_VULKAN
    auto vkCtx =
        nickel::ECS::Instance().World().res_mut<plugin::ImGuiVkContext>();
    drawList->AddImage(vkCtx->GetTextureBindedDescriptorSet(*texture), {0, 0},
                       ImVec2(texture->Width(), texture->Height()));
#endif

    auto reg = nickel::ECS::Instance().World().cur_registry();
    auto camera = reg->res_mut<nickel::Camera>();
    auto& uiCamera = reg->res_mut<nickel::ui::UIContext>()->renderCtx.camera;
    auto mouse = reg->res<nickel::Mouse>();

    // draw grid
    auto gameCanvaSize = camera->GetTarget().Texture().Extent();
    nickel::cgmath::Vec2 halfGameCanvaSize{gameCanvaSize.width * 0.5f,
                                           gameCanvaSize.height * 0.5f};
    auto halfGameWindowSize =
        EditorContext::Instance().projectInfo.windowData.size * 0.5;

    std::array points = {
  // rect
        nickel::cgmath::Vec2{-halfGameWindowSize.x, -halfGameWindowSize.y},
        nickel::cgmath::Vec2{ halfGameWindowSize.x, -halfGameWindowSize.y},
        nickel::cgmath::Vec2{ halfGameWindowSize.x,  halfGameWindowSize.y},
        nickel::cgmath::Vec2{-halfGameWindowSize.x,  halfGameWindowSize.y},

 // origin
        nickel::cgmath::Vec2{                    0,                     0},
    };

    for (auto& pt : points) {
        pt *= camera->Scale();
        pt += halfGameCanvaSize + nickel::cgmath::Vec2{camera->Position().x,
                                                       -camera->Position().y} *
                                      camera->Scale();
    }

    drawList->AddRect(ImVec2{points[2].x, points[2].y},
                      ImVec2{points[0].x, points[0].y},
                      ImGui::GetColorU32({0, 0, 1, 1}));

    drawList->AddLine({points.back().x - 10000, points.back().y},
                      {points.back().x + 10000, points.back().y},
                      ImGui::GetColorU32({1.0, 0.0, 0.0, 0.5}));
    drawList->AddLine({points.back().x, points.back().y - 10000},
                      {points.back().x, points.back().y + 10000},
                      ImGui::GetColorU32({0.0, 1.0, 0.0, 0.5}));

    // use ImGuizmo
    if (IsFocused()) {
        if (mouse->MiddleBtn().IsPressing()) {
            auto offset = mouse->Offset();
            auto scale = camera->Scale();
            camera->Move(nickel::cgmath::Vec2{-offset.x, offset.y} / scale);
            uiCamera.Move(nickel::cgmath::Vec2{-offset.x, offset.y} / scale);
        }

        if (auto wheel = mouse->WheelOffset(); wheel.y != 0) {
            auto scale = camera->Scale();
            auto newScale = scale + wheel.y * nickel::cgmath::Vec2{0.01, 0.01};
            if (newScale.x <= 0 || newScale.y <= 0) {
                newScale.Set(0.01, 0.01);
            }
            camera->ScaleTo(newScale);
            uiCamera.ScaleTo(newScale);
        }
    }

    auto selectedEnt = EditorContext::Instance().entityListWindow.GetSelected();
    if (!reg->alive(selectedEnt) || !reg->has<nickel::Transform>(selectedEnt)) {
        return;
    }

    auto& transform = reg->get_mut<nickel::Transform>(selectedEnt);

    auto keyboard = reg->res<nickel::Keyboard>();
    if (keyboard->Key(nickel::Key::Q).IsPressed()) {
        guizmoOperation_ = ImGuizmo::TRANSLATE;
    }
    if (keyboard->Key(nickel::Key::W).IsPressed()) {
        guizmoOperation_ = ImGuizmo::ROTATE;
    }
    if (keyboard->Key(nickel::Key::E).IsPressed()) {
        guizmoOperation_ = ImGuizmo::SCALE;
    }
    if (ImGui::RadioButton("Translate",
                           guizmoOperation_ == ImGuizmo::TRANSLATE))
        guizmoOperation_ = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", guizmoOperation_ == ImGuizmo::ROTATE))
        guizmoOperation_ = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", guizmoOperation_ == ImGuizmo::SCALE))
        guizmoOperation_ = ImGuizmo::SCALE;

    nickel::cgmath::Mat44 matrix;
    auto windowSize = reg->res<nickel::Window>()->Size();
    float matrixRot[3] = {0, 0, transform.rotation};
    float matrixTrans[3] = {transform.translation.x, transform.translation.y,
                            0};
    float matrixScale[3] = {transform.scale.x, transform.scale.y, 0};
    ImGuizmo::RecomposeMatrixFromComponents(matrixTrans, matrixRot, matrixScale,
                                            matrix.data);

    if (guizmoOperation_ != ImGuizmo::SCALE) {
        if (ImGui::RadioButton("Local", guizmoMode_ == ImGuizmo::LOCAL))
            guizmoMode_ = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", guizmoMode_ == ImGuizmo::WORLD))
            guizmoMode_ = ImGuizmo::WORLD;
    }

    auto snap = nickel::cgmath::Vec3{1, 1, 0};
    if (guizmoOperation_ == ImGuizmo::OPERATION::TRANSLATE) {
        snap.Set(1, 1, 1);
    } else if (guizmoOperation_ == ImGuizmo::OPERATION::ROTATE) {
        snap.Set(0, 0, 1);
    } else if (guizmoOperation_ == ImGuizmo::OPERATION::SCALE) {
        snap.Set(0, 0, ScaleFactor);
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, 1536, 864);
    auto view = nickel::cgmath::CreateScale({1, -1, 1}) * camera->View();
    ImGuizmo::Manipulate(view.data, camera->Project().data, guizmoOperation_,
                         guizmoMode_, matrix.data, nullptr, snap.data);

    ImGuizmo::DecomposeMatrixToComponents(matrix.data, matrixTrans, matrixRot,
                                          matrixScale);

    if (ImGuizmo::IsUsing()) {
        if (guizmoOperation_ == ImGuizmo::OPERATION::TRANSLATE) {
            transform.translation.x = matrixTrans[0];
            transform.translation.y = matrixTrans[1];
        }
        if (guizmoOperation_ == ImGuizmo::OPERATION::ROTATE) {
            transform.rotation = matrixRot[2];
        }
        if (guizmoOperation_ == ImGuizmo::OPERATION::SCALE) {
            transform.scale.x = matrixScale[0];
            transform.scale.y = matrixScale[1];
        }
    }
}
