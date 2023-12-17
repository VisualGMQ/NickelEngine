#pragma once

#include "core/gogl.hpp"
#include "pch.hpp"
#include "core/cgmath.hpp"
#include "geom/geom3d.hpp"
#include "window/window.hpp"

namespace nickel {

class BaseCamera {
public:
    BaseCamera()
        : view_{cgmath::Mat44::Identity()},
          project_(cgmath::Mat44::Identity()) {}

    BaseCamera(BaseCamera&&) = default;

    const cgmath::Mat44& View() const { return view_; }

    const cgmath::Mat44& Project() const { return project_; }

    void SetRenderBuffer(gogl::Framebuffer& target) {
        renderTarget_ = &target;
    }

    auto GetRenderBuffer() const {
        return renderTarget_;
    }

    void ApplyRenderTarget() const {
        if (renderTarget_) {
            renderTarget_->Bind();
            GL_CALL(glViewport(0, 0, renderTarget_->Size().w,
                               renderTarget_->Size().h));
        } else {
            GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        }
    }

    virtual ~BaseCamera() = default;

protected:
    cgmath::Mat44 view_;
    cgmath::Mat44 project_;
    gogl::Framebuffer* renderTarget_ = nullptr;
};

class Camera2D : public BaseCamera {
public:
    struct ConfigData final {
        float left;
        float right;
        float top;
        float bottom;
        float near;
        float far;
    };

    static Camera2D Create(float left, float right, float top, float bottom,
                           float near, float far) {
        return Camera2D(left, right, top, bottom, near, far);
    }

    static Camera2D Default(const Window& window) {
        auto size = window.Size();
        return Camera2D(0.0, size.w, 0.0, size.h, 1.0, -1.0);
    }

    void MoveTo(const cgmath::Vec2& position) {
        position_ = position;
        recalcViewMat();
    }

    void Move(const cgmath::Vec2& offset) {
        position_ += offset;
        recalcViewMat();
    }

    void SetScale(const cgmath::Vec2& scale) {
        scale_ = scale;
        recalcViewMat();
    }

    auto& Scale() const {
        return scale_;
    }

    auto& Position() const { return position_; }

    void SetProject(float left, float right, float top, float bottom, float near,
             float far);

private:
    cgmath::Vec2 position_;
    cgmath::Vec2 scale_{1, 1};
    geom3d::Cube<float> cube_;

    Camera2D(float left, float right, float top, float bottom, float near,
             float far);

    void recalcViewMat() {
        view_ = cgmath::CreateTranslation(
            cgmath::Vec3{-position_.x, -position_.y, 0.0}) *
                cgmath::CreateScale(cgmath::Vec3{scale_.x, scale_.y, 1.0});
    }
};

class Camera final {
public:
    enum class Type {
        Dimension2,
        Dimension3,
    };

    Camera(Camera2D&& camera)
        : camera_(std::make_unique<Camera2D>(std::move(camera))),
          type_(Type::Dimension2) {}

    const cgmath::Mat44& View() const { return camera_->View(); }

    const cgmath::Mat44& Project() const { return camera_->Project(); }

    void SetRenderTarget(gogl::Framebuffer& target) {
        camera_->SetRenderBuffer(target);
    }

    auto GetRenderBuffer() const {
        return camera_->GetRenderBuffer();
    }

    void ApplyRenderTarget() const {
        camera_->ApplyRenderTarget();
    }

    void SetScale(const cgmath::Vec2& scale) {
        switch (type_) {
            case Type::Dimension2:
                As2D()->SetScale(scale);
                break;
            case Type::Dimension3:
                break;
        }
    }

    cgmath::Vec2 Scale() const {
        switch (type_) {
            case Type::Dimension2:
                return As2D()->Scale();
            case Type::Dimension3:
                return {1, 1};
        }
    }

    Type GetType() const { return type_; }

    Camera2D* const As2D() const {
        if (type_ == Type::Dimension2) {
            return static_cast<Camera2D*>(camera_.get());
        } else {
            return nullptr;
        }
    }

    Camera2D* As2D() {
        return const_cast<Camera2D*>(std::as_const(*this).As2D());
    }

private:
    std::unique_ptr<BaseCamera> camera_;
    Type type_;
};

}  // namespace nickel