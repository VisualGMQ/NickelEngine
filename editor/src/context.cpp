#include "context.hpp"
#include "config.hpp"
#include "core/gogl.hpp"

EditorContext::EditorContext() {
    renderBuffer_ = std::make_unique<nickel::gogl::RenderBuffer>(
        EditorWindowWidth, EditorWindowHeight);
    gameContentTexture_ = std::make_unique<nickel::gogl::Texture>(
        nickel::gogl::Texture::Type::Dimension2, nullptr, EditorWindowWidth,
        EditorWindowHeight, nickel::gogl::Sampler::CreateLinearRepeat(),
        nickel::gogl::Format::RGBA, nickel::gogl::Format::RGBA,
        nickel::gogl::DataType::UByte);
    gameContentTarget_ = std::make_unique<nickel::gogl::Framebuffer>(
        nickel::gogl::FramebufferAccess::ReadDraw);
    gameContentTarget_->AttachColorTexture2D(*gameContentTexture_);
    gameContentTarget_->AttacheRenderBuffer(*renderBuffer_);

    GLenum err;
    if (!gameContentTarget_->CheckValid(&err)) {
        LOGW(nickel::log_tag::Renderer,
            // TODO: output human readable error
             "create game content framebuffer failed! ",  err);
    }
}

EditorContext::~EditorContext() {
    renderBuffer_.reset();
    gameContentTexture_.reset();
    gameContentTarget_.reset();
}