#pragma once
#include "common/cgmath.hpp"
#include "graphics/rhi/adapter.hpp"

struct Camera {
public:
    friend class SphericalCoordCameraProxy;

    Camera(nickel::rhi::APIPreference api, const nickel::cgmath::Vec2& size)
        : proj_{nickel::cgmath::CreatePersp(nickel::cgmath::Deg2Rad(45.0f),
                                            size.w / size.h, 0.1, 10000,
                                            api == nickel::rhi::APIPreference::GL)} {}

    auto& View() const { return view_; }

    auto& Proj() const { return proj_; }

    auto& Position() const { return pos_; }

    void Move(const nickel::cgmath::Vec3& pos) { pos_ = pos; }

private:
    nickel::cgmath::Mat44 proj_;
    nickel::cgmath::Mat44 view_ = nickel::cgmath::Mat44::Identity();
    nickel::cgmath::Vec3 pos_;
};

struct SphericalCoordCameraProxy final {
public:
    SphericalCoordCameraProxy(Camera& camera,
                              const nickel::cgmath::Vec3& center)
        : camera_{camera}, center_{center} {
        auto dir = camera_.pos_ - center_;
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

    auto GetRadius() const {
        return radius_;
    }

    void SetPhi(float p) {
        phi_ = p;
    }

    auto GetPhi() const {
        return phi_;
    }

    void SetTheta(float t) {
        if (t > MaxTheta) {
            t = MaxTheta;
        } else if (t < MinTheta) {
            t = MinTheta;
        }
        theta_ = t;
    }

    auto GetTheta() const {
        return theta_;
    }

    void Update2Camera() {

        auto xoyAxis = nickel::cgmath::Vec2{std::cos(phi_), std::sin(phi_)} *
                       std::sin(theta_) * radius_;
        auto y = std::cos(theta_) * radius_;

        camera_.pos_.Set(xoyAxis.x, y, xoyAxis.y);
        camera_.view_ =
            nickel::cgmath::LookAt(center_, camera_.pos_, {0, 1, 0});
    }

private:
    Camera& camera_;
    nickel::cgmath::Vec3 center_;
    float radius_;
    float phi_;
    float theta_;

    static constexpr float MaxTheta = nickel::cgmath::PI - 0.001;
    static constexpr float MinTheta = 0.001;
    static constexpr float MinRadius = 0.0001;
};
