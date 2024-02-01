#include "game_window.hpp"
#include "context.hpp"
#include "util.hpp"

GameWindow::GameWindow() {
    auto screenSize = nickel::Screen::Instance().Size();
    rbo_ = std::make_unique<nickel::gogl::RenderBuffer>(screenSize.w,
                                                        screenSize.h);
    texture_ = std::make_unique<nickel::gogl::Texture>(
        nickel::gogl::Texture::Type::Dimension2, nullptr, screenSize.w,
        screenSize.h, nickel::gogl::Sampler::CreateLinearRepeat(),
        nickel::gogl::Format::RGBA, nickel::gogl::Format::RGBA,
        nickel::gogl::DataType::UByte);
    fbo_ = std::make_unique<nickel::gogl::Framebuffer>(
        nickel::gogl::FramebufferAccess::ReadDraw);
    fbo_->AttachColorTexture2D(*texture_);
    fbo_->AttacheRenderBuffer(*rbo_);

    GLenum err;
    if (!fbo_->CheckValid(&err)) {
        LOGW(nickel::log_tag::Renderer,
             // TODO: output human readable error
             "create game content framebuffer failed! ", err);
    }

    auto& camera = nickel::ECS::Instance().World().res_mut<nickel::Camera>().get();
    auto& uiCamera = nickel::ECS::Instance().World().res_mut<nickel::ui::Context>()->camera;

    camera.SetRenderTarget(*fbo_);
    uiCamera.SetRenderTarget(*fbo_);
    camera.SetProject(nickel::cgmath::CreateOrtho(0, fbo_->Size().w, 0,
                                                  fbo_->Size().h, 1000, -1000));
    uiCamera.SetProject(nickel::cgmath::CreateOrtho(
        0, fbo_->Size().w, 0, fbo_->Size().h, 1000, -1000));
}

void drawCoordLine(const nickel::cgmath::Vec2& winSize,
                   nickel::Renderer2D& renderer, nickel::Camera& camera,
                   nickel::Camera& uiCamera) {
    renderer.BeginRenderTexture(camera);
    nickel::cgmath::Rect rect{0, 0, winSize.w, winSize.h};
    renderer.DrawRect(rect, {0, 0, 1, 1});
    // TODO: clip line start&end point to draw
    renderer.DrawLine({-10000, 0}, {10000, 0}, {1, 0, 0, 1});
    renderer.DrawLine({0, -10000}, {0, 10000}, {0, 1, 0, 1});

    renderer.EndRender();
}

void GameWindow::update() {
    auto& ctx = nickel::ECS::Instance().World().res_mut<EditorContext>().get();
    auto& renderer = nickel::ECS::Instance().World().res_mut<nickel::Renderer2D>().get();
    auto& camera = nickel::ECS::Instance().World().res_mut<nickel::Camera>().get();
    auto& uiCamera = nickel::ECS::Instance().World().res_mut<nickel::ui::Context>()->camera;

    auto gameWindownSize = ctx.projectInfo.windowData.size;

    drawCoordLine(gameWindownSize, renderer, camera, uiCamera);

    auto oldStyle = ImGui::GetStyle();
    auto newStyle = oldStyle;
    newStyle.WindowPadding.x = 0;
    newStyle.WindowPadding.y = 0;
    ImGui::GetStyle() = newStyle;

    auto regionMin = ImGui::GetWindowContentRegionMin();
    auto regionMax = ImGui::GetWindowContentRegionMax();
    auto windowPos = ImGui::GetWindowPos() + regionMin;
    auto windowSize = regionMax - regionMin;

    ImVec2 uvMin = {0, 1};
    ImVec2 uvMax = {windowSize.x / fbo_->Size().w,
                    (fbo_->Size().h - windowSize.y) / fbo_->Size().h};

    ImGui::GetWindowDrawList()->AddImage(
        (ImTextureID)texture_->Id(), windowPos,
        ImVec2{windowPos.x + windowSize.x, windowPos.y + windowSize.y}, uvMin,
        uvMax);

    auto& io = ImGui::GetIO();
    nickel::cgmath::Vec2 halfWindowSize{windowSize.x * 0.5f,
                                        windowSize.y * 0.5f};

    if (io.MouseWheel && ImGui::IsWindowHovered()) {
        scale_ += ScaleFactor * io.MouseWheel;
        scale_ = scale_ < minScaleFactor ? minScaleFactor : scale_;
    }
    if (ImGui::IsWindowHovered() &&
        (io.MouseDelta.x != 0 || io.MouseDelta.y != 0) &&
        io.MouseDown[ImGuiMouseButton_Middle]) {
        offset_ += nickel::cgmath::Vec2{io.MouseDelta.x, io.MouseDelta.y};
    }

    auto view = ScaleByAnchorAsMat({0, 0}, scale_, halfWindowSize, offset_);

    auto pos = ImGui::GetMousePos() - ImGui::GetWindowPos() -
               ImGui::GetWindowContentRegionMin();
    nickel::cgmath::Vec2 translatedPos{pos.x, pos.y};
    translatedPos = (translatedPos - halfWindowSize) *
                        nickel::cgmath::Vec2{1.0f / scale_, 1.0f / scale_} -
                    offset_ + halfWindowSize;
    srtGizmos_.SetMouseRelativePos(
        nickel::cgmath::Vec2{translatedPos.x, translatedPos.y});
    srtGizmos_.SetFBOScale(scale_);
    srtGizmos_.SetEventHandleState(
        ImGui::IsMouseHoveringRect(windowPos, windowPos + windowSize));
    if (ImGui::IsWindowFocused()) {
        if (ImGui::IsKeyPressed(ImGuiKey_S)) {
            srtGizmos_.SetMode(SRTGizmos::Mode::Scale);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_R)) {
            srtGizmos_.SetMode(SRTGizmos::Mode::Rotate);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_T)) {
            srtGizmos_.SetMode(SRTGizmos::Mode::Translate);
        }
    }
    srtGizmos_.Update(*nickel::ECS::Instance().World().cur_registry());

    camera.SetView(view);
    uiCamera.SetView(view);

    ImGui::GetStyle() = oldStyle;
}
