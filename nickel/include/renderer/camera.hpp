#pragma once

#include "core/cgmath.hpp"
#include "core/gogl.hpp"
#include "window/window.hpp"

namespace nickel {
class Camera {
public:
    enum class Type {
        Ortho,
        Perspective,
    };

    static Camera CreateOrtho(float left, float right, float top, float bottom,
                              float near, float far) {
        return {Type::Ortho,
                cgmath::CreateOrtho(left, right, top, bottom, near, far),
                cgmath::Mat44::Identity()};
    }

    static Camera CreateOrthoByWindow(const Window& window) {
        auto size = window.Size();
        return CreateOrtho(0.0, size.w, 0.0, size.h, 1.0, -1.0);
    }

    auto& Project() const { return proj_; }

    void SetProject(const cgmath::Mat44& proj) { proj_ = proj; }
    void SetView(const cgmath::Mat44& view) { view_ = view; }

    auto& View() const { return view_; }

    void SetRenderTarget(gogl::Framebuffer& target) { renderTarget_ = &target; }

    auto& GetRenderTarget() const { return renderTarget_; }

    void ApplyRenderTarget() {
        if (renderTarget_) {
            renderTarget_->Bind();
            GL_CALL(glViewport(0, 0, renderTarget_->Size().w,
                               renderTarget_->Size().h));
            if (IsOrtho()) {
                // TODO: record cube to create ortho
                SetProject(cgmath::CreateOrtho(0, renderTarget_->Size().w, 0,
                                               renderTarget_->Size().h, 10000,
                                               -10000));
            }
        } else {
            GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        }
    }

    auto GetType() const { return type_; }

    bool IsOrtho() const { return type_ == Type::Ortho; }

    bool IsPerspective() const { return type_ == Type::Perspective; }

    auto& GetClearColor() const { return clearColor_; }

    void SetClearColor(const cgmath::Color& color) { clearColor_ = color; }

    void Move(const cgmath::Vec3& offset) {
        position_ += offset;
        recalcView();
    }

    void Move(const cgmath::Vec2& offset) {
        position_.x -= offset.x;
        position_.y -= offset.y;
        recalcView();
    }

    void MoveTo(const cgmath::Vec3& pos) {
        position_ = -pos;
        recalcView();
    }

    void MoveTo(const cgmath::Vec2& pos) {
        position_.x = -pos.x;
        position_.y = -pos.y;
        recalcView();
    }

    void ScaleTo(const cgmath::Vec3& scale) {
        scale_ = scale;
        recalcView();
    }

    void ScaleTo(const cgmath::Vec2& scale) {
        scale_.x = scale.x;
        scale_.y = scale.y;
        scale_.z = 1.0;
        recalcView();
    }

    auto& Position() const { return position_; }

    auto& Scale() const { return scale_; }

private:
    Camera(Type type, const cgmath::Mat44& proj, const cgmath::Mat44 view)
        : proj_{proj}, view_{view}, type_{type} {}

    cgmath::Mat44 proj_;
    cgmath::Mat44 view_;
    cgmath::Vec3 position_;
    cgmath::Vec3 scale_{1, 1};

    gogl::Framebuffer* renderTarget_ = nullptr;
    Type type_;
    cgmath::Color clearColor_{0.1, 0.1, 0.1, 1};

    void recalcView() {
        view_ =
            cgmath::CreateScale(scale_) * cgmath::CreateTranslation(position_);
    }
};

}  // namespace nickel