#include "context.hpp"
#include "config.hpp"
#include "core/gogl.hpp"

EditorContext::EditorContext() {
}

void EditorContext::Init() {
    /* NOTE: currently we can't load custom font due to bug in ImGui itself
    auto& io = ImGui::GetIO();
    imguiIconFont = io.Fonts->AddFontFromFileTTF(
        "editor/resources/fonts/forkawesome-webfont.ttf", 20, nullptr,
        io.Fonts->GetGlyphRangesDefault());
    if (!imguiIconFont) {
        LOGW(nickel::log_tag::Editor, "load icon font failed");
        imguiIconFont = nullptr;
    }
    */

    renderBuffer = std::make_unique<nickel::gogl::RenderBuffer>(
        EditorWindowWidth, EditorWindowHeight);
    gameContentTexture = std::make_unique<nickel::gogl::Texture>(
        nickel::gogl::Texture::Type::Dimension2, nullptr, EditorWindowWidth,
        EditorWindowHeight, nickel::gogl::Sampler::CreateLinearRepeat(),
        nickel::gogl::Format::RGBA, nickel::gogl::Format::RGBA,
        nickel::gogl::DataType::UByte);
    gameContentTarget = std::make_unique<nickel::gogl::Framebuffer>(
        nickel::gogl::FramebufferAccess::ReadDraw);
    gameContentTarget->AttachColorTexture2D(*gameContentTexture);
    gameContentTarget->AttacheRenderBuffer(*renderBuffer);

    GLenum err;
    if (!gameContentTarget->CheckValid(&err)) {
        LOGW(nickel::log_tag::Renderer,
             // TODO: output human readable error
             "create game content framebuffer failed! ", err);
    }
}

EditorContext::~EditorContext() {
    renderBuffer.reset();
    gameContentTexture.reset();
    gameContentTarget.reset();
}