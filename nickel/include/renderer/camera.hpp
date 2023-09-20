#pragma once

#include "pch.hpp"
#include "core/cgmath.hpp"
#include "core/geom.hpp"
#include "window/window.hpp"

namespace nickel {

class BaseCamera {
public:
    BaseCamera()
        : view_{cgmath::Mat44::Identity()},
          project_(cgmath::Mat44::Identity()) {}

    const cgmath::Mat44& View() const { return view_; }

    const cgmath::Mat44& Project() const { return project_; }

    virtual ~BaseCamera() = default;

protected:
    cgmath::Mat44 view_;
    cgmath::Mat44 project_;
};

class Camera2D : public BaseCamera {
public:
    struct ConfigData final {
        float left, right, top, bottom, near, far;
    };

    static Camera2D Create(float left, float right, float top, float bottom,
                           float near, float far) {
        return Camera2D(left, right, top, bottom, near, far);
    }

    static Camera2D Default(const Window& window) {
        auto size = window.Size();
        return Camera2D(0.0, size.w, 0.0, size.h, 1.0, -1.0);
    }

    static std::optional<Camera2D> FromConfigFile(std::string_view filename);
    static Camera2D FromConfig(const toml::table&);

    void MoveTo(const cgmath::Vec2& position) {
        position_ = position;
        view_ = cgmath::CreateTranslation(
            cgmath::Vec3{-position_.x, -position_.y, 0.0});
    }

    void Move(const cgmath::Vec2& offset) {
        position_ += offset;
        view_ = cgmath::CreateTranslation(
            cgmath::Vec3{-position_.x, -position_.y, 0.0});
    }

    // void ScaleTo(const cgmath::Vec2& scale) {
    //     scale_ = scale;
    // }

    auto& Position() const { return position_; }

private:
    cgmath::Vec2 position_;
    cgmath::Vec2 scale_;
    geom::Cube cube_;

    Camera2D(float left, float right, float top, float bottom, float near,
             float far);
};

class Camera final {
public:
    enum class Type {
        Dimension2,
        Dimension3,
    };

    Camera(Camera2D&& camera)
        : camera_(std::make_unique<Camera2D>(camera)),
          type_(Type::Dimension2) {}

    const cgmath::Mat44& View() const { return camera_->View(); }

    const cgmath::Mat44& Project() const { return camera_->Project(); }

    Type GetType() const { return type_; }

    Camera2D* as_2d() {
        if (type_ == Type::Dimension2) {
            return static_cast<Camera2D*>(camera_.get());
        } else {
            return nullptr;
        }
    }

private:
    std::unique_ptr<BaseCamera> camera_;
    Type type_;
};

}  // namespace nickel