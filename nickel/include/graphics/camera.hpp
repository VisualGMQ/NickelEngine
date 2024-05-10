#pragma once

#include "common/cgmath.hpp"
#include "common/ecs.hpp"
#include "graphics/rhi/rhi.hpp"

namespace nickel {

struct SphericalCoordCameraProxy;

class Camera {
public:
    friend class SphericalCoordCameraProxy;

    enum class Type {
        Ortho,
        Perspective,
    };

    static Camera CreateOrtho(float left, float right, float top, float bottom,
                              float near, float far,
                              const nickel::cgmath::Rect& viewport) {
        auto api = ECS::Instance()
                       .World()
                       .res<rhi::Adapter>()
                       ->RequestAdapterInfo()
                       .api;
        return {Type::Ortho,
                cgmath::CreateOrtho(left, right, top, bottom, near, far,
                                    api == rhi::APIPreference::GL),
                cgmath::Mat44::Identity(), viewport};
    }

    static Camera CreateOrthoByWindowRegion(const cgmath::Vec2& size) {
        auto halfSize = size * 0.5;
        return CreateOrtho(-halfSize.w, halfSize.w, halfSize.h, -halfSize.h,
                           1000.0, -1000.0, {0, 0, size.w, size.h});
    }

    auto& Project() const { return proj_; }

    void SetProject(const cgmath::Mat44& proj) { proj_ = proj; }

    void SetView(const cgmath::Mat44& view) { view_ = view; }

    auto& View() const { return view_; }

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

    void ScaleTo(const cgmath::Vec2& scale) {
        scale_ = scale;
        recalcView();
    }

    auto& Position() const { return position_; }

    auto& Scale() const { return scale_; }

    void SetRenderTarget(rhi::TextureView view) { target_ = view; }

    void SetViewport(const nickel::cgmath::Rect& rect) { viewport_ = rect; }

    auto& GetViewport() const { return viewport_; }

    void SetTarget2Default() { target_ = {}; }

    rhi::TextureView GetTarget() const { return target_; }

private:
    Camera(Type type, const cgmath::Mat44& proj, const cgmath::Mat44 view,
           const cgmath::Rect& viewport)
        : proj_{proj}, view_{view}, viewport_{viewport}, type_{type} {}

    cgmath::Mat44 proj_;
    cgmath::Mat44 view_;
    cgmath::Vec3 position_;
    cgmath::Vec2 scale_{1, 1};
    cgmath::Rect viewport_;
    rhi::TextureView target_;

    Type type_;
    cgmath::Color clearColor_{0.1, 0.1, 0.1, 1};

    void recalcView() {
        view_ = cgmath::CreateScale({scale_.x, scale_.y, 1.0}) *
                cgmath::CreateTranslation(position_);
    }
};

struct SphericalCoordCameraProxy final {
public:
    SphericalCoordCameraProxy(Camera& camera,
                              const nickel::cgmath::Vec3& center)
        : camera_{camera}, center_{center} {
        auto dir = camera_.Position() - center_;
        auto zAxis = nickel::cgmath::Vec3{0, 1, 0};
        radius_ = dir.Length();
        dir.Normalize();
        Assert(radius_ != 0, "your camera too close to origin (distance = 0)");

        auto xozVec = nickel::cgmath::Normalize(dir - (dir.Dot(zAxis) * zAxis));

        theta_ = nickel::cgmath::GetRadianIn360(
            nickel::cgmath::Vec3{0, 1, 0}, dir,
            nickel::cgmath::Cross({0, 1, 0}, xozVec));

        phi_ = nickel::cgmath::GetRadianInPISigned(
            nickel::cgmath::Vec2{1, 0},
            nickel::cgmath::Vec2{xozVec.x, xozVec.z});
    }

    void SetRadius(float r) {
        if (r < MinRadius) {
            radius_ = MinRadius;
        } else {
            radius_ = r;
        }
    }

    auto GetRadius() const { return radius_; }

    void SetPhi(float p) { phi_ = p; }

    auto GetPhi() const { return phi_; }

    void SetTheta(float t) {
        if (t > MaxTheta) {
            t = MaxTheta;
        } else if (t < MinTheta) {
            t = MinTheta;
        }
        theta_ = t;
    }

    auto GetTheta() const { return theta_; }

    void Update2Camera() {
        auto xoyAxis = nickel::cgmath::Vec2{std::cos(phi_), std::sin(phi_)} *
                       std::sin(theta_) * radius_;
        auto y = std::cos(theta_) * radius_;

        camera_.position_.Set(xoyAxis.x, y, xoyAxis.y);
        camera_.view_ =
            nickel::cgmath::LookAt(center_, camera_.position_, {0, 1, 0});
    }

private:
    Camera& camera_;
    nickel::cgmath::Vec3 center_;
    float radius_;
    float phi_;
    float theta_;

    static constexpr float MaxTheta = cgmath::PI - 0.001;
    static constexpr float MinTheta = 0.001;
    static constexpr float MinRadius = 0.0001;
};

}  // namespace nickel