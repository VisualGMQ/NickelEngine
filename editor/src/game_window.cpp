#include "game_window.hpp"
#include "context.hpp"

GameWindow::GameWindow() {
    auto screenSize = nickel::Screen::Instance().Size();
    rbo_ = std::make_unique<nickel::gogl::RenderBuffer>(screenSize.w,
                                                        screenSize.w);
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

    auto& camera = gWorld->res_mut<nickel::Camera>().get();
    auto& uiCamera = gWorld->res_mut<nickel::ui::Context>()->camera;

    camera.SetRenderTarget(*fbo_);
    uiCamera.SetRenderTarget(*fbo_);
    camera.SetProject(nickel::cgmath::CreateOrtho(0, fbo_->Size().w, 0,
                                                  fbo_->Size().h, 1000, -1000));
    uiCamera.SetProject(nickel::cgmath::CreateOrtho(
        0, fbo_->Size().w, 0, fbo_->Size().h, 1000, -1000));
}

void drawCoordLine(EditorContext& ctx, nickel::Renderer2D& renderer,
                   const nickel::Camera& camera,
                   const nickel::Camera& uiCamera) {
    renderer.BeginRenderTexture(camera);
    nickel::cgmath::Rect rect{0, 0, ctx.projectInfo.windowData.size.w,
                              ctx.projectInfo.windowData.size.h};
    renderer.DrawRect(rect, {0, 0, 1, 1});
    // TODO: clip line start&end point to draw
    renderer.DrawLine({-10000, 0}, {10000, 0}, {1, 0, 0, 1});
    renderer.DrawLine({0, -10000}, {0, 10000}, {0, 1, 0, 1});

    renderer.EndRender();
}

void GameWindow::Update() {
    if (!IsVisible()) return;

    auto& ctx = gWorld->res_mut<EditorContext>().get();
    auto& renderer = gWorld->res_mut<nickel::Renderer2D>().get();
    auto& camera = gWorld->res_mut<nickel::Camera>().get();
    auto& uiCamera = gWorld->res_mut<nickel::ui::Context>()->camera;

    drawCoordLine(ctx, renderer, camera, uiCamera);

    auto oldStyle = ImGui::GetStyle();
    auto newStyle = oldStyle;
    newStyle.WindowPadding.x = 0;
    newStyle.WindowPadding.y = 0;
    ImGui::GetStyle() = newStyle;

    if (ImGui::Begin("game", &ctx.openGameWindow)) {
        auto windowPos = ImGui::GetWindowPos();
        auto windowSize = ImGui::GetWindowSize();
        auto& gameWindowSize = ctx.projectInfo.windowData.size;

        ImVec2 uvMin = {0, windowSize.y / fbo_->Size().h};
        ImVec2 uvMax = {windowSize.x / fbo_->Size().w, 0};

        ImGui::GetWindowDrawList()->AddImage(
            (ImTextureID)texture_->Id(), windowPos,
            ImVec2{windowPos.x + windowSize.x, windowPos.y + windowSize.y},
            uvMin, uvMax);

        auto& io = ImGui::GetIO();
        if (io.MouseWheel && ImGui::IsWindowHovered()) {
            scale_ += ScaleFactor * io.MouseWheel;
            scale_ = scale_ < minScaleFactor ? minScaleFactor : scale_;

            camera.ScaleTo(nickel::cgmath::Vec2{scale_, scale_});
            uiCamera.ScaleTo(nickel::cgmath::Vec2{scale_, scale_});
        }
        if (io.MouseDelta.x != 0 && io.MouseDelta.y != 0 &&
            ImGui::IsWindowHovered() && io.MouseDown[ImGuiMouseButton_Left]) {
            offset_ -= nickel::cgmath::Vec2{io.MouseDelta.x, io.MouseDelta.y};
        }
        auto finalOffset = offset_;
        if (nickel::cgmath::Vec2{camera.Position()} != finalOffset) {
            camera.MoveTo(finalOffset);
        }
        if (nickel::cgmath::Vec2{uiCamera.Position()} != finalOffset) {
            uiCamera.MoveTo(finalOffset);
        }
    }
    ImGui::End();

    ImGui::GetStyle() = oldStyle;
}
